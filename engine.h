#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#define TILE_WIDTH 30
#define TILE_HEIGHT 30
using namespace std;

class Sprite{
    private:
        SDL_Rect position;
        SDL_Rect slice;
        SDL_Texture * texture;
    public:
    Sprite(int x, int y, int w, int h, int sx, int sy, int sw, int sh, SDL_Texture * txt){
        position.x = x;
        position.y = y;
        position.w = w;
        position.h = h;
        slice.x = sx;
        slice.y = sy;
        slice.w = sw;
        slice.h = sh;
        texture = txt;
    }
    void render(SDL_Renderer * renderer){
        SDL_RenderCopy(renderer, texture, &slice, &position);
    }
    int priority(){
        return position.y;
    }
    void setPosition(int x, int y, int w, int h){
        position.x = x;
        position.y = y;
        position.w = w;
        position.h = h;
    }
    void setSlice(int x, int y, int w, int h){
        slice.x = x;
        slice.y = y;
        slice.w = w;
        slice.h = h;
    }
};
bool colision(SDL_Rect a, SDL_Rect b){
	if(a.y+a.h <= b.y)
		return false;
	if(a.y>= b.y+b.h)
		return false;
	if(a.x+a.w<= b.x)
		return false;
	if(a.x>= b.x+b.w)
		return false;
	return true;
}
class GameMap;
class Camera{
	private:
		SDL_Rect coordinates;
	public:
		SDL_Rect GetCoordinates(){
			return coordinates;
		};
		void Move(map <int, bool> keyboard, GameMap stage);
		Camera(int x, int y, int w, int h){
			coordinates.x = x;
			coordinates.y = y;
			coordinates.w = w;
			coordinates.h = h;
		}
};

class Tile{
	private:
		SDL_Rect coordinates;
		int type;

	public:
			Tile(int x, int y, int t){
			coordinates.x = x;
			coordinates.y = y;
			coordinates.w = TILE_WIDTH;
			coordinates.h = TILE_HEIGHT;
			type = t;
		}
		int GetType(){
		return type;}
		SDL_Rect GetCoordinates(){
			return coordinates;
		}
		void Render(Camera cam, SDL_Renderer *render, vector<SDL_Texture*> texture){
			SDL_Rect local = coordinates;
			local.x -= cam.GetCoordinates().x;
			local.y -= cam.GetCoordinates().y;
			SDL_RenderCopy(render,texture[GetType()],NULL,&local);
		}
};
class GameMap{
	public:
		vector<Tile> maptiles;   // é privado, mas para efeito de construção é público enquanto não houver editor
		int MAP_WIDTH, MAP_HEIGHT;

		Tile GetTile(int i){
			return maptiles[i];
		}
		GameMap(int x, int y){
			MAP_WIDTH = x;
			MAP_HEIGHT = y;

		}
		int GetWidth(){
		
			return MAP_WIDTH;
		}
		int GetHeight(){
			return MAP_HEIGHT;
		}
		vector<Tile> GetMapTiles(){
			return maptiles;
		}
		int GetTileIndex(Camera cam, int mousex, int mousey){
			int xcomponent = (mousex+cam.GetCoordinates().x)/TILE_WIDTH;
			int ycomponent = (mousey+cam.GetCoordinates().y)/TILE_HEIGHT;
			int index = ycomponent * (MAP_WIDTH/TILE_WIDTH) + xcomponent;
			cout << index << endl;
			return index;
		}
		void OldRenderTiles(Camera cam, SDL_Renderer* render, vector<SDL_Texture*> textures){
			for (auto i = this->maptiles.begin();i!=this->maptiles.end();i++){
			if (colision(i->GetCoordinates(),cam.GetCoordinates())){
				i->Render(cam, render,textures);
			}
		}
}

};
void Camera::Move(map <int, bool> keyboard, GameMap stage){
			if (keyboard['i']==true){
				coordinates.y-=10;
				if (coordinates.y<0)
					coordinates.y=0;
			}
			if (keyboard['k']==true){
				coordinates.y+=10;
				if (coordinates.y> stage.GetHeight()-coordinates.h)
					coordinates.y=stage.GetHeight()-coordinates.h;
			}
			if (keyboard['j']==true){
				coordinates.x-=10;
				if (coordinates.x<0)
					coordinates.x=0;
			}
			if (keyboard['l']==true){
				coordinates.x+=10;
				if (coordinates.x>stage.GetWidth()-coordinates.w)
					coordinates.x=stage.GetWidth()-coordinates.w;
			}
		}

class Game {
    private:
        SDL_Window * window = NULL;

        int SCREEN_WIDTH;
        int SCREEN_HEIGHT;

        bool running;

        SDL_Event event;
        SDL_Renderer* renderer = NULL;

    public:

        map<int, bool> keyboard;
        vector<int> pressedKeys;
        vector<Sprite*> sprites;

        SDL_Renderer* getRenderer(){
            return renderer;
        }

        int getScreen_Width() const { return SCREEN_WIDTH;}
        int getScreen_Height() const { return SCREEN_HEIGHT;}


        int init(int width, int height, const char * name){
            SCREEN_WIDTH = width;
            SCREEN_HEIGHT = height;
            if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
            {
                printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
                return 1;
            }
            window = SDL_CreateWindow(
                name, 
                SDL_WINDOWPOS_UNDEFINED, 
                SDL_WINDOWPOS_UNDEFINED, 
                SCREEN_WIDTH, 
                SCREEN_HEIGHT, 
                SDL_WINDOW_SHOWN 
            );
            if( window == NULL){
                printf("Window could not be created! %s\n", SDL_GetError());
                return 2;
            }

            renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL){
                printf("Could not create renderer! %s\n", SDL_GetError());
                return 3;
            }
            if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0){
                printf("Could not initialize images! %s\n", SDL_GetError());
                return 4;
            }
            //set variables
            running = true;

            return 0;
        }

        bool update(int delay){
            SDL_Delay(delay);
            return running;
        }

        void getEvents(){
            // Input
            while( SDL_PollEvent(&event))
            {
                int key = event.key.keysym.sym;
                switch(event.type)
                {
                case SDL_QUIT:
                    running = false;
                case SDL_KEYDOWN:
                    pressedKeys.push_back(key);
                    keyboard[key] = true;
                    break;
                case SDL_KEYUP:
                    keyboard[key] = false;
                    break;
                }
            }
        }

        Sprite * createSprite(int x, int y, int w, int h, int sx, int sy, int sw, int sh, SDL_Texture * txt){
            sprites.push_back(new Sprite(x, y, w, h, sx, sy, sw, sh, txt));
            return sprites.back();
        }

        void draw(GameMap map, Camera cam,vector<SDL_Texture*> textures ){
            //background
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
            SDL_RenderClear( renderer );
			
			map.OldRenderTiles(cam, renderer,textures);

            sort(sprites.begin(), sprites.end(), [ ]( const auto& lhs, const auto& rhs )
            {
                return lhs->priority() < rhs->priority();
            });

            //sprites
            for(Sprite * s : sprites){
                s->render(renderer);
            }
            //update screen
            SDL_RenderPresent(renderer);
        }

        void Quit(){
            running = false;
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            IMG_Quit();
            SDL_Quit();
        }
};

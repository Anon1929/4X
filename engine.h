#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#define TILE_WIDTH 30
#define TILE_HEIGHT 30
using namespace std;

class Camera{
	private:
		int x_;
        int y_;
        int w_;
        int h_;
        float ratio;
        SDL_Renderer * renderer;
	public:
		SDL_Rect GetCoordinates(){
			SDL_Rect coordinates;
            coordinates.x = x_;
            coordinates.y = y_;
            coordinates.w = w_;
            coordinates.h = h_;
            return coordinates;
		};
        void setRatio(int r){
            ratio = r;
        }
        void changeRatio(float a){
            ratio = ratio*a;
        }
        float getRatio(){
            return ratio;
        }
        void move(int x, int y){
            x_ += x / ratio;
            y_ += y / ratio;
        }
		void init(SDL_Renderer * rend, int x, int y, int w, int h, int r){
			x_ = x;
			y_ = y;
			w_ = w;
			h_ = h;
            ratio = r;
            renderer = rend;
		}
        void render(SDL_Texture* texture, SDL_Rect s, SDL_Rect p){
            p.x = int((p.x - x_)*ratio);
            p.y = int((p.y - y_)*ratio);
            p.w = int(p.w *ratio);
            p.h = int(p.h *ratio);
            SDL_Rect position = p;
            SDL_Rect slice = s;
            SDL_RenderCopy(renderer, texture, &slice, &position);
        }
};

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
    void render(Camera camera){
        camera.render(texture, slice, position);
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

class Tile{
	public:
        int type;
        Tile(){
            type=0;
        }
};

class GameMap{
    private:
		int MAP_WIDTH = 0;
        int MAP_HEIGHT = 0;
        vector<Tile> tileMap;
        SDL_Texture * texture;
	public: 
		void init(int x, int y, SDL_Texture * t){
			MAP_WIDTH = x;
			MAP_HEIGHT = y;
            tileMap = vector<Tile>(x*y);
            texture = t;
		}

        void renderTiles(Camera camera){
            SDL_Rect pos = camera.GetCoordinates();
            float ratio = camera.getRatio();
            int sY = pos.y/TILE_HEIGHT;
            int sX = pos.x/TILE_WIDTH;
            int pY = pos.y+pos.h/ratio;
            int eY = pY/TILE_WIDTH +1;
            int pX = pos.x+pos.w/ratio;
            int eX = pX/TILE_HEIGHT +1;
            if(sX<0)sX=0;
            if(sY<0)sY=0;
            if(eX>MAP_WIDTH)eX=MAP_WIDTH;
            if(eY>MAP_HEIGHT)eY=MAP_HEIGHT;

            for(int j = sY; j < eY; j++){
                for(int i = sX; i < eX; i++){
                    SDL_Rect position;
                    position.x = i * TILE_WIDTH;
                    position.y = j * TILE_HEIGHT;
                    position.w = TILE_WIDTH;
                    position.h = TILE_HEIGHT;
                    SDL_Rect slice;
                    slice.w=16;
                    slice.h=16;
                    slice.x = 16*(tileMap[i + j*MAP_WIDTH].type%16);
                    slice.y = 16*(tileMap[i + j*MAP_WIDTH].type/16);
                    camera.render(texture, slice, position);
                }
            }
        }

		int GetTileIndex(Camera camera, int mousex, int mousey){
            SDL_Rect pos = camera.GetCoordinates();
            float ratio = camera.getRatio();
            int sY = (pos.y+mousey/ratio)/TILE_HEIGHT;
            int sX = (pos.x+mousex/ratio)/TILE_WIDTH;
            cout << sX << " " << sY << endl;
			return sY + sX*MAP_WIDTH;
		}

        void updateTile(Camera camera, int mousex, int mousey){
            SDL_Rect pos = camera.GetCoordinates();
            float ratio = camera.getRatio();
            int sY = (pos.y+mousey/ratio)/TILE_HEIGHT;
            int sX = (pos.x+mousex/ratio)/TILE_WIDTH;
            tileMap[sX + sY*MAP_WIDTH].type = 1;
		}

};

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
        vector<pair<int, int>> mouseClicks;
        vector<int> pressedKeys;
        vector<Sprite*> sprites;

        SDL_Renderer* getRenderer(){
            return renderer;
        }

        Camera camera;
        GameMap gameMap;

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
                case SDL_MOUSEBUTTONDOWN:
                        pair<int, int> click;
                        SDL_GetMouseState(&click.first, &click.second);
                        mouseClicks.push_back(click);
                    break;
                }
            }
        }

        Sprite * createSprite(int x, int y, int w, int h, int sx, int sy, int sw, int sh, SDL_Texture * txt){
            sprites.push_back(new Sprite(x, y, w, h, sx, sy, sw, sh, txt));
            return sprites.back();
        }

        void draw( ){
            //background
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
            SDL_RenderClear( renderer );
			
			gameMap.renderTiles(camera);

            sort(sprites.begin(), sprites.end(), [ ]( const auto& lhs, const auto& rhs )
            {
                return lhs->priority() < rhs->priority();
            });

            //sprites
            for(Sprite * s : sprites){
                s->render(camera);
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

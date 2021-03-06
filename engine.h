#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
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
    void setPosition(int x, int y){
        position.x = x;
        position.y = y;
    }
    void setSlice(int w, int h){
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

typedef struct Rect{
    float x;
    float y;
    float w;
    float h;
}Rect;

class GameObject{
    protected:
    Rect rect;
    Sprite * sprite = nullptr;
    public:
    GameObject(float x, float y, float w, float h){
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
    }
    SDL_Rect getRect(){
        SDL_Rect r;
        r.x = rect.x;
        r.y = rect.y;
        r.w = rect.w;
        r.h = rect.h;
        return r;
    }
    void setSprite(Sprite * sp){
        sprite = sp;
    }
    void translate(float x, float y){
        rect.x += x;
        rect.y += y;
        sprite->setPosition(int(rect.x), int(rect.y));
    }
};
class TextWrapper{
	private:
		vector <pair<SDL_Texture*,map<int,SDL_Rect>>> fonts;
		queue<tuple<char *,SDL_Rect,SDL_Color,int>> textqueue;
	
	public:
		void CreateFontAtlas(SDL_Renderer *render,string filename){
			TTF_Font * newfont;
			SDL_Color color = {255,255,255};
			map <int,SDL_Rect> AtlasSlice;
			char letter[2];
			newfont = TTF_OpenFont(filename.c_str(), 50);
			SDL_Surface *surface, *text;
			surface = SDL_CreateRGBSurface(0, 500, 500, 32, 0, 0, 0, 0xff);
			SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
			SDL_Rect dest;
			dest.x = dest.y = 0;
			int i;
			for (i = ' '; i <='z';i++){
				letter[0] = i;
				letter[1] = 0;
				text = TTF_RenderText_Blended(newfont,letter,color);
				TTF_SizeText(newfont,letter ,&dest.w, &dest.h);
				if (dest.x+dest.w >= 500){
					dest.x = 0;
					dest.y += dest.h + 1;
				}
				SDL_BlitSurface(text, NULL, surface, &dest);
				AtlasSlice[i].x= dest.x;
				AtlasSlice[i].y= dest.y;
				AtlasSlice[i].w= dest.w;
				AtlasSlice[i].h= dest.h;
				SDL_FreeSurface(text);
				dest.x+=dest.w;
			}
			TTF_CloseFont(newfont);
			SDL_Texture * AtlasTexture = SDL_CreateTextureFromSurface(render, surface);
			pair<SDL_Texture*,map<int,SDL_Rect>> createdfont = {AtlasTexture,AtlasSlice};
			fonts.emplace_back(createdfont);
		}
		void QueueText(char const *text,int font, int x, int y,int w, int h, unsigned char  r, unsigned char g, unsigned char  b){
			char * t = (char*)text;
			SDL_Rect pos = {x,y,w,h};
			SDL_Color color = {r,g,b};
			tuple <char *,SDL_Rect,SDL_Color,int> newtext(t,pos,color,font);
			textqueue.push(newtext);
		}
		void DrawText(SDL_Renderer* render){
			while(!textqueue.empty()){
				auto text = textqueue.front();
				int i, character;
				char * word = get<0>(text);
				SDL_Rect pos = get<1>(text);
				SDL_Color color = get<2>(text);
				int fontselect = get<3>(text);
				SDL_Rect *slice, dest;
				if(fonts.size()==0){
					cout << "No FontAtlas loaded" <<endl;
				}
				else{
					fontselect = fontselect%fonts.size();
					SDL_SetTextureColorMod(fonts[fontselect].first,color.r,color.g,color.b);
					i = 0;
					character = word[i++];
					while (character){
						slice = &fonts[fontselect].second[character];
						
						dest.x = pos.x;
						dest.y = pos.y;
						dest.w = pos.w;  // slice->w/h for size in font texture
						dest.h = pos.w;
						SDL_RenderCopy(render,fonts[fontselect].first , slice, &dest);
						pos.x += pos.w;
						character = word[i++];
					}
				}
				textqueue.pop();
			}
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

struct Tile{
	public:
        int type;
        Tile(int t=0){
            type=t;
        }
};

class GameMap{
    private:
		int MAP_WIDTH = 0;
        int MAP_HEIGHT = 0;
        vector<Tile> tileMap;
        SDL_Texture * texture;
	public: 
		void init(int x, int y, SDL_Texture * t,vector<Tile> v){
			MAP_WIDTH = x;
			MAP_HEIGHT = y;
            //tileMap = vector<Tile>(x*y);
            texture = t;
			tileMap = v;
		}
		void UpdateTileMap(vector<Tile> newtilemap){
			tileMap = newtilemap;
		}
		Tile* GetTile(int i){
			return &tileMap[i];
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
			return sX + sY*MAP_WIDTH;
		}

        void EditorUpdateTile(Camera camera, int mousex, int mousey){
			int index = GetTileIndex(camera,mousex,mousey);
            tileMap[index].type = (tileMap[index].type+1)%3;
		}

};

vector<Tile> CreateOrLoadMap(int &x,int &y,string &filename){
	vector<Tile> stage;
	ifstream infile;
	ofstream outfile;
	cout <<"Enter map name:";
	cin >> filename;
	filename.append(".bin");
	infile.open(filename, ios::in | ios::binary);
	if (infile){
		cout<<"Reading file"<<endl;
		infile.read(reinterpret_cast<char*>(&x), sizeof(int));
		infile.read(reinterpret_cast<char*>(&y), sizeof(int));
		cout <<outfile.tellp()<< endl;
		for( int i = 0; i <x*y; ++i ){
			stage.emplace_back(Tile());
			infile.read(reinterpret_cast<char*>(&stage[i]),sizeof(Tile));
		}
		infile.close();
	}
	else{
		cout<< "No file opened, creating a new file" << endl;
		outfile.open(filename, ios::out | ios::binary);
		if (outfile){
			cout<<"New file created" <<endl;
			cout<<"Input Horizontal Number of Tiles:";
			cin >> x;
			cout<<"Input Vertical Number of Tiles:";
			cin >> y;
			outfile.write(reinterpret_cast<char*>(&x), sizeof(int));
			outfile.write(reinterpret_cast<char*>(&y), sizeof(int));
			for (int i =0;i<x*y; ++i){
				stage.emplace_back(Tile());
				outfile.write(reinterpret_cast<char*>(&stage[i]), sizeof(Tile));

			}
			outfile.close();
		}
		else{
			cout << "Unable to create file"<<endl;
		}
	}
	return stage;
}
void SaveMap(int x, int y, string filename, GameMap map){
	ofstream outfile;
	outfile.open(filename, ios::out | ios::binary);
	if (outfile){
		outfile.write(reinterpret_cast<char*>(&x), sizeof(int));
		outfile.write(reinterpret_cast<char*>(&y), sizeof(int));
		for( int i = 0; i < x*y; ++i ){
			outfile.write(reinterpret_cast<char*>(map.GetTile(i)),sizeof(Tile));
		}
				outfile.close();
				cout << "Save Complete" << endl;
			}
			else{
				cout << "Unable to save map" <<endl;
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
        vector<pair<int, int>> mouseClicks;
        vector<int> pressedKeys;
        vector<Sprite*> sprites;
        vector<GameObject*> gameObjects;

        SDL_Renderer* getRenderer(){
            return renderer;
        }

        Camera camera;
        GameMap gameMap;
		TextWrapper textwrapper;

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
			if (TTF_Init() <0){
                printf("Could not initialize fonts! %s\n", SDL_GetError());
				return 5;
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
        GameObject * createGameObject(int x, int y, int w, int h, int sx, int sy, int sw, int sh, SDL_Texture * txt){
            sprites.push_back(new Sprite(x, y, w, h, sx, sy, sw, sh, txt));
            gameObjects.push_back(new GameObject(x, y, w, h));
            gameObjects.back()->setSprite(sprites.back());
            return gameObjects.back();
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
			textwrapper.DrawText(renderer);

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


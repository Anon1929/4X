#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

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

        int getScreen_Width() const { return SCREEN_WIDTH;}
        int getScreen_Height() const { return SCREEN_HEIGHT;}

        int init(int width, int height, const char * name){
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
            SCREEN_WIDTH = width;
            SCREEN_HEIGHT = height;
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
        void Quit(){
            running = false;
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            IMG_Quit();
            SDL_Quit();
        }
};
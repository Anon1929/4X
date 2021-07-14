#include "engine.h"

using namespace std;

int main(){
    Game game;
    game.init(300, 300, "bicicleta");

    SDL_Texture * characterTexture = IMG_LoadTexture(game.getRenderer(), "./sprite_sheet.png" );
    
    Sprite * player = game.createSprite(50, 50, 128, 128, 0, 0, 32, 32, characterTexture );
    game.createSprite(40, 40, 128, 128, 0, 0, 32, 32, characterTexture );
    while(game.update(20)){
        game.getEvents();
        game.draw();

        if(game.keyboard['w'] == true){
            player->transform(0, -5);
        }else if(game.keyboard['s'] == true){
            player->transform(0, 5);
        }else if(game.keyboard['a'] == true){
            player->transform(-5, 0);
        }else if(game.keyboard['d'] == true){
            player->transform(5, 0);
        }

    }

    return 0;
}
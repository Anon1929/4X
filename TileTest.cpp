#include "engine.h"

using namespace std;

class Npc{
    public:
    GameObject * gameObject;
    Game * game;
    Npc(Game * game_, GameObject * gameObject_){
        gameObject = gameObject_;
        game = game_;
    }
    void update(){
        gameObject->translate(rand()%10 -5, rand()%10 -5);
    }
};

int main(){
    Game game;
    game.init(600, 600, "bicicleta");

    SDL_Texture * tileTexture =IMG_LoadTexture(game.getRenderer(), "./tiles.png");
    SDL_Texture * characterTexture = IMG_LoadTexture(game.getRenderer(), "./sprite_sheet.png" );

	int x,y;
	string filename;
    game.camera.init(game.getRenderer(), 0, 0, game.getScreen_Width(), game.getScreen_Height(), 2);
    game.gameMap.init(x, y, tileTexture,CreateOrLoadMap(x,y,filename ));
    
    Npc npc(&game, game.createGameObject(300, 300, 128, 128, 0, 0, 32, 32, characterTexture));

    while(game.update(20)){
        game.getEvents();
        game.draw();

        npc.update();

        while(game.pressedKeys.size()>0){
            int key = game.pressedKeys.back();
            game.pressedKeys.pop_back();
            switch (key)
            {
            case 'z':
                game.camera.changeRatio(2);
                break;
            case 'x':
                game.camera.changeRatio(0.5);
                break;
            
            default:
                break;
            }
        }
        while(game.mouseClicks.size()>0){
            pair<int, int> click = game.mouseClicks.back();
            game.mouseClicks.pop_back();
        }

        if(game.keyboard['d']){
            game.camera.move(5, 0);
        }else if(game.keyboard['a']){
            game.camera.move(-5, 0);
        }else if(game.keyboard['w']){
            game.camera.move(0, -5);
        }else if(game.keyboard['s']){
            game.camera.move(0, 5);
        }

    }
    return 0;
}
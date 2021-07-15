#include "engine.h"

using namespace std;

int main(){
    Game game;
    game.init(300, 300, "bicicleta");

    SDL_Texture * characterTexture = IMG_LoadTexture(game.getRenderer(), "./sprite_sheet.png" );
	vector<SDL_Texture*> tiletextures;
    tiletextures.emplace_back(IMG_LoadTexture(game.getRenderer(), "tile01.png" ));
    tiletextures.emplace_back(IMG_LoadTexture(game.getRenderer(), "tile02.png" ));
    tiletextures.emplace_back(IMG_LoadTexture(game.getRenderer(), "tile03.png" ));
	Sprite * player = game.createSprite(50, 50, 128, 128, 0, 0, 32, 32, characterTexture );
    game.createSprite(40, 40, 128, 128, 0, 0, 32, 32, characterTexture );
	Camera cam(0,0,game.getScreen_Width(),game.getScreen_Height());
	GameMap stage(600,600);
	for (int i =0;i<400;i++){
		stage.maptiles.emplace_back(Tile((i*TILE_WIDTH)%stage.GetWidth(),((i*TILE_WIDTH)/stage.GetWidth())*TILE_HEIGHT,i%3));
		//cout << stage.GetMapTiles()[i].GetCoordinates().x << endl;
	}

    while(game.update(20)){
        game.getEvents();
		cam.Move(game.keyboard,stage);
        game.draw(stage,cam,tiletextures);
    }

    return 0;
}

#include "../include/GameWorld.h"

#include <cstdio>

GameWorld::GameWorld()
{
    ballIdx = racket1Idx = racket2Idx = -1;
    paused = true;
    EntityList.reserve(10);
}

bool GameWorld::OnLoop()
{
    bool Running = OnEvent();

    if(paused) return Running;

    for(auto i = EntityList.begin(); i != EntityList.end(); ++i)
        i->OnLoop();

    for(auto i = EntityList.begin(); i != EntityList.end(); ++i)
        i->OnMove();

    CollisionChecker();

    return Running;
}

void GameWorld::addEntity(Entity *newEntity)
{
    EntityList.push_back(*newEntity);

    if(newEntity->getType() == BALL) ballIdx = (int)EntityList.size() - 1;
    if(newEntity->getType() == RACKET)
    {
        if(racket1Idx < 0) racket1Idx = (int)EntityList.size() - 1;
        else racket2Idx = (int)EntityList.size() - 1;
    }
}

bool GameWorld::OnEvent()
{
    SDL_PollEvent(&event);

    if(event.type == SDL_QUIT) return false;

    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_p) paused = !paused;
        if(event.key.keysym.sym == SDLK_r)
        {
            paused = true;
            EntityList[ballIdx].getRect()->x = 385;
            EntityList[ballIdx].getRect()->y = 235;
            EntityList[ballIdx].setX(385);
            EntityList[ballIdx].setY(235);
        }
        else if(event.key.keysym.sym == SDLK_ESCAPE) return false;
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if(keystate[SDL_SCANCODE_W]) EntityList[racket1Idx].setYDir(UP);
    if(keystate[SDL_SCANCODE_S]) EntityList[racket1Idx].setYDir(DOWN);
    if(keystate[SDL_SCANCODE_UP]) EntityList[racket2Idx].setYDir(UP);
    if(keystate[SDL_SCANCODE_DOWN]) EntityList[racket2Idx].setYDir(DOWN);

    return true;
}

void GameWorld::CollisionChecker()
{
    for(int i = 0; i < (int)EntityList.size(); ++i)
    {
        if(!EntityList[i].isCollidable() || i == ballIdx) continue;

        if(SDL_HasIntersection(EntityList[ballIdx].getRect(), EntityList[i].getRect()) == SDL_TRUE)
        {
            if(EntityList[i].getType() == RACKET)
                EntityList[ballIdx].OnCollision(EntityList[i].getMidPoint() -
                                                EntityList[ballIdx].getMidPoint());
            else
                EntityList[ballIdx].OnCollision();
        }
    }
}

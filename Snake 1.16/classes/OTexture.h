/*
Class used for the objects like apples, score,
backgrounds, ecc..
*/

#include <stdlib.h>
#include <time.h>

#include "Texture.h"

#ifndef OTEXTURE_H
#define OTEXTURE_H



class OTexture : public Texture
{
	public:
		OTexture();
		~OTexture();
		
		virtual void render(int width, int height, SDL_Rect* clip = NULL, char direction = '0', SDL_Point* center = NULL, SDL_RendererFlip filp = SDL_FLIP_NONE) override;
		
		//get the coordinates
		int getXPos();
		int getYPos();
		
		//reset the position of the object randomly
		void resetPosition(int screenWidth, int screenHeight, int replacer = 0);
		
		//the object coordinates are passed
		void setPosition(int x, int y);
		
	private:
		int xPos;
		int yPos;
};

#endif

/*
The class used for rendering the snake
*/



#ifndef LTEXTURE_H
#define LTEXTURE_H

#include "Texture.h"

class LTexture : public Texture
{
	public:
		LTexture();
		~LTexture();
		
		virtual void render(int x, int y, SDL_Rect* clip = NULL, char direction = '\0', SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) override;
				
		//store the direction of the snake
		void setBeforeDirection(char beforeDirection);
		
		//render particles for graphic effect
		void renderParticles(int x, int y);
		
		//set the type of snake pice
		void setType(std::string type);
		
	protected:
		std::string mType;
		char mBeforeDirection;
		
		
};

#endif

/*
This class is used for text rendering in
menues
*/

#include "SDL2/SDL_ttf.h"

#include "Texture.h"

#ifndef TTEXTURE_H
#define TTEXTURE_H

class TTexture : public Texture
{
	public:
		TTexture();
		~TTexture();
		
		virtual void render(int x, int y, SDL_Rect* clip = NULL, char direction = '0', SDL_Point* center = NULL, SDL_RendererFlip filp = SDL_FLIP_NONE) override;
		
		//Creates the texture from an input text
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor = {0, 0, 0}, TTF_Font *font = NULL);

		//Set text position.
		void setPosition(int x, int y);
		
		//handle the mouse click event
		void handleEvent(SDL_Event* e, int xPos = 0, int yPos = 0);
		
	private:
		//The position of the text
		SDL_Point mPosition;
};

#endif

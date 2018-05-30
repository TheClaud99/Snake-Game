#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "globals.inc"

#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
	public:
		
		Texture();
		~Texture();
		
		//Clear all values.
		void free();
		
		//Load the texture from an image file.
		bool loadFromFile(std::string path);
		
		//Render the object.
		virtual void render(int x, int y, SDL_Rect* clip = NULL, char direction = '0', SDL_Point* center = NULL, SDL_RendererFlip filp = SDL_FLIP_NONE) = 0;
		
		int getWidth() const;
		int getHeight() const;
		
		//Set the texture color rendering.
		void setTextureColorMod(SDL_Color);
		
		//Set the texture alpha rendering.
		void setAlpha(Uint8 alpha);

	protected:
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};

#endif

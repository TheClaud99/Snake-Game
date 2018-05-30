#include "Texture.h"

Texture::Texture()
{
	free();
}

Texture::~Texture()
{
    free();
}

void Texture::free()
{
    if(mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;	
	}
	
	mWidth = 0;
	mHeight = 0;
}

bool Texture::loadFromFile(std::string path)
{
	free();
	bool success = true;
	SDL_Texture* newTexture = NULL;
	
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(loadedSurface == NULL)
	{
		success = false;
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface -> format, 0, 255, 255));
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if(newTexture == NULL)
		{
			success = false;
		}
		else
		{
			mTexture = newTexture;
			mWidth = loadedSurface -> w;
			mHeight = loadedSurface -> h;
		}
	}
	
	SDL_FreeSurface(loadedSurface);
	return success;
}


void Texture::render(int x, int y, SDL_Rect* clip, char direction, SDL_Point* center, SDL_RendererFlip)
{
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};
	if(clip != NULL)
	{
		renderQuad.w = clip -> w;
		renderQuad.h = clip -> h;
	}
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

void Texture::setTextureColorMod(SDL_Color color)
{
	SDL_SetTextureColorMod(mTexture, color.r, color.g, color.b);
}


int Texture::getWidth() const
{
    return mWidth;
}

int Texture::getHeight() const
{
    return mHeight;
}

void Texture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

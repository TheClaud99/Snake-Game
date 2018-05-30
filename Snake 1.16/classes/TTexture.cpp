#include "TTexture.h"

TTexture::TTexture() : Texture()
{

	mPosition.x = 0;
	mPosition.y = 0;
}

TTexture::~TTexture()
{
	free();
	mPosition.x = 0;
	mPosition.y = 0;
}


bool TTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font *font)
{

	bool success = true;
	free();
	
	if(font == NULL)
	{
		font = gFont;
	}
	
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);
	
	if(textSurface == NULL && textureText != "")
	{
		std::printf("Error creating the TTF text surface\n");
		success = false;
	}
	else
	{
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if(mTexture != NULL)
		{
			mWidth = textSurface -> w;
			mHeight = textSurface -> h;
		}
		else
		{
			success = false;
		}
		SDL_FreeSurface(textSurface);
	}
	
	textSurface = NULL;
	delete textSurface;
	
	return success;
}

void TTexture::render(int x, int y, SDL_Rect* clip, char direction, SDL_Point* center, SDL_RendererFlip filp)
{
	if(mPosition.x == 0 && mPosition.y == 0)
	{
		mPosition.x = x;
		mPosition.y = y;	
	}
	
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};
	if(clip != NULL)
	{
		renderQuad.w = clip -> w;
		renderQuad.h = clip -> h;
	}
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

void TTexture::setPosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

void TTexture::handleEvent(SDL_Event* e, int xPos, int yPos)
{
	TTexture::setPosition(xPos, yPos);
	if(e -> type == SDL_MOUSEBUTTONDOWN || e -> type == SDL_MOUSEMOTION)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		
		bool inside = true;
		
		if(x < mPosition.x || x > mPosition.x + mWidth)
		{
			inside = false;
		}
		else if(y < mPosition.y || y > mPosition.y + mHeight)
		{
			inside = false;
		}
		
		if(inside)
			std::printf("over\n");
	}
}

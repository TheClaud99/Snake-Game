#include "OTexture.h"

OTexture::OTexture() : Texture()
{
	xPos = 0;
	yPos = 0;
}

OTexture::~OTexture()
{
	free();
	xPos = 0;
	yPos = 0;
}

int OTexture::getXPos()
{
	return xPos / BOX_SIZE;
}

int OTexture::getYPos()
{
	return yPos / BOX_SIZE;
}

void OTexture::render(int width, int height, SDL_Rect* clip, char direction, SDL_Point* center, SDL_RendererFlip filp)
{

	if(!width || !height)
	{
		width = mWidth;
		height = mHeight;
	}
	
	SDL_Rect renderQuad{xPos, yPos, width, height};
	if(clip != NULL)
	{
		mWidth = clip -> w;
		mHeight = clip -> h;
	}
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

void OTexture::resetPosition(int screenWidth, int screenHeight, int replacer)
{
	srand(time(NULL) + replacer);
	xPos = (rand() % screenWidth) * BOX_SIZE;
	
	srand(time(NULL) + replacer + 1);
	yPos = (rand() % screenHeight) * BOX_SIZE;
}

void OTexture::setPosition(int x, int y)
{
	xPos = x;
	yPos = y;
}

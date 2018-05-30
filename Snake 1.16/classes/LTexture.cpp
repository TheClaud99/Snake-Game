#include "LTexture.h"

LTexture::LTexture() : Texture()
{
	
	mType = "";
	mBeforeDirection = '0';
}

LTexture::~LTexture()
{
	free();
}


void LTexture::render(int x, int y, SDL_Rect* clip, char direction, SDL_Point* center, SDL_RendererFlip flip)
{
	double angle = 0.0;
	SDL_Rect renderQuad {x * BOX_SIZE, y * BOX_SIZE, BOX_SIZE, BOX_SIZE};
	if(clip != NULL)
	{
		renderQuad.w = clip -> w;
		renderQuad.h = clip -> h;
	}
	
	if(mType != "turn")
	{
		switch (direction)
		{
			case 'u':
				angle = 270;
				break;
		
			case 'd':
				angle = 90;
				break;
				
			case 'l':
				angle = 180;
				break;
			
			case 'r':
				angle = 0;
				break;
		}
	}
	else 
	{
		
		switch (direction)
		{
			case 'u':
				if(mBeforeDirection == 'u')
				{
					flip = SDL_FLIP_NONE;
				}
				else if(mBeforeDirection == 'r')
				{
					flip = SDL_FLIP_VERTICAL;
				}
				angle = 270;
				break;
			
			case 'd':
				if(mBeforeDirection == 'r')
				{
					flip = SDL_FLIP_NONE;
				}
				else if(mBeforeDirection == 'l')
				{
					flip = SDL_FLIP_VERTICAL;
				}
				angle = 90;
				break;
				
			case 'l':
				if(mBeforeDirection == 'd')
				{
					flip = SDL_FLIP_NONE;
				}
				else if(mBeforeDirection == 'u')
				{
					flip = SDL_FLIP_VERTICAL;
				}
				angle = 180;
				break;
			
			case 'r':
				if(mBeforeDirection == 'u')
				{
					flip = SDL_FLIP_NONE;
				}
				else if(mBeforeDirection == 'd')
				{
					flip = SDL_FLIP_VERTICAL;
				}
				angle = 0;
				break;
		}
			
	}
	
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::setBeforeDirection(char beforeDirection)
{
	mBeforeDirection = beforeDirection;
}

void LTexture::renderParticles(int x, int y)
{
	SDL_Rect renderQuad {x, y, mWidth, mHeight};
	SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

void LTexture::setType(std::string type)
{
	mType = type;
}

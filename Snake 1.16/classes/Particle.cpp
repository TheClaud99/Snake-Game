#include "Particle.h"

Particle::Particle(int x, int y, char direction)
{
	x *= BOX_SIZE;
	y *= BOX_SIZE;
	
	mPosX = x + rand() % RANDOMIZING;
	mPosY = y + rand() % RANDOMIZING;
	
	if(direction)
	{
		switch(direction)
		{
			case 'r':
				mPosX -= PARTICLES_DISTANCE;
				break;
			
			case 'l':
				mPosX += PARTICLES_DISTANCE;
				break;
				
			case 'u':
				mPosY += PARTICLES_DISTANCE;
				break;
			
			case 'd':
				mPosY -= PARTICLES_DISTANCE;
				break;
		}
	}
	
	
	mFrame = rand() % 5;
	
	switch(rand() % 3)
	{
		case 0:
			mTexture = (LTexture*)mRedTexture;
			break;
		case 1:
			mTexture = (LTexture*)mBlueTexture;
			break;
		case 2:
			mTexture = (LTexture*)mGreenTexture;
			break;
	}
	
	gShimmerTexture = (LTexture*)mShimmerTexture;
}


void Particle::render()
{
	mTexture -> renderParticles(mPosX, mPosY);
	
	if(mFrame % 2 == 0)
	{
		gShimmerTexture -> renderParticles(mPosX, mPosY);
	}
	
	mFrame++;
}

bool Particle::isDead()
{
	return mFrame > 1000;
}


int Particle::getX()
{
	return mPosX;
}

int Particle::getY()
{
	return mPosY;
}

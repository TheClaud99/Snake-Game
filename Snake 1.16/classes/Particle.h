/*
The class for particle effects
*/

#ifndef PARTICLE_H
#define PARTICLE_H

#include "LTexture.h"
#include <stdlib.h>
#include "globals.inc"

class Particle
{
	public:
		Particle(int x, int y, char direction = '\0');
		
		void render();
		
		bool isDead();
		
		int getX();
		
		int getY();
		
	private:
		int mPosX;
		int mPosY;
		
		int mFrame;
		
		LTexture *mTexture;
		
		LTexture* gShimmerTexture;
		
		const int RANDOMIZING = 50;
};

#endif

/*
This is the main file of the program. Here there
are functions and the main part of game execution
*/

//System libraries.
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <time.h>

//SDL libraries.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

//Class for snake rendering.
#include "classes/LTexture.h"

//Class for objects like the score and the apple.
#include "classes/OTexture.h"

//Class for the text.
#include "classes/TTexture.h"

//Class for particles.
#include "classes/Particle.h"

//Global variables.
#include "classes/globals.inc"

/*/Uncomment on Windows/*/
#include "Winsock.h"

/*/Uncomment on Linux/*/
//#include "classes/Socket.h"


using namespace std;

//Size in px of the the boxes.
const int BOX_SIZE = 50;

//The window width and height.
const int SCREEN_WIDTH = 16 * BOX_SIZE;
const int SCREEN_HEIGHT = 12 * BOX_SIZE;

//Max of reachable snake pieces.
const int MAX_PIECES = 1000;

//Speed of the snake.
const int SPEED = 250;

//Numbero of stored records.
const int RECORD_NUMBER = 3;

//Size of text.
const int TEXT_SIZE = 30;

//Snake pieces on start.
const int INITIAL_SNAKE_PIECES = 3;

//Pickup spawn probability.
const int PICKUP_SPAWN_PROB = 20;

//Port for multiplayer.
const int PORT = 25565;

//Number of particles.
const int TOTAL_PARTICLES = 500;

//Distance of the particles.
const int PARTICLES_DISTANCE = 5;

//Cordinates of every snake's pieces.
struct coordinates
{
	int x;
	int y;
	char direction;
}
snakeHead, snakeTail;

//Add string "type" to coordinates for storing snake piece's type.
struct bodyPiece
{
	coordinates c;
	string type;
}
snakePieces[MAX_PIECES];

enum pickups
{
	DISCOBALL,
	PORTAL,
	TOTAL_PICKUPS,
};

bool activatedPickups[TOTAL_PICKUPS];


//Rendering color.
SDL_Color gColor;

//Class for socket communication.
Socket net;

//music effects
Mix_Music *gMusic = NULL;
Mix_Chunk *gAppleBiteSound = NULL;
Mix_Chunk *gSplatSound = NULL;

//Timer for the movement.
SDL_TimerID timer;

//The main window.
SDL_Window* gWindow = NULL;

//Icon surface.
SDL_Surface* gIconSurface = NULL;

//Variable used for rendering.
SDL_Renderer* gRenderer = NULL;

//Score draw area.
SDL_Rect gScoreClips[10];

//Snake textures.
LTexture gSnakeHeadSprite;
LTexture gSnakeBodySprite;
LTexture gSnakeTurnSprite;
LTexture gSnakeTailSprite;

//Particles textures.
LTexture gRedTexture;
LTexture gBlueTexture;
LTexture gGreenTexture;
LTexture gShimmerTexture;
void* mShimmerTexture = (void*)&gShimmerTexture;
void* mRedTexture = (void*)&gRedTexture;
void* mBlueTexture = (void*)&gBlueTexture;
void* mGreenTexture = (void*)&gGreenTexture;

//Objects textures.
OTexture gScoreTexture;
OTexture gPauseIcon;
OTexture gAppleTexture;
OTexture gBackgroundTexture;
OTexture gDiscoBallTexture;
OTexture gPickupTexture;
OTexture gPortalTexture;

//Text fonts.
TTF_Font *gFont = NULL;
TTexture gInitialMenuTextTexture;
TTexture gFinalTextTexture;
TTexture gInputTextTexture;

//Opponent score used for multiplayer.
int gOpponentScore = 0;

//Actual snake pieces number.
int gSnakePiecesNum;

//Global score.
int gScore = 0;

//Actual snake direction.
char gSnakeDirection;

//Speed decreaser, used for making the snake slower.
unsigned int speedDecrease;


bool start = true;
bool endEffect = true;
bool multiplayer = false;
bool endGame = false;
bool gThreadEnded;
bool portalActivated = false;
int gRecordScores[RECORD_NUMBER];

//This variable can be "Server" or "Client".
string gMode;

//This array contains the bests scores.
string gRecords[RECORD_NUMBER];

//Used for themes selecting.
string gSpriteTipe = "";

//The pickup to spawn.
string pickup;

//Define the particles array.
Particle* particles[TOTAL_PARTICLES * MAX_PIECES];

void Replacer(OTexture &texture, int replacer = 0)
{
	bool rightPosition = false;
	
	while(!rightPosition)
	{
		//Replace the apple.
		texture.resetPosition(SCREEN_WIDTH / BOX_SIZE, SCREEN_HEIGHT / BOX_SIZE, replacer);
		
		//Check if the apple is spawned on the haed.
		if(snakeHead.x != texture.getXPos() && snakeHead.y != texture.getYPos())
		{
			//Check if the apple is spawned on the tail.
			if(snakeTail.x != texture.getXPos() && snakeTail.y != texture.getYPos())
			{
				rightPosition = true;
				
				//Check if the apple is sapwned on a pice of the snake.
				for(int i = 0; i < gSnakePiecesNum; i++)
				{
					if(snakePieces[i].c.x == texture.getXPos() && snakePieces[i].c.y == texture.getYPos())
					{
						rightPosition = false;
					}
				}
			}
		}
		
		replacer ++;
	}
	
}


void resetGame()
{
	//Replace the snake PIECES.
	for(int i = 0; i < gSnakePiecesNum; i++)
	{
		snakePieces[i] = {0, 0, 'r', "normal"};
	}
	
	gSnakeTurnSprite.setType("turn");
	
	start = true;
	endGame = false;
	speedDecrease = 150;
	gColor = {255, 255 , 255};
	gOpponentScore = 0;
	gScore = 0;
	gSnakeDirection = 'r';
	gSnakePiecesNum = INITIAL_SNAKE_PIECES;
	snakeHead = {SCREEN_WIDTH/(BOX_SIZE * 2), SCREEN_HEIGHT/(BOX_SIZE * 2), 'r'};
	
	for(int i = 0; i < gSnakePiecesNum; i++)
	{
		snakePieces[i] = {snakeHead.x - i - 1, snakeHead.y, 'r', "normal"};
	}

	snakeTail = {snakeHead.x - gSnakePiecesNum - 1, snakeHead.y, 'r'};
	
	//Set the particles.
	for(int i = 0; i < TOTAL_PARTICLES * MAX_PIECES; i++)
	{
		particles[i] = NULL;
	}
	
	//Replace the apple end the pick up.
	Replacer(gAppleTexture);
	Replacer(gPickupTexture, 1);
	
	
	//The list to able and disable pickups
	activatedPickups[DISCOBALL] = true;
	activatedPickups[PORTAL] = true;
}


bool Init()
{
	bool success = true;
	
	//initDiscord();
	
	//Initialize the game's parameters.
	resetGame();
	
	for(int i = 0; i < RECORD_NUMBER; i++)
	{
		gRecordScores[i] = 0;
	}
	
	//Initialize sdl functions.
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{
		cout << SDL_GetError() << endl;
		success = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("Snake game by Claudio Mano Alpha 1.17", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(gWindow == NULL)
		{
			cout << SDL_GetError() << endl;
			success = false;
		}
		else
		{
			
			if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0)
					cout << Mix_GetError() << endl;
				//success = false;
			}
			
			
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer == NULL)
			{
				cout << SDL_GetError() << endl;
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				
				int imgFlag = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlag) & imgFlag))
				{
					cout << IMG_GetError() << endl;
					success = false;
				}
				
				if(TTF_Init() < 0)
				{
					success = false;
					cout << "Error initializing TTF\t" << TTF_GetError() << endl;
				}

			}
		}
	}
	
	return success;
}

//Set the window icon.
void setWindowIcon()
{
	gIconSurface = IMG_Load("images/favicon.ico");
	if(gIconSurface == NULL)
	{
		cout << IMG_GetError() << endl;
	}
	SDL_SetWindowIcon(gWindow, gIconSurface);
}


bool loadMedia()
{
	bool success = true;
	
	if(!gSnakeHeadSprite.loadFromFile("images/" + gSpriteTipe + "snakeHead.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gSnakeBodySprite.loadFromFile("images/" + gSpriteTipe + "snakeBody.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gSnakeTailSprite.loadFromFile("images/" + gSpriteTipe + "snakeTail.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gSnakeTurnSprite.loadFromFile("images/" + gSpriteTipe + "snakeTurn.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gRedTexture.loadFromFile("images/red.bmp"))
	{
		success = false;
	}
	else gRedTexture.setAlpha(192);
	
	if(!gGreenTexture.loadFromFile("images/green.bmp"))
	{
		success = false;
	}
	else gGreenTexture.setAlpha(192);
	
	if(!gBlueTexture.loadFromFile("images/blue.bmp"))
	{
		
		success = false;
	}
	else gBlueTexture.setAlpha(192);
	
	
	if(!gShimmerTexture.loadFromFile("images/shimmer.bmp"))
	{
		success = false;
	}
	else gShimmerTexture.setAlpha(192);
	
	if(!gAppleTexture.loadFromFile("images/" + gSpriteTipe + "apple.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gBackgroundTexture.loadFromFile("images/" + gSpriteTipe + "background.jpg"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	gFont = TTF_OpenFont("ttf/PressStart2P-Regular.ttf", TEXT_SIZE);
	if(gFont == NULL)
	{
		cout << "Error loading the TTF font " << TTF_GetError() << endl;
		success = false;
	}
	else
	{
		SDL_Color textColor = {0, 0, 0};
		if(!gInitialMenuTextTexture.loadFromRenderedText("Snake Game by Claudio Mano", textColor))
		{
			cout << TTF_GetError() << endl;
		}

	}
	
	if(!gDiscoBallTexture.loadFromFile("images/DiscoBall.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gPortalTexture.loadFromFile("images/portal.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	
	if(!gPauseIcon.loadFromFile("images/pause.png"))
	{
		cout << IMG_GetError << endl;
	}
	
	gSplatSound = Mix_LoadWAV("sounds/splat_sound.wav");
	if(gSplatSound == NULL)
	{
		cout << Mix_GetError() << endl;
	}
	
	gAppleBiteSound = Mix_LoadWAV("sounds/apple_bite_sound.wav");
	if(gAppleBiteSound == NULL)
	{
		cout << Mix_GetError() << endl;
	}
	
	gMusic = Mix_LoadMUS("sounds/sax_guy.wav");
	if(gMusic == NULL)
	{
		cout << Mix_GetError() << endl;
	}
	
	//Load the score's numbers texture.
	if(!gScoreTexture.loadFromFile("images/score.png"))
	{
		cout << IMG_GetError() << endl;
		success = false;
	}
	else
	{
		for(int i = 0; i < 10; i++)
		{
			gScoreClips[i].x = i * 72;
			gScoreClips[i].y = 0;
			gScoreClips[i].w = 72;
			gScoreClips[i].h = 77;
		}
	}
	
	setWindowIcon();
	return success;
}

//Close the program
void close()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	TTF_CloseFont(gFont);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_RemoveTimer(timer);
	SDL_FreeSurface(gIconSurface);
	Mix_FreeMusic(gMusic);
	
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void closeMultiplayer()
{
	endGame = true;
	net.freeSocket();
}

void renderPortalParticles()
{
	int x, y;
	
	for(int i = 0; i < TOTAL_PARTICLES; i++)
	{
		
		if(TOTAL_PARTICLES / 4 > i)
		{
			x = 0;
			y = i % (SCREEN_HEIGHT / BOX_SIZE);
		}
		else if(TOTAL_PARTICLES / 2 > i)
		{
			x = SCREEN_WIDTH / BOX_SIZE - 1;
			y = i % (SCREEN_HEIGHT / BOX_SIZE);
		}
		else if(TOTAL_PARTICLES * 3 / 4 > i)
		{
			x = i % (SCREEN_WIDTH / BOX_SIZE);
			y = 0;
		}
		else
		{
			x = i % (SCREEN_WIDTH / BOX_SIZE);
			y = SCREEN_HEIGHT / BOX_SIZE - 1;
		}
		
		if(particles[i] == NULL)
		{
			particles[i] = new Particle(x, y);
		}
		else if(particles[i] -> isDead())
		{
			delete particles[i];
			
			particles[i] = new Particle(x, y);
		}
		
		particles[i] -> render();
	}
	
}

void renderParticles()
{
	
	
	for(int i = 0; i < TOTAL_PARTICLES; i++)
	{
		if(particles[i] == NULL)
		{
			particles[i] = new Particle(snakeTail.x, snakeTail.y, snakeTail.direction);
		}
		else if(particles[i] -> isDead())
		{
			delete particles[i];
			
			particles[i] = new Particle(snakeTail.x, snakeTail.y, snakeTail.direction);
		}
		
		particles[i] -> render();
	}
	
	for(int i = 0; i < gSnakePiecesNum; i++)
	{
		for(int j = 0; j < TOTAL_PARTICLES * gSnakePiecesNum; j++)
		{
			if(particles[j] == NULL)
			{
				particles[j] = new Particle(snakePieces[i].c.x, snakePieces[i].c.y, snakePieces[i].c.direction);
			}
			else if(particles[j] -> isDead())
			{
				delete particles[j];
				
				particles[j] = new Particle(snakePieces[i].c.x, snakePieces[i].c.y, snakePieces[i].c.direction);
			}
			
			particles[j] -> render();
		}
	}
	
}


//Render snake PIECES.
void renderSnake()
{
	//Render the particles.
	//renderParticles();
	
	//Render the snake head.
	gSnakeHeadSprite.render(snakeHead.x, snakeHead.y, NULL, snakeHead.direction);
	
	
	//Render the snake body.
	for(int i = 0; i<gSnakePiecesNum; i++)
	{
		if(snakePieces[i].type == "normal")
			gSnakeBodySprite.render(snakePieces[i].c.x, snakePieces[i].c.y, NULL, snakePieces[i].c.direction);
		else if(snakePieces[i].type == "turn")
		{
			if(i == 0)
				gSnakeTurnSprite.setBeforeDirection(snakeHead.direction);
			else
				gSnakeTurnSprite.setBeforeDirection(snakePieces[i - 1].c.direction);
			gSnakeTurnSprite.render(snakePieces[i].c.x, snakePieces[i].c.y, NULL, snakePieces[i].c.direction);
		}
	}
	
	//Render the snake tail.
	gSnakeTailSprite.render(snakeTail.x, snakeTail.y, NULL, snakeTail.direction);
	
	
}

void moveSnake()
{
	//Moves snake tail.
	snakeTail = snakePieces[gSnakePiecesNum - 1].c;
	
	//The snake's PIECES are moved.
	for(int j = gSnakePiecesNum - 1; j > 0; j--)
	{
		snakePieces[j] = snakePieces[j - 1];
	}
	
	//Set the tail direction equal to the last pice's direction.
	snakeTail.direction = snakePieces[gSnakePiecesNum - 1].c.direction;
	
	//Updates the first pice's direction.
	snakePieces[0].c = snakeHead;
	snakePieces[0].c.direction = gSnakeDirection;
	
	
	//Moves snake head.
	switch(snakeHead.direction)
	{
		case 'u':
			snakeHead.y--;
			break;
			
		case 'd':
			snakeHead.y++;
			break;
				
		case 'l':
			snakeHead.x--;
			break;
			
		case 'r':
			snakeHead.x++;
			break;
	}
	
	//Set the global snake direction equal to the head direction.
	gSnakeDirection = snakeHead.direction;
	
	//Create a turn if the snake changes direction.
	if(snakePieces[0].c.direction != gSnakeDirection)
	{
		snakePieces[0].type = "turn";
	}
	else
	{
		snakePieces[0].type = "normal";
	}
	
}


void addPiceOfSnake()
{
	gSnakePiecesNum ++;
	
	snakePieces[gSnakePiecesNum - 1].c = snakeTail;
	
	switch (snakeTail.direction)
	{
		case 'u':
			snakeTail.y++;
			break;

		case 'd':
			snakeTail.y--;
			break;

		case 'l':
			snakeTail.x++;
			break;

		case 'r':
			snakeTail.x--;
			break;
	}
	
	
}


bool checkCollisions()
{
	bool collision = false;
	
	//Checks the collision with the bounds.
	if((snakeHead.x + 1) * BOX_SIZE > SCREEN_WIDTH)
	{
		collision = true;
		snakeHead.x = 0;
	}
	else if(snakeHead.x * BOX_SIZE < 0)
	{
		collision = true;
		snakeHead.x = SCREEN_WIDTH / BOX_SIZE - 1;
	}
	else if((snakeHead.y + 1) * BOX_SIZE > SCREEN_HEIGHT)
	{
		collision = true;
		snakeHead.y = 0;
	}
	else if(snakeHead.y * BOX_SIZE < 0)
	{
		collision = true;
		snakeHead.y = SCREEN_HEIGHT / BOX_SIZE - 1;
	}
	
	if(portalActivated)
	{
		collision = false;
	}
		
	
	//Check the collsion with any PIECES of the snake.
	for(int i = 0; i < gSnakePiecesNum; i++)
	{
		if(snakeHead.x == snakePieces[i].c.x && snakeHead.y == snakePieces[i].c.y)
		{
			collision = true;
		}
	}
	
	//Checks the collision with the tail.
	if(snakeHead.x == snakeTail.x && snakeHead.y == snakeTail.y)
	{
		collision = true;
	}
	
	return collision;
}


bool checkTaken(OTexture &texture)
{
	bool taken = snakeHead.x == texture.getXPos() && snakeHead.y == texture.getYPos();
	
	return taken;
}

//This function is called when the timer is ended.
Uint32 operations(Uint32 interval, void* param)
{
	*(bool*)param = true;
}


string genericMenu(string highWrite, string selections[], int selectionsNumber, SDL_Event &e)
{
	int index = 0;
	SDL_Color blue = {0, 0, 255}, red = {255, 0, 0};
	SDL_Rect buttons[selectionsNumber];
	bool quit = false;
	
	for(int i = 0; i < selectionsNumber; i++)
	{
		gInitialMenuTextTexture.loadFromRenderedText(selections[i]);
		buttons[i] = {(SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2, (i + 2) * 100, gInitialMenuTextTexture.getWidth(), gInitialMenuTextTexture.getHeight()};
	}
	
	while(!quit)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
				return "";
			}
			else if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_RETURN:
						quit = true;
						break;
					
					case SDLK_UP:
						if(index > 0)
							index--;
						else
							index = selectionsNumber - 1;
						break;
					
					case SDLK_DOWN:
						if(index < selectionsNumber - 1)
							index++;
						else
							index = 0;
						break;
				}
			}
			
			if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				bool inside;
				
				for(int i = 0; i < selectionsNumber; i++)
				{
					inside = true;
					if(x < buttons[i].x || x > buttons[i].x + buttons[i].w)
					{
						inside = false;
					}
					else if(y < buttons[i].y || y > buttons[i].y + buttons[i].h)
					{
						inside = false;
					}
					
					if(inside)
					{
						index = i;
						if(e.type == SDL_MOUSEBUTTONDOWN)
						{
							quit = true;
						}
					}
				}
				
			}
			
		}
		
		SDL_RenderClear(gRenderer);
		
		gInitialMenuTextTexture.loadFromRenderedText(highWrite);
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth())/ 2, 100);
		
		for(int i = 0; i < selectionsNumber; i++)
		{
			if(index == i)
			{
				gInitialMenuTextTexture.loadFromRenderedText(selections[i], red);
			}
			else
			{
				gInitialMenuTextTexture.loadFromRenderedText(selections[i], blue);
			}
			
			gInitialMenuTextTexture.render(buttons[i].x, buttons[i].y);
		}
		
		
		SDL_RenderPresent(gRenderer);
	}
	
	return selections[index];
}



bool initialMenu(SDL_Event &e)
{
	string themes[] = {"Normal", "Horror"};
	bool quit = false;
	bool exitGame = false;
	int i = 0;
	
	ifstream recordsFile("saves/records.bin", ios::binary);
	if(recordsFile.is_open())
	{
		while(!recordsFile.eof() && i < RECORD_NUMBER)
		{
			recordsFile >> gRecords[i];
			recordsFile >> gRecordScores[i];
			i++;
		}
		
		recordsFile.close();
	}
	else
	{
		cout << "Error opening the file\n" << endl;
	}
	
	
	while(!quit && e.type != SDL_QUIT)
	{
		string choices[] = {"Singleplayer", "Multiplayer", "Records", "Exit"};
		string choice;

		choice = genericMenu("Snake Game by Claudio Mano", choices, sizeof(choices) / sizeof(string), e);
		if(choice == choices[0])
		{
			gSpriteTipe = genericMenu("Choose the skin theme", themes, sizeof(themes) / sizeof(string), e) + "/";
			loadMedia();
			quit = true;
		}
		else if(choice == choices[1])
		{
			string choices[] = {"Server", "Client", "Back"};
			string choice;
			choice = genericMenu("Choose modality", choices, sizeof(choices) / sizeof(string), e);

			if(choice != "Back")
			{
				multiplayer = true;
				quit = true;
				gMode = choice;
			}
			
		}
		else if(choice == choices[2])
		{
			string temp[RECORD_NUMBER + 1];
			for(int i = 0; i < RECORD_NUMBER; i++)
			{
				temp[i] = gRecords[i] += " " + to_string(gRecordScores[i]);
			}
			
			temp[RECORD_NUMBER] = "back";
			
			while(genericMenu("Higests scores", temp, RECORD_NUMBER + 1, e) != "back" && e.type != SDL_QUIT);
		}
		else if(choice == choices[3])
		{
			exitGame = true;
		}
	}
	
	return exitGame;

}


void renderScore(int score, int yPos, SDL_Color color = {255, 255, 255})
{
	gScoreTexture.setTextureColorMod(color);
	gScoreTexture.setPosition(SCREEN_WIDTH, 0);
	int scoreDivider = 1, i = 1;
	
	do
	{
		gScoreTexture.setPosition(SCREEN_WIDTH - gScoreTexture.getWidth() * i, yPos);
		gScoreTexture.render(0, 0, &gScoreClips[(score % (scoreDivider * 10)) / scoreDivider]);
		scoreDivider *= 10;
		i++;
	}
	while(score % scoreDivider != score);
}


void getInputText(SDL_Event e, string &text, bool &renderText)
{
	
	renderText = false;
	
	if(e.type == SDL_KEYDOWN)
	{
		if(e.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0)
		{
			text.pop_back();
			renderText = true;
		}
		//Handle copy.
		else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
		{
			SDL_SetClipboardText(text.c_str());
		}
		//Handle past.
		else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
		{
			text = SDL_GetClipboardText();
			renderText = true;
		}
	}
	else if(e.type == SDL_TEXTINPUT)
	{
		text += e.text.text;
		renderText = true;
	}
	
}

string inputMenu(string highWrite, string undertitle, SDL_Event &e)
{
	SDL_StartTextInput();
	bool quit = false, renderText = false;
	string input = "";
	gInputTextTexture.loadFromRenderedText("_");
	TTF_Font *font =  TTF_OpenFont("ttf/PressStart2P-Regular.ttf", TEXT_SIZE - 12);
	SDL_StartTextInput();
	
	while(!quit)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
			}
			//Quit if the enter key is pressed.
			else if(e.type == SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_RETURN)
				{
					quit = true;
				}
			}
			
			//Check the inserting of the text.
			if(input.length() < 15 || e.key.keysym.sym == SDLK_BACKSPACE)
			{
				getInputText(e, input, renderText);
			}
			
		}
		
		SDL_RenderClear(gRenderer);
		
		//Render score text.
		gFinalTextTexture.loadFromRenderedText(undertitle);
		gFinalTextTexture.render((SCREEN_WIDTH - gFinalTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - 3 * gFinalTextTexture.getHeight()) / 2);
		
		//Render "Insert your name" string.
		gFinalTextTexture.loadFromRenderedText(highWrite, {0, 0, 0}, font);
		gFinalTextTexture.render((SCREEN_WIDTH - gFinalTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gFinalTextTexture.getHeight()) / 2);

		//Edit the text on input event.
		if(renderText)
		{
			gInputTextTexture.loadFromRenderedText(input + "_");
		}
		
		//Render name input text.
		gInputTextTexture.render((SCREEN_WIDTH - gInputTextTexture.getWidth()) / 2, (SCREEN_HEIGHT +  gInputTextTexture.getHeight()) / 2);
		
		SDL_RenderPresent(gRenderer);
	}

	SDL_StopTextInput();
	
	return input;
}

void newRecordMenu(SDL_Event &e, int newRecordPosition)
{
	string inputNameText = inputMenu("Wow, it's a new record! Insert your name","Your score is " + to_string(gScore), e);
	
	if(e.type != SDL_QUIT)
	{
		//Check if the name was inserted.
		if(inputNameText != "")
		{
			//Split the record list.
			for(int i = RECORD_NUMBER - 1; i > newRecordPosition; i--)
			{
				gRecords[i] = gRecords[i - 1];
				gRecordScores[i] = gRecordScores[i - 1];
			}
			
			gRecords[newRecordPosition] = inputNameText;
			gRecordScores[newRecordPosition] = gScore;
		
			ofstream file("saves/records.bin", ios::binary);
			if(file.is_open())
			{
				//Write record list in the file.
				for(int i = 0; i < RECORD_NUMBER; i++)
				{
					file << gRecords[i];
					file << "\t";
					file << gRecordScores[i] << endl;
					
				}
				
				file.close();
			}
			else
			{
				cout << "Error opening file for writing records\n" << endl;
			}
		}	
	}
	
	
}


int checkNewRecord()
{
	int newRecordPosition = -1;

	for(int i = 0; i < RECORD_NUMBER; i++)
	{
		if(gRecordScores[i] < gScore)
		{
			newRecordPosition = i;
			break;
		}
	}

	return newRecordPosition;
}


bool tryAgainMenu(SDL_Event e)
{
	bool tryAgain = false, quit = false;
	SDL_Color red = {255, 0, 0}, black ={0, 0, 0};
	SDL_Rect entries[2];
	
	gInitialMenuTextTexture.loadFromRenderedText("Yes", red);
	entries[0] = {(SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 4, 400, gInitialMenuTextTexture.getWidth(), gInitialMenuTextTexture.getHeight()};
	gInitialMenuTextTexture.loadFromRenderedText("No");
	entries[1] = {SCREEN_WIDTH - (SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 4, 400, gInitialMenuTextTexture.getWidth(), gInitialMenuTextTexture.getHeight()};
	
	
	
	while(!quit)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
				tryAgain = false;
			}
			else if(e.type == SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT)
				{
					tryAgain = !tryAgain;
				}
				else if(e.key.keysym.sym == SDLK_RETURN)
				{
					quit = true;
				}
			}
			else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				bool inside;
				
				for(int i = 0; i < 2; i++)
				{
					inside = true;
					if(x < entries[i].x || x > entries[i].x + entries[i].w)
					{
						inside = false;
					}
					else if(y < entries[i].y || y > entries[i].y + entries[i].h)
					{
						inside = false;
					}
					
					if(inside)
					{
						tryAgain = !i;
						if(e.type == SDL_MOUSEBUTTONDOWN)
						{
							quit = true;
						}
					}
				}
			}
		}
		
		SDL_RenderClear(gRenderer);
		
		gInitialMenuTextTexture.loadFromRenderedText("Your score is " + to_string(gScore));
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2, 200);
		
		gInitialMenuTextTexture.loadFromRenderedText("Do you want to try again?");
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2, 300);
		
		
		if(tryAgain)
		{
			gInitialMenuTextTexture.loadFromRenderedText("Yes", red);
		}
		else
		{
			gInitialMenuTextTexture.loadFromRenderedText("Yes");
		}
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 4, 400);
		
		
		if(!tryAgain)
		{
			gInitialMenuTextTexture.loadFromRenderedText("No", red);
		}
		else
		{
			gInitialMenuTextTexture.loadFromRenderedText("No");
		}
		gInitialMenuTextTexture.render(SCREEN_WIDTH - (SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 4, 400);
		
		SDL_RenderPresent(gRenderer);
	}
	
	return tryAgain;
}


bool checkOverlap(OTexture &texture1, OTexture &texture2)
{
	return texture1.getXPos() == texture2.getXPos() && texture1.getYPos() == texture2.getYPos();
}


void changeTexturesColor(SDL_Color color)
{
	gBackgroundTexture.setTextureColorMod(color);
	gSnakeHeadSprite.setTextureColorMod(color);
	gSnakeBodySprite.setTextureColorMod(color);
	gSnakeTurnSprite.setTextureColorMod(color);
	gSnakeTailSprite.setTextureColorMod(color);
	gAppleTexture.setTextureColorMod(color);
}

int changeColors(void* data)
{
	int times = 38;
	srand(time(NULL));
	Uint8 color;
	endEffect = false;
	
	//Increas the speed.
	speedDecrease += 60;
	
	Mix_PlayMusic(gMusic, -1);
	
	for(int i = 0; i < times; i++)
	{
		
		color = rand() % 256;
		
		gColor.r = color;
		
		color = rand() % 256;
		
		gColor.g = color;
		
		color = rand() % 256;
		
		gColor.b = color;
		
		changeTexturesColor(gColor);
		
		SDL_Delay(500);
		
		if(*(bool*)data)
			break;
	}
	
	if(speedDecrease >= 60)
		speedDecrease -= 60;
	
	Mix_HaltMusic();
	
	gColor = {255, 255, 255};
	changeTexturesColor(gColor);
	endEffect = true;
	return 0;
}

int portal(void* data)
{
	endEffect = false;
	int duration = 15, i = 0;
	
	while(!*(bool*)data && i < duration)
	{
		SDL_Delay(1000);
		i++;
	}
	
	portalActivated = false;
	endEffect = true;
	
	return 0;
	
}



int wThread(void* thread)
{
	int threadReturnValue;
	
	gThreadEnded = false;
	
	SDL_WaitThread((SDL_Thread*)thread, &threadReturnValue);
	
	gThreadEnded = true;
	
	return threadReturnValue;
}

int mThread(void* data)
{
	gMode = *(string*)data;
	char message;
	
	if(gMode == "Server")
	{
		while(net.serverRecive(&message, 1) > 0 && !endGame)
		{
			if((int)message < 0)
			{
				break;
			}
			else
			{
				gOpponentScore = (int)message;
			}
		}
		
		cout << "Ended data reciving from the client\n";
	}
	else if(gMode == "Client")
	{
		
		while(net.clientRecive(&message, 1) > 0 && !endGame)
		{
			if((int)message < 0)
			{
				break;
			}
			else
			{
				gOpponentScore = (int)message;
			}
		}
		
		cout << "Ended data reciving from the server\n";
	}
	
	return 0;
}



SDL_Thread* startMultiplayer(SDL_Event &e)
{
	
	char IP[17];
	
	SDL_Thread* multiplayerThread = NULL;

	
	if(gMode == "Server")
	{

		net.getMyIp(IP);
		
		SDL_RenderClear(gRenderer);
		
		gInitialMenuTextTexture.loadFromRenderedText("Waiting on IP " + (string)IP);
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gInitialMenuTextTexture.getHeight()) / 2);
		
		SDL_RenderPresent(gRenderer);
		
		if(net.initServer(25565))
		{
			multiplayerThread = SDL_CreateThread(mThread, "Multiplayer thread", (void*) &gMode);
		}
	}
	else if(gMode == "Client")
	{

		strcpy(IP, inputMenu("Insert server IP address", "", e).c_str());
		if(net.initClient(IP, 25565))
		{
			cout << "Reciving data from server\n";
			
			multiplayerThread = SDL_CreateThread(mThread, "Multiplayer thread", (void*) &gMode);
		}
		else
		{
			cout << "Failed server connection\n";
			multiplayer = false;
			gMode = "";
		}
	}
	
	return multiplayerThread;
}

void waitOpponent(SDL_Event &e)
{
	bool quit = false;
	
	while(!gThreadEnded && !quit)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		
		SDL_RenderClear(gRenderer);
		
		gInitialMenuTextTexture.loadFromRenderedText("Waiting opponent end");
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2, (SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2 + 20);
		
		gInitialMenuTextTexture.loadFromRenderedText(to_string(gScore) + " - " + to_string(gOpponentScore));
		gInitialMenuTextTexture.render((SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2, (SCREEN_WIDTH - gInitialMenuTextTexture.getWidth()) / 2);
		
		SDL_RenderPresent(gRenderer);
	}
}

int main(int argv, char* args[])
{
	//The event handler.
	SDL_Event e;
	
	//The new direction of the snake.
	char newDirection = 'r';
	
	//If true, the pickup spawns.
	bool spawnPickUp = false;
	
	bool gamePaused = true;
	
	//The game ends when true.
	bool quit = false;
	
	//The game starts again if true.
	bool restart = false;
	
	//Threads.
	SDL_Thread* discoBallThread;
	SDL_Thread* portalThread;
	SDL_Thread* multiplayerThread;
	
	srand(time(NULL));
	
	//Check the correctly initialization.
	if(Init())
	{
		//Check the correctly media loading.
		if(loadMedia())
		{
			
			//Show the initial menu.
			if(initialMenu(e) || e.type == SDL_QUIT)
				return 0;
			
			if(multiplayer)
			{
				multiplayerThread = startMultiplayer(e);
				if(e.type == SDL_QUIT)
				{
					return 0;
				}
			}
			
			//Running the game.
			while(!quit)
			{
				//Check events.
				while(SDL_PollEvent(&e) != 0)
				{
					//Check exit event.
					if(e.type == SDL_QUIT)
					{
						quit = true;
					}		
					//Check keyboard event to change direction.
					else if(e.type == SDL_KEYDOWN)
					{
						gamePaused = false;
						switch(e.key.keysym.sym)
						{
							case SDLK_UP:
							case 'w':
								if(snakeHead.direction != 'd')
									newDirection = 'u';
								break;
							
							case SDLK_DOWN:
							case 's':
								if(snakeHead.direction != 'u')
									newDirection = 'd';
								break;
								
							case SDLK_RIGHT:
							case 'd':
								if(snakeHead.direction != 'l')
									newDirection = 'r';
								break;
								
							case SDLK_LEFT:
							case 'a':
								if(snakeHead.direction != 'r')
									newDirection = 'l';
								break;
								
							case SDLK_p:
							case SDLK_ESCAPE:
								gamePaused = !gamePaused;
								break;
								
							default:
								gamePaused = true;
						}
					}
				}
				
				//Clear the screen.
				SDL_RenderClear(gRenderer);
				
				//Render the bakcground.
				gBackgroundTexture.render(SCREEN_WIDTH, SCREEN_HEIGHT, NULL);
				
				
				if(!spawnPickUp)
				//Render the apple.
				{
					gAppleTexture.render(BOX_SIZE, BOX_SIZE, NULL);
				}
				//Spawn pickups.
				else
				{
					gPickupTexture.render(BOX_SIZE, BOX_SIZE, NULL);
				}
				
				//Render the snake.
				renderSnake();
				
				//Render the effect if the portal pickup is activated.
				if(portalActivated)
				{
					renderPortalParticles();
				}
				
				//Render the score.
				renderScore(gScore, 0);
				
				if(multiplayer)
				{
					renderScore(gOpponentScore, 100, {255, 100, 100});
				}
				
				//Render the pause icon if the game is paused.
				if(gamePaused)
				{
					gPauseIcon.render(0, 0, NULL);
				}
				
				//Update the screen.
				SDL_RenderPresent(gRenderer);
				
				
				//Moves the snake's pieces coordinates.
				if(start)
				{
					
					//Reomove the timer.
					SDL_RemoveTimer(timer);
					
					//Add a timer to repete the operations inside the conditions.
					timer = SDL_AddTimer(SPEED - speedDecrease, operations, (void*)&start);
					
					snakeHead.direction = newDirection;
					if(checkTaken(gPickupTexture)  && spawnPickUp)
					{
						
						//Add a pice to the snake.
						addPiceOfSnake();
						if(pickup == "discoball")
						{
							discoBallThread = SDL_CreateThread(changeColors, "Color changing", (void*)&quit);
						}
						else if(pickup == "portal")
						{
							portalActivated = true;
							portalThread = SDL_CreateThread(portal, "portal", (void*)&quit);
						}
						
						
						spawnPickUp = false;
						
						//Increase the score.
						gScore ++;
						
						//Change the speed of the snake.
						if(speedDecrease > 0)
							speedDecrease -= 5;
							
						Replacer(gAppleTexture);
						
					}
					//Check the apple taken.
					else if(checkTaken(gAppleTexture))
					{
						
						//Play the sound effect.
						Mix_PlayChannel(-1, gAppleBiteSound, 0);
						
						//Replace the apple.
						int counter = 0;
						
						Replacer(gAppleTexture, counter);
						
						//Add a pice to the snake.
						addPiceOfSnake();
						
						//Increase the score.
						gScore ++;
						
						//Send the score.
						if(multiplayer)
						{
							if(gMode == "Server")
							{
								net.serverSend((char*) &gScore, 1);
							}
							else if(gMode == "Client")
							{
								net.clientSend((char*) &gScore, 1);
							}
							
						}	
						
						//Change the speed of the snake.
						if(speedDecrease > 0)
							speedDecrease -= 5;
						
						
						if(!spawnPickUp && endEffect)
						{
							
							switch(rand() % PICKUP_SPAWN_PROB)
							{
								case 0:
									if(activatedPickups[DISCOBALL])
									{
										gPickupTexture = gDiscoBallTexture;
										pickup = "discoball";
										spawnPickUp = true;
									}
									
									break;
								
								case 1:
									if(activatedPickups[PORTAL])
									{
										gPickupTexture = gPortalTexture;
										pickup = "portal";
										spawnPickUp = true;
									}
									
									break;
							}
							Replacer(gPickupTexture, counter);
							
							
						}
						
						
					}

					
					start = false;
					
					//You win when you reach the max score.
					if(gSnakePiecesNum == MAX_PIECES)
					{
						quit = true;
						cout << "You won!!\n";
					}
					
					//Update the snake coordinates if the game is not paused.
					if(!gamePaused)
						moveSnake();
				}
					
				//Check the collisions with other PIECES or the bounds.
				if(checkCollisions())
				{
					//If present, the current effect will be stopped.
					endEffect = true;
					
					//Play the splat effect.
					Mix_PlayChannel(-1, gSplatSound, 0);
			
					//Quit from the loop.
					quit = true;
					
					//Remove the timer.
					SDL_RemoveTimer(timer);
					
					//No more input commands.
					start = false;
					
				}
				
				if(quit && e.type != SDL_QUIT)
				{
					int newRecordPosition = checkNewRecord();
					
					//Check if the score is a new record.
					if(!(newRecordPosition < 0))
					{
						//Menu where the player insert his name.
						newRecordMenu(e, newRecordPosition);
						if(e.type == SDL_QUIT)
						{
							return 0;
						}
					}
					
					if(multiplayer)
					{
						int threadReturnValue;
						char endMessage = -1;
						SDL_Thread* waitThread;
						string result, choises[] = {"Play again", "Exit multiplayer"}, choice;
						
						cout << "Waiting opponent end\n";
						
						
						if(gMode == "Server")
						{
							net.serverSend(&endMessage, 1);
						}
						else if(gMode == "Client")
						{
							net.clientSend(&endMessage, 1);
						}
						
						//Wait the opponent end.
						waitThread = SDL_CreateThread(wThread, "Wait thread", (void*)multiplayerThread);
						waitOpponent(e);
						
						if(gScore > gOpponentScore)
						{
							result = "You won!!";
						}
						else if(gScore < gOpponentScore)
						{
							result =  "You lose";
						}
						else if(gScore == gOpponentScore)
						{
							result = "Draw";
						}
						
						choice = genericMenu(result + " " + to_string(gScore) + " - " + to_string(gOpponentScore), choises, 2, e);
						
						
						if(choice == "Play again")
						{
							quit = false;
							resetGame();
							gamePaused = true;
							newDirection = 'r';
							multiplayerThread = SDL_CreateThread(mThread, "Multiplayer thread", (void*) &gMode);
						}
						else
						{
							closeMultiplayer();
							restart = tryAgainMenu(e);
						}
						
					}
					else
					{
						restart = tryAgainMenu(e);
					}
					
				}
			
			}
			
		}
	}
	
	
	//Terminate sdl functins.
	close();
	
	
	//If restart is true, the game starts again.
	if(restart)
	{
		main(0, {});
	}
	
	return 0;
}

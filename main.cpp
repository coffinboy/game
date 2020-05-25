

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <SDL_mixer.h>
#include <ctime>
#include <iostream>
#include <SDL_ttf.h>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 400;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);


	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);


	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//The dot that will move around on the screen
class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 96;
	static const int DOT_HEIGHT = 100;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	//Initializes the variables
	Dot();

	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e,bool GameOver);

	//Moves the dot
	void move();

	//Shows the dot on the screen
	void render();
	int mPosX, mPosY;
	int mVelX, mVelY;
	void collision(int x , int y);
	int health;
	

private:
	
};

static const int ENEMY_WIDTH = 115;
static const int ENEMY_HEIGHT = 110;
//The enemy that will move around on the screen
class ENEMY
{
public:
	//The dimensions of the enemy
	

	//Maximum axis velocity of the enemy
	static const int ENEMY_VEL = 10;

	//Initializes the variables
	ENEMY();

	//Moves the enemy
	void move();

	//Shows the enemy on the screen
	void render();

	//The X and Y offsets of the enemy
	int mPosX_E, mPosY_E;

	//The velocity of the enemy
	int mVelX_E, mVelY_E;
private:
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font* gFont = NULL;

//textures
LTexture gDotTexture;
LTexture gBGTexture;
LTexture gENEMYTexture;
LTexture gFullHP;
LTexture g23HP;
LTexture g13HP;
LTexture g0HP;
LTexture gText;
LTexture gGameOver;
//The music that will be played
Mix_Music* gMusic = NULL;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromRenderedText(string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

void runtime(int time)
{
	SDL_Color white = { 255, 255, 255 };
	gText.loadFromRenderedText((string("TIME:") + to_string(time)).c_str(), white);
	gText.render(400, 10);

}

Dot::Dot()
{
	//Initialize the offsets
	mPosX = 0;
	mPosY = 0;
	health = 96;
	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

ENEMY::ENEMY()
{
	//Initialize the offsets
	mPosX_E = 0;
	mPosY_E = 0;

	//Initialize the velocity
	mVelX_E = 0;
	mVelY_E = 0;
}

void Dot::handleEvent(SDL_Event& e,bool GameOver)
{
	if (GameOver) 
	{
		mVelX = 0;
		mVelY = 0;
	}
	else
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: mPosY -= DOT_HEIGHT; break;
			case SDLK_DOWN: mPosY += DOT_HEIGHT; break;
			case SDLK_LEFT: mVelX -= DOT_VEL; break;
			case SDLK_RIGHT: mVelX += DOT_VEL; break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_LEFT: mVelX += DOT_VEL; break;
			case SDLK_RIGHT: mVelX -= DOT_VEL; break;
			}
		}
	}
}

void Dot::move()
{
	//Move the dot left or right
	mPosX += mVelX;

	//If the dot went too far to the left or right
	if ((mPosX < -DOT_WIDTH) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
	{
		//Move back
		mPosX -= mVelX;
	}

	//Move the dot up or down
	mPosY += mVelY;
	
	//If the dot went too far up or down
	if ((mPosY < -DOT_HEIGHT) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		mPosY -= mVelY;
	}
	if (mPosX < 0)
	{
		mPosX = 0;
	}
	if (mPosX > SCREEN_WIDTH)
	{
		mPosX = SCREEN_WIDTH;
	}
	if (mPosY < 0)
	{
		mPosY = 0;
	}
	if (mPosY > SCREEN_HEIGHT-100)
	{
		mPosY = SCREEN_HEIGHT-100;
	}
}

void ENEMY::move()
{
	//Move the dot left or right
	mPosX_E += mVelX_E;

	//Create random speed for enemies after they reach the screen border
	if (mPosX_E < -150)
	{
		mPosX_E = SCREEN_WIDTH;
		mVelX_E = -(rand() % 10 + 1);
	}

}
int dem = 0;
void Dot::collision(int x, int y)
{
	
	if (mPosY >= y && mPosY  <= y + 80)
	{
		if (mPosX + DOT_WIDTH >= x && mPosX + DOT_WIDTH <= x + 115)
		{
			dem++;
			if (dem > 25)
			{
				dem = 0;
				health = health - 32;
			}
		}
	}
	if (mPosY+50 >= y && mPosY+50 <= y + 105)
	{
		if (mPosX + DOT_WIDTH >= x && mPosX + DOT_WIDTH <= x + 115)
		{

			dem++;
			if (dem > 25)
			{
				dem = 0;
				health = health - 32;
			}
			
		}
		else if (mPosX >= x && mPosX <= x + 115)
		{

			dem++;
			if (dem > 25)
			{
				dem = 0;
				health = health - 32;
			}
		}
	}	
}

void Dot::render()
{
	//Show the dot
	gDotTexture.render(mPosX, mPosY);
	if (health == 96)
	{
		gFullHP.render(mPosX, mPosY + 80);
	}
	if (health >= 64 && health < 96)
	{
		g23HP.render(mPosX, mPosY + 80);
	}
	if (health >= 32 && health < 64)
	{
		g13HP.render(mPosX, mPosY + 80);
	}
	if (health >= 0 && health < 32)
	{
		g0HP.render(mPosX, mPosY + 80);
	}
}

void ENEMY::render()
{
	//Show the enemy
	gENEMYTexture.render(mPosX_E, mPosY_E);
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Superman Dodge", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
				//Initialize SDL_ttf;
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if (!gDotTexture.loadFromFile("player.png"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	//Load background texture
	if (!gBGTexture.loadFromFile("bkground.png"))
	{
		printf("Failed to load background texture!\n");
		success = false;
	}
	//Load enenies texture
	if (!gENEMYTexture.loadFromFile("meteorite1.png"))
	{
		printf("Failed to load enemy texture!\n");
		success = false;
	}
	//Load healthbar texture
	if (!gFullHP.loadFromFile("FullHP.png"))
	{
		printf("Failed to load enemy texture!\n");
		success = false;
	}
	if (!g23HP.loadFromFile("23HP.png"))
	{
		printf("Failed to load enemy texture!\n");
		success = false;
	}
	if (!g13HP.loadFromFile("13HP.png"))
	{
		printf("Failed to load enemy texture!\n");
		success = false;
	}
	if (!g0HP.loadFromFile("0HP.png"))
	{
		printf("Failed to load enemy texture!\n");
		success = false;
	}
	//Load music
	gMusic = Mix_LoadMUS("Smusic.mp3");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	//load font
	gFont = TTF_OpenFont("font.ttf", 30);
	if (gFont == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	//load gameover texture
	if (!gGameOver.loadFromFile("gameover.png"))
	{
		printf("Failed to load gameover texture!\n");
		success = false;
	}
	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();
	gBGTexture.free();
	gENEMYTexture.free();
	//Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	srand(time(NULL));	
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot dot;

			ENEMY enemy1;
			ENEMY enemy2;
			ENEMY enemy3;
			ENEMY enemy4;
			enemy1.mPosX_E  = 840;
			enemy1.mPosY_E  = 0;
			enemy2.mPosX_E = 840; 
			enemy2.mPosY_E = 100;
			enemy3.mPosX_E = 840;
			enemy3.mPosY_E = 200;
			enemy4.mPosX_E = 840;
			enemy4.mPosY_E = 300;
			enemy1.mVelX_E = -(rand() % 10 + 1);
			enemy2.mVelX_E = -(rand() % 10 + 1);
			enemy3.mVelX_E = -(rand() % 10 + 1);
			enemy4.mVelX_E = -(rand() % 10 + 1);
			bool GameOver = false;
			//The background scrolling offset
			int scrollingOffset = 0;

			//start the backgroundSound
			Mix_PlayMusic(gMusic, -1);
			Uint32 Timerun;
			//While application is running
			while (!quit)
			{

				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent(e,GameOver);
				}
				
				
				//Move the dot
				dot.move();
				//Move the enemy
				enemy1.move();
				enemy2.move();
				enemy3.move();
				enemy4.move();
				dot.collision(enemy1.mPosX_E, enemy1.mPosY_E);
				dot.collision(enemy2.mPosX_E, enemy2.mPosY_E);
				dot.collision(enemy3.mPosX_E, enemy3.mPosY_E);
				dot.collision(enemy4.mPosX_E, enemy4.mPosY_E);
				
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render background
				gBGTexture.render(scrollingOffset, 0);
				gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);

				//Render objects
				dot.render();
				enemy1.render();
				enemy2.render();
				enemy3.render();
				enemy4.render();
				if (!GameOver) 
				{
					
					Uint32 Timevalue = SDL_GetTicks();
					Timerun = Timevalue / 1000;
					runtime(Timerun);
				}
				if (dot.health == 0)
				{
					GameOver = true;
				}
				if (GameOver) 
				{ 
					gGameOver.render((SCREEN_WIDTH - gGameOver.getWidth()) / 2, 30); 
					dot.mPosX = 0;
					dot.mPosY = 0;
					enemy1.mVelX_E = 0;
					enemy2.mVelX_E = 0;
					enemy3.mVelX_E = 0;
					enemy4.mVelX_E = 0;
					gText.loadFromRenderedText((string("lifetime : ") + to_string(Timerun)).c_str(), { 255, 0, 0 });
					gText.render((SCREEN_WIDTH - gText.getWidth()) / 2, 250);
				}
				
				
				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
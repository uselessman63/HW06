#include "iostream"
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL_timer.h"
#include <vector>
#include <cmath>
#include "SDL2_rotozoom.h"
#include "SDL_Keyboard.h"
#include "SDL_keycode.h"
#include <time.h>
#include <SDL_image.h>

using namespace std;

const int FRAMERATE = 60;
const double FRAME_TIME = 1000/FRAMERATE;
const float PI = 3.14159265;
const int TIME_IN_SECONDS = 60;
int window_height = 640;
int window_width = 800;
int speed = 5;
const int player_width = window_width/16;
const int player_height = window_height/16;
int maxEnemies = 100;

static bool boost;
SDL_Event keypress;
bool KEY[332];
static uint8_t PlayerFrame;
static uint32_t	PlayerFrameTime;
static bool	PlayerBlinking;
static uint32_t PlayerBlinkTime;
#define BLINK_TIME     200
#define SPEED_BOOST -90.0f
#define GRAVITY 5.0f




struct pipe {
	float x, y, vx, vy, speeds, height;
	bool passed;
};

float randomizer()
{
	return rand()/(float)RAND_MAX;
};
vector<pipe> upper (maxEnemies);
vector<pipe> lower (maxEnemies);
struct hero{
	float x, y, vy, roto, startx, starty, centerx, centery;
};
hero player;
void generateHero()
{
	player.x = player_width/2;
	player.y = player_height/2;
	player.vy = 5;
	player.roto = 1.5;
	player.startx = player_width - player.x;
	player.starty = window_height/2 - player.y;
	player.centerx = player.startx + (player_width/2);
	player.centery = player.starty + (player_height/2);
	
}
void generateUpperPipe()
{
	int distance = 0;
	int speedster = speed+0.1;
	int rando = rand();
	for (int i = 0; i < maxEnemies; i++)
	{
		
			
			upper[i].x = distance + window_width;
			upper[i].y = 0;
			upper[i].vx = upper[i].x + player_width/2;	
			
			//upper[i].speeds = rand()%speedster;
			upper[i].height = 40 + randomizer()* (window_height - player_height*4);
			upper[i].vy = upper[i].y + (upper[i].height/2);
			distance+= window_width/3;
		
	}
}
void generateLowerPipe()
{
	int distance = 0;
	for (int i = 0; i < maxEnemies; i++)
	{
		//cout << fRand() << " ";
		
			lower[i].x = distance + window_width;
			lower[i].y = upper[i].height + player_height*5;
			lower[i].vx = lower[i].x + (player_width/2);
			
			//lower[i].speeds = rand()%speed+0.1;
			lower[i].height = window_height - lower[i].y;
			lower[i].vy = lower[i].y + lower[i].height/2;
			distance+= window_width/3;
		
	}
}


void drawUpper(SDL_Renderer *ren, SDL_Texture *tex)
{
	for (int i = 0; i < maxEnemies; i++)
	{
		
		SDL_Rect bulletSpace = {upper[i].x, 0, player_width, upper[i].height }; //positioning of thingy
		SDL_RenderCopy(ren, tex, NULL, &bulletSpace); //texturization of thingy
		//SDL_FillRect(surf, &bulletSpace, 0x00FF00);
		
	}
	
}
void drawLower(SDL_Renderer *ren, SDL_Texture *tex)
{
	for (int i = 0; i < maxEnemies; i++)
	{

		SDL_Rect bulletSpace = {lower[i].x, lower[i].y, player_width, lower[i].height }; //positioning of thingy
		SDL_RenderCopy(ren, tex, NULL, &bulletSpace); //texturization of thingy
		//SDL_FillRect(surf, &bulletSpace,0x00FF00);
	}
	
}
void drawPlayer(SDL_Renderer *ren, SDL_Texture *tex, int speed)
{
	
	SDL_Point center = {player.x, player.y};
	SDL_Rect players = {player.startx,player.starty, player_width, player_height};
	SDL_RenderCopyEx(ren, tex, NULL, &players, player.roto*speed, &center, SDL_FLIP_NONE);
	
	
}
void moveHero(int speed)
{
	
	 player.starty +=speed;
	 player.centery += speed;
}
void action()
{
	player.starty -= 12;
}
void advanceBullets(int playerposx, int playerposy, int t)
{
	 
	for (int i = 0; i < maxEnemies; i++)
	{
		
		upper[i].x -= t;
		lower[i].x -= t;
		upper[i].vx -= t;
		lower[i].vx -= t;
		/*int angle = atan(1.0*xDistance/yDistance) * 180 / PI;

		enemies[i].vx =  speed*(cos(angle*PI/180));
		enemies[i].vy =  speed*(sin(angle*PI/180));
		enemies[i].x += enemies[i].vx;
		enemies[i].y += enemies[i].vy;*/
	}
	
}
double magnitude(float vecx, float vecy)
{
	return sqrt(vecx*vecx+vecy*vecy);
}
float normalizeX(float vecx, float vecy)
{
	double mag = sqrt((vecx*vecx)+(vecy*vecy));
	float resultx, resulty;
	resultx = vecx/mag;
	resulty = vecy/mag;
	return resultx;


}
void removePassedPipes(int index)
{
	upper[index].x = upper[index+1].x;
	upper[index].y = upper[index+1].y;
	upper[index].height = upper[index+1].height;
	upper[index].vx = upper[index+1].vx;
	upper[index].vy = upper[index+1].vy;
	upper[index].passed = upper[index+1].passed;

	lower[index].x = lower[index+1].x;
	lower[index].y = lower[index+1].y;
	lower[index].height = lower[index+1].height;
	lower[index].vx = lower[index+1].vx;
	lower[index].vy = lower[index+1].vy;
	lower[index].passed = lower[index+1].passed;
	
}
float normalizeY(float vecx, float vecy)
{
	double mag = sqrt((vecx*vecx)+(vecy*vecy));
	float resultx, resulty;
	resultx = vecx/mag;
	resulty = vecy/mag;
	return resulty;


}
double dotproduct( float vec1x, float vec1y, float vec2x, float vec2y)
{
	return ((vec1x*vec2x)+(vec2y*vec1y));
}
double maximize(float hhx, float hhy, float hwx, float hwy, float projaxisx, float projaxisy)
{
	double a = dotproduct(hhx, hhy, projaxisx, projaxisy) + dotproduct(hwx, hwy, projaxisx, projaxisy);
	double b = -dotproduct(hhx, hhy, projaxisx, projaxisy) - dotproduct(hwx, hwy, projaxisx, projaxisy);
	if (b >= a)
	{
		a = b;
	}

	b = -dotproduct(hhx, hhy, projaxisx, projaxisy) + dotproduct(hwx, hwy, projaxisx, projaxisy);
	if (b >=a)
	{
		a = b;
	}

	b = +dotproduct(hhx, hhy, projaxisx, projaxisy) - dotproduct(hwx, hwy, projaxisx, projaxisy);
	if (b>=a)
	{
		a = b;
	}
	return a;
}
bool CollisionCheckSAT(int speed, vector<pipe> piper)
{
	for(int i = 0; i<maxEnemies; i++)
	{
		if (piper[i].passed)
		{
			int haha = 1+1;
		}
		else
		{
			float theta1 = (player.roto*speed)/(float)180*PI;
			float theta2 = 0/(float)180*PI;
			int hw1x, hw1y, hw2x, hw2y, hh1x, hh1y, hh2x, hh2y;
			double cs  = cos(theta1);
			double sn = sin(theta1);
			hw1x = (player_width/2)*cs;
			hw1y = (player_width/2)*sn;
			hh1x = (player_height/2)*sn;
			hh1y = (player_height/2)*cs;

			cs = cos(theta2);
			sn = sin(theta2);
			hw2x = (player_width/2)*cs;
			hw2y = (player_width/2)*sn;
			hh2x = (piper[i].height/2)*sn;
			hh2y = (piper[i].height/2)*cs;

			int centervecx, centervecy;
			centervecx = piper[i].vx - player.centerx;
			centervecy = piper[i].vy - player.centery;
			

			//distance
			double distsqrd = (centervecx*centervecx)+(centervecy*centervecy);
			float normalizedx = normalizeX(centervecx, centervecy);
			float normalizedy = normalizeY(centervecx, centervecy);
			double wew1 = maximize(hh1x, hh1y, hw1x, hw1y, normalizedx, normalizedy);
			double wew2 = maximize(hh2x, hh2y, hw2x, hw2y, normalizedx, normalizedy);
		
			if (distsqrd>= (wew1+wew2)*(wew1+wew2))
			{
				return false;
			}

			//hw1
			normalizedx = normalizeX(hw1x, hw1y);
			normalizedy = normalizeY(hw1x, hw1y);
			double distance = dotproduct(centervecx, centervecy, normalizedx, normalizedy);
			wew1 = maximize(hh1x, hh1y, hw1x, hw1y, normalizedx, normalizedy);
			wew2 = maximize(hh2x, hh2y, hw2x, hw2y, normalizedx, normalizedy);
			if ( distance*distance >= (wew1+wew2)*(wew1+wew2))
			{
				return false;
			}
		
			//hh1
			normalizedx = normalizeX(hh1x, hh1y);
			normalizedy = normalizeY(hh1x, hh1y);
			distance = dotproduct(centervecx, centervecy, normalizedx, normalizedy);
			wew1 = maximize(hh1x, hh1y, hw1x, hw1y, normalizedx, normalizedy);
			wew2 = maximize(hh2x, hh2y, hw2x, hw2y, normalizedx, normalizedy);
			if ( distance*distance >= (wew1+wew2)*(wew1+wew2))
			{
				return false;
			}
			//hw2
			normalizedx = normalizeX(hw2x, hw2y);
			normalizedy = normalizeY(hw2x, hw2y);
			distance = dotproduct(centervecx, centervecy, normalizedx, normalizedy);
			wew1 = maximize(hh1x, hh1y, hw1x, hw1y, normalizedx, normalizedy);
			wew2 = maximize(hh2x, hh2y, hw2x, hw2y, normalizedx, normalizedy);
			if ( distance*distance >= (wew1+wew2)*(wew1+wew2))
			{
				return false;
			}

			//hh2
			normalizedx = normalizeX(hh2x, hh2y);
			normalizedy = normalizeY(hh2x, hh2y);
			distance = dotproduct(centervecx, centervecy, normalizedx, normalizedy);
			wew1 = maximize(hh1x, hh1y, hw1x, hw1y, normalizedx, normalizedy);
			wew2 = maximize(hh2x, hh2y, hw2x, hw2y, normalizedx, normalizedy);
			if ( distance*distance >= (wew1+wew2)*(wew1+wew2))
			{
				return false;
			}
			return true;
		}

	}
	
}

bool CollisionCheck()
{
	for (int i = 0; i<maxEnemies; i++)
	{
		
		if(((((((player.centery - player_height/2 <= upper[i].vy + upper[i].height/2)&& (player.centery - player_height/2 >= upper[i].vy - upper[i].height/2))
			|| ((player.centery + player_height/2 >= upper[i].vy + upper[i].height/2)&& (player.centery + player_height/2 <= upper[i].vy - upper[i].height/2)))
			&& ((player.centerx + player_width/2 >= upper[i].vx - player_width/2)&&(player.centerx + player_width/2 <= upper[i].vx + player_width/2))
			|| ((player.centerx - player_width/2 <= upper[i].vx - player_width/2)&&(player.centerx - player_width/2 >= upper[i].vx + player_width/2)))
			|| ((((player.centery + player_height/2 >= lower[i].vy - lower[i].height/2)&& (player.centery - player_height/2 <= lower[i].vy - lower[i].height/2))
			|| ((player.centery - player_height/2 <= lower[i].vy - lower[i].height/2)&& (player.centery + player_height/2 >= lower[i].vy - lower[i].height/2)))
			&& ((player.centerx - player_width/2 >= lower[i].vx + player_width/2)&&(player.centerx + player_width/2 <= lower[i].vx + player_width/2))
			|| ((player.centerx + player_width/2 <= lower[i].vx + player_width/2)&&(player.centerx - player_width/2 >= lower[i].vx + player_width/2))))	))
		{
			return true;
			break;
		}
		/*if ((player.centery<= upper[i].vy) && (((player.centerx + player_width/2 >= upper[i].vx - player_width/2)&&(player.centerx + player_width/2 <= upper[i].vx + player_width/2))
			|| ((player.centerx - player_width/2 <= upper[i].vx - player_width/2)&&(player.centerx - player_width/2 >= upper[i].vx + player_width/2))))
		{
			return true;
			break;
		}*/
	}
}


//http://wiki.libsdl.org/APIByCategory

int main(int argc, char* argv[]) {
	int mx, my;
	srand(time(NULL));
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Run this", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 640, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	
	SDL_Surface *flapper = SDL_LoadBMP("raiden.bmp");
	SDL_Texture *flappertex = SDL_CreateTextureFromSurface(ren, flapper);
	SDL_FreeSurface(flapper);

	SDL_Surface *pipeman = SDL_LoadBMP("raiden.bmp");
	SDL_Texture *pipemantex = SDL_CreateTextureFromSurface(ren, pipeman);
	SDL_FreeSurface(pipeman);
	
	generateHero();
	generateUpperPipe();
	generateLowerPipe();
	bool running = true;
	double t =0 ;
	double dt = 0.1;
	int frameNo = 0;
	int speed = 1;
	int count = 0;
	double speedtimer = 0;
	bool isSpeed = false;
	
	uint32_t time;
	int pipeSpeed = 2;
	bool alive = true;
	while (running) {

		int frameTimeStart = SDL_GetTicks();
		
		if (flapper == NULL)
		{
			return 1;
		}
		
		int frame_time_start = SDL_GetTicks();
		
		
		SDL_Event ev;
		SDL_KeyboardEvent key;
		const auto start = SDL_GetTicks();
		SDL_GetMouseState(&mx, &my);
		
		
		
		SDL_SetRenderDrawColor(ren,0,0,0,0);
		
			
		
			
		//generateEnemy(mx, my);
			while(SDL_PollEvent(&ev) != 0) {
			if(ev.type == SDL_QUIT) running = false;
			else if (ev.type == SDL_KEYDOWN && alive)
			{
				if (ev.key.keysym.sym == SDLK_SPACE)
				{
					
					
					speed = SPEED_BOOST;
					
					
					
				}
				else if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
				
			}
				
			}
			
			
			
			
			SDL_RenderClear(ren);
			double wew = 10;
			
				if ((alive && (CollisionCheckSAT(speed, upper) == true) || (CollisionCheckSAT (speed, lower) == true) || (player.centery +player_height/2>= window_height) || (player.centery - player_height/2 <= 0)))
				{
					
					
				}
				else if(!alive)
				{
					
					speed+= GRAVITY;
					moveHero(speed/10);

				}

				else
				{
					speed += GRAVITY;
					advanceBullets(mx,my, pipeSpeed);
					moveHero(speed/10);
					drawPlayer(ren, flappertex, speed/10);
				
				
				}
			
			drawUpper(ren, pipemantex);
			drawLower(ren, pipemantex);
			
			
			
			
		
	
		
		for(int i = 0; i< maxEnemies; i++)
		{
			if (alive && player.startx > upper[i].vx && upper[i].passed!=true)
			{
				count++;
				upper[i].passed = true;
				lower[i].passed = true;
				cout << "Score: " << count << '\n';
				
			}
		}
        
			
		SDL_RenderPresent(ren);
			if ((FRAME_TIME - (SDL_GetTicks() - frameTimeStart)) > 0)
			{
				SDL_Delay(FRAME_TIME - (SDL_GetTicks() - frameTimeStart));
			}
		
		
		//SDL_Delay(2000);
	}
	     
	SDL_DestroyTexture(pipemantex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

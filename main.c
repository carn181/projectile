#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

struct projectile {
  float angle;
  float u;
  float g;
};

const uint16_t SCREEN_WIDTH = 640;
const uint16_t SCREEN_HEIGHT = 480;

struct font{
  TTF_Font *ttf;
  SDL_Color color;
  SDL_Surface *surface;
  SDL_Texture *texture;
} ;

struct program {
  SDL_Window *win;
  SDL_Renderer *renderer;
  SDL_Event gevent;
  SDL_Color bgcolor;
  struct font font;
  struct projectile projectile;  
  bool done;
};

void font_init(struct font *font, SDL_Renderer *renderer) {
  font->ttf = TTF_OpenFont("font.ttf", 25);
}

void text_update(SDL_Renderer *renderer, struct font *font, const char *text, int x, int y){
  if(font->surface != NULL){
    SDL_DestroyTexture(font->texture);
    SDL_FreeSurface(font->surface);
  }
  font->surface = TTF_RenderText_Solid(font->ttf, text, font->color);
  font->texture = SDL_CreateTextureFromSurface(renderer, font->surface);
  
  int texW = 0, texH = 0;
  SDL_QueryTexture(font->texture, NULL, NULL, &texW, &texH);
  SDL_Rect dsrect = {x, y, texW, texH};
  SDL_RenderCopy(renderer, font->texture, NULL, &dsrect);
}

void font_destroy(struct font *font) {
  SDL_DestroyTexture(font->texture);
  SDL_FreeSurface(font->surface);
  TTF_CloseFont(font->ttf);
}


void clearscene(struct program *p) {
  SDL_SetRenderDrawColor(p->renderer, p->bgcolor.r, p->bgcolor.g, p->bgcolor.b, 255);
  SDL_RenderClear(p->renderer);
}


float degree2rad(float theta) {
  return theta * (M_PI / 180);
}

float projectilefunc(float x, float theta, float u, float g) {
  double t = tan(theta);
  double c = cos(theta);
  return ((x*t) - (g * pow(x, 2) /
		   (2 * pow(u, 2) * pow(c, 2))));
}

void drawprojectile(struct program *p) {
  SDL_SetRenderDrawColor(p->renderer, 250, 189, 47, 255);
  for(float i = 0 ; i < 640 ; i += 0.5){
    SDL_RenderDrawPointF(p->renderer, i,
			 480-projectilefunc(i, degree2rad(p->projectile.angle),
					p->projectile.u, p->projectile.g));
  }
}

void scene_display(struct program *p) {
  clearscene(p);
  if(p->projectile.angle < 90)
    drawprojectile(p);

  SDL_Rect bg = (SDL_Rect){290, 0, 350, 75};
  SDL_SetRenderDrawColor(p->renderer, 50, 48, 47, 50);
  SDL_RenderFillRect(p->renderer, &bg);
  
  char *angle = (char *)calloc(10, sizeof(char));
  char *u = (char *)calloc(22, sizeof(char));
  sprintf(angle, "Angle: %.0f", p->projectile.angle);
  sprintf(u, "Initial Velocity: %.0f", p->projectile.u);
  
  text_update(p->renderer, &p->font, angle, 300, 0);
  text_update(p->renderer, &p->font, u, 300, 25);
  if(p->projectile.angle == 90)
    text_update(p->renderer, &p->font, "Max Angle", 300, 50);
  else if(p->projectile.angle == 0)
    text_update(p->renderer, &p->font, "Min Angle", 300, 50);
  
  SDL_RenderPresent(p->renderer);

  free(u);
  free(angle);
}

void event_handle(struct program *p) {
  SDL_Event *event = &p->gevent;
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_QUIT:
      p->done = true;
      break;
      
    case SDL_KEYDOWN: {
      switch (event->key.keysym.sym) {
      case SDLK_ESCAPE:
	p->done = true;
	break;
	
      case SDLK_UP:
	if(p->projectile.angle < 90)
	  p->projectile.angle++;
	scene_display(p);
	break;
		
      case SDLK_DOWN:
	if(p->projectile.angle > 0)
	  p->projectile.angle--;
	scene_display(p);
	break;

      case SDLK_LEFT:
	p->projectile.u--;
	scene_display(p);
	break;
	
      case SDLK_RIGHT:
	p->projectile.u++;
	scene_display(p);
	break;

      }
    }
    }
  }
}

int init_program(struct program *p) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize: %s\n", SDL_GetError());
    return 0;
  }
  else {
    TTF_Init();
    p->win = SDL_CreateWindow("Projectile", SDL_WINDOWPOS_UNDEFINED,
			      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
			      SCREEN_HEIGHT, 0);
    p->renderer = SDL_CreateRenderer(p->win, -1, SDL_RENDERER_ACCELERATED);
    
    font_init(&p->font, p->renderer);
    return 1;
  }
}

void end_program(struct program *p) {
  if(p->win){
    printf("Ending\n");

    font_destroy(&p->font);
    SDL_DestroyRenderer(p->renderer);
    SDL_DestroyWindow(p->win);
    TTF_Quit();
    SDL_Quit();
  }
}

int main(int argc, char *args[]) {
  struct program p;
  p.bgcolor = (SDL_Color){29, 32, 33};
  p.projectile = (struct projectile) {0, 50, 10};
  p.font.color = (SDL_Color) {235, 219, 178};
  
  if (!init_program(&p))
    return 0;

  p.done = false;
  
  scene_display(&p);
  while (!p.done) {
    event_handle(&p);
  }
  end_program(&p);
  return 0;
}

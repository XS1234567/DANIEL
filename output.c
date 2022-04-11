#include "SDL/SDL.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


#include "SystemaCV.h"
//#include "sdl_output.h"
 //s£tatic SDL_Surface *buffer_surface;

void render(Window*w)
{
	w->screen = SDL_GetVideoSurface();
		
	if(SDL_BlitSurface(w->buffer_surface, NULL, w->screen, NULL) == 0)
		SDL_UpdateRect(w->screen, 0, 0, 0, 0);
	SDL_Delay(1);
}

static int event_filter(const SDL_Event * event)
{ 
	return event->type == SDL_QUIT; 
}

int start_showing(Window*w,Image *i, const char *name)
{
	atexit(SDL_Quit);
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	
	SDL_SetVideoMode((*i).width, (*i).heigth, 24, SDL_SWSURFACE);
	SDL_WM_SetCaption(name, name);
	
	 w->buffer_surface= SDL_CreateRGBSurfaceFrom( (i->data).start, 
			(*i).width, (*i).heigth, 24, (*i).width*3,
			0xFF0000, 0x00FF00, 0x0000FF, 0);
	SDL_SetEventFilter(event_filter);
	
	return 0;
}
void init_point(Point*a,int x,int y)
{
	a->x=x;
	a->y=y;
}
void init_color(Color*a,unsigned char r,unsigned char g, unsigned char b)
{
	a->r=r;
	a->b=b;
	a->g=g;

}
void init_image(Image *i,int w,int h,int c)
{
	i->data.length=w*h*3;
	i->data.start=calloc(w*h*3,sizeof(uint8_t));
	i->heigth=h;
	i->width=w;
	i->channels=c;
	
}
void init_buffer(buffer *b, long int b_len)
{
	b->length = b_len;
	b->start = calloc(b_len,sizeof(uint8_t));
}
void kill_buffer(buffer *b)
{
	free(b->start);
}


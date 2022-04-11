

#include<stdio.h>
#include "SDL/SDL.h"
typedef struct STATE
{
int lane_keeping;
int Intersection;
int Intersection_Entry_Point;
int Park;
int Retake;
int Overtake;
int Pedestrian;

char str[30];
}STATE;


typedef struct FS
{
    int StangaLim;
    int DreaptaLim;
    int segments;
    int *x_seg;
    int *y_seg;
    int mijloc;
    STATE *st;
    int angle;
}FS;

typedef struct buffer {
	void *start;
	long int length;
} buffer;
typedef struct Image{
	int width;
	int heigth;
	int channels;
    buffer data;

}Image;
typedef struct Point
{
double x,y;
}Point;
typedef struct Vector
{
	int *data; //toate numerele din vector
	int n; //cate nr is in vector
}Vector;
typedef struct Color
{
	unsigned char r,g,b;
}Color;  
typedef struct VideoIO
{
    int width,heigth,channel;
    unsigned char *data;
    FILE *pipeIn;
    FILE *pipeOut;
}VideoIO;
typedef struct Window // TODO sa fac multiple windows
{
SDL_Surface *buffer_surface;
SDL_Surface * screen;
SDL_Event * event;

}Window;



void render(Window *w);
int start_showing(Window*w,Image*i, const char*);




void init_color(Color*,unsigned char r,unsigned char g,unsigned char b);
void init_point(Point*,int,int);
void init_image(Image*,int,int,int);//w*h*c
void init_buffer(buffer*,long int);
void kill_buffer(buffer*);










void rgb_to_gray3D(Image *rgb,Image *gray);
void gray_to_sobel(Image* in ,Image* out);




void render();












void init_fs(FS*fs,int segments,STATE*st);
void Limits(FS*fs,Image*im,Image*out); //Outdated
int Get_Array(FS*fs,Image*mask,Image*out);
int Get_ArrayH(FS*fs,Image*mask,Image*out);


void putText( Image *, const char *,int x,int y,Color color,Color Background);
void rectangle(struct Image *pic,struct Point a,struct Point b,struct Color*c,int fill);
void Servo(int angle,int prevangle);
void initServo();
void moveDC(int pin,int angle, int prevangle, int hold, int align);

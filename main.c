#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
//camera/mmal/raspberry specific libraries
#include "bcm_host.h"
#include <interface/mmal/mmal.h>
#include <interface/mmal/util/mmal_default_components.h>
#include <interface/mmal/util/mmal_util.h>
#include <interface/mmal/util/mmal_connection.h>
#include <interface/mmal/util/mmal_util_params.h>
#include <interface/vcos/vcos.h>
#include "SystemaCV.h"
#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

typedef struct Bridge
{
int angle,prevangle,accel;

}Bridge;

//will affect framerate, it seems that if framerate is higher than possible shutter speed, it will be automatically lowered
#define CAMERA_SHUTTER_SPEED 15000

//framerate above 30 only possible for some resolution, depends on the camera
//can also reduce the displayed portion of the camera on screen
#define CAMERA_FRAMERATE 60

//resolution needs to be smaller than the screen size
#define CAMERA_RESOLUTION_X 640
#define CAMERA_RESOLUTION_Y 480

#define CHECK_STATUS(status, msg) if (status != MMAL_SUCCESS) { fprintf(stderr, msg"\n\r");}


uint32_t screen_size_x = 0;
uint32_t screen_size_y = 0;

static int cur_sec;

sem_t semaphore;


void output_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);

void init_time_keeping();
float get_cur_time();

void *CAMERA(void*br){
    struct Bridge *b = (struct Bridge*)br;

    //sets up the framebuffer, will draw
    //framebuffer_init();

    MMAL_STATUS_T status = MMAL_EINVAL;
    MMAL_COMPONENT_T *camera;
    MMAL_PORT_T *video_port;
    MMAL_ES_FORMAT_T *format;
    MMAL_POOL_T *pool;

    sem_init(&semaphore, 0, 0);

    bcm_host_init();
    
    status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);
    CHECK_STATUS(status, "failed to create decoder");

    status = mmal_port_parameter_set_uint32(camera->control, MMAL_PARAMETER_SHUTTER_SPEED, CAMERA_SHUTTER_SPEED);
    CHECK_STATUS(status, "failed to set shutter speed");

    video_port = camera->output[MMAL_CAMERA_VIDEO_PORT];

    format = video_port->format;
    format->encoding = MMAL_ENCODING_RGB24;
    format->es->video.width = VCOS_ALIGN_UP(CAMERA_RESOLUTION_X, 32);
    format->es->video.height = VCOS_ALIGN_UP(CAMERA_RESOLUTION_Y, 16);
    format->es->video.crop.x = 0;
    format->es->video.crop.y = 0;
    format->es->video.crop.width = CAMERA_RESOLUTION_X;
    format->es->video.crop.height = CAMERA_RESOLUTION_Y;

    printf("Camera: resolution %dx%d\n\r", CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y);

    status = mmal_port_format_commit(video_port);
    CHECK_STATUS(status, "failed to commit format");

    //second paramter of the second parameter is the denominator for the framerate
    MMAL_PARAMETER_FRAME_RATE_T framerate_param = {{MMAL_PARAMETER_VIDEO_FRAME_RATE, sizeof(framerate_param)}, {CAMERA_FRAMERATE, 0}};
    status = mmal_port_parameter_set(video_port, &framerate_param.hdr);
    CHECK_STATUS(status, "failed to set framerate");

    //two buffers seem a good compromise, more will cause some latency
    video_port->buffer_num = 1;
    pool = mmal_port_pool_create(video_port, video_port->buffer_num, video_port->buffer_size);

    video_port->userdata = (void *)pool->queue;

    status = mmal_component_enable(camera);
    CHECK_STATUS(status, "failed to enable camera");

    //will call the callback function everytime there is an image available
    status = mmal_port_enable(video_port, output_callback);
    CHECK_STATUS(status, "failed to enable video port");

    usleep(250);

    //necessary parameter to get the RGB data out of the video port
    status = mmal_port_parameter_set_boolean(video_port, MMAL_PARAMETER_CAPTURE, 1);
    CHECK_STATUS(status, "failed to set parameter capture");

    //need to provide the buffers to the port
    int queue_length = mmal_queue_length(pool->queue);
    for(int i = 0; i < queue_length; i++){
        MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(pool->queue);
        if(buffer == NULL){
            printf("problem to get the buffer\n\r");
        }

        status = mmal_port_send_buffer(video_port, buffer);
        CHECK_STATUS(status, "could not send buffer");
    }

    MMAL_BUFFER_HEADER_T *buffer;
    float time_since_report = 0.0f;
    int count_frames = 0;

    float start_time;
    float end_time;
    float start_copy_time;
    float end_copy_time;
    STATE st;
    init_time_keeping();
    FS fs;
   init_fs(&fs,30,&st);
    Window w;
    Image rgb,mask;
    init_image(&rgb,640,480,3);
    init_image(&mask,640,480,3);
    int ok = start_showing(&w,&rgb,"Visual"); 
    //(rgb.data).start=buffer;
    int angle=0,prevangle=0;
    
    while(1){
        start_time = get_cur_time();
        
        //wait until a buffer has been received
        sem_wait(&semaphore);

        buffer = mmal_queue_get(pool->queue);
	
        start_copy_time = get_cur_time();
        
        //draw the image on the top left corner of the framebuffer
        //would be less costly to limit frambuffer size and just do a memcpy
        int offset_data = 0;
        uint8_t* px,*mx;
	//printf("\n%d\n",sizeof(buffer->data));
	px=rgb.data.start;
        mx=mask.data.start;
        int t=200;
	int R_min=50,R_max=255,b_min=60;
        for(int i=0;i<rgb.data.length;i+=3)
	{
              //int a=(int*)((buffer->data[i]+buffer->data[i+1]+buffer->data[i+2])/3);
              
              *(px)=(uint8_t*)buffer->data[i];
              *(px+1)=(uint8_t*)buffer->data[i+1];
              *(px+2)=(uint8_t*)buffer->data[i+2];
               if(buffer->data[i]>R_min&&  buffer->data[i+1]<b_min  &&buffer->data[i+2]<b_min 	) { *(mx)=255;*(mx+1)=255,*(mx+2)=255;}
               else{*(mx)=0;*(mx+1)=0,*(mx+2)=0;}
               mx=mx+3;
	       px=px+3;
	   	//*(px)=(uint8_t*)((buffer->data[i*3]+buffer->data[i*3+1]+buffer->data[i*3+2])/3);
	      
              
	      
             
	     
              
           
        }
	//gateway to civilsation
        //rgb_to_gray3D(&rgb,&rgb);
       // gray_to_sobel(&rgb,&mask);
        
        //Get_ArrayH(&fs,&mask,&rgb);
        
        
	Limits(&fs,&mask,&rgb);
	angle=Get_Array(&fs,&mask,&rgb);
	(*b).angle=fs.mijloc;
	//Servo(fs.angle,prevangle);
	//prevangle=fs.angle;
	render(&w);
        end_copy_time = get_cur_time();
        // printf("frame copy time: %f\n\r", end_copy_time-start_copy_time);

        //Send back the buffer to the port to be filled with an image again
        mmal_port_send_buffer(video_port, buffer);
     
        end_time = get_cur_time();
        float seconds = (float)(end_time - start_time);
        time_since_report += seconds;
        count_frames++;

        if(time_since_report > 1.0f){
            float framerate = count_frames/time_since_report;
            printf("frequency: %fHz\n\r", framerate);
            time_since_report = 0;
            count_frames = 0;
        }
    }
    kill_buffer(&rgb.data);
    kill_buffer(&mask.data);
    //todo free the mmal and framebuffer ressources cleanly
}



void output_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer){
   struct MMAL_QUEUE_T *queue = (struct MMAL_QUEUE_T *)port->userdata;

   mmal_queue_put(queue, buffer);

   sem_post(&semaphore);

}

//clock_gettime is a better time keeping mechanism than other on the raspberry pi
void init_time_keeping(){
    struct timespec time_read;
    clock_gettime(CLOCK_REALTIME, &time_read);
    cur_sec = time_read.tv_sec; //global
}

float get_cur_time(){
    struct timespec time_read;
    clock_gettime(CLOCK_REALTIME, &time_read);
    return (time_read.tv_sec-cur_sec)+time_read.tv_sec/1000000000.0f;
}






void *SERVO(void*br)
{
struct Bridge *b = (struct Bridge*)br;
//p//rintf("\nThread %ld, %ld", (*b).angle, (*b).prevangle);





while(1)
{

//void *pt=p;
int angle= (*b).angle;
float ratio=320/90;
angle=(int)(angle/ratio);
if (angle>120)
{
angle=120;
}
else if (angle<60)
{
angle=60;
}
//int prevangle=(*b).prevangle;
int pa=0;
int pb=60;
move(17,angle,(*b).prevangle,0,0,0);
//move(17,pb,pa,0,0,0);
printf("\nAngle %d %d",angle,(*b).prevangle);
(*b).prevangle=angle;
}
}
void *DC(void*p)

{
moveDC(18,35,0,0,0);
int i=0;
while(1)
{
i++;
int *angle=(int*)p;
if(angle=120)
{
moveDC(18,35,0,0,0);
}
else{
moveDC(18,35,0,0,0);
}


}}

















void main()
{
initServo();
Bridge br;
br.angle=1;
br.accel=1;
br.prevangle=1;
pthread_attr_t attr;
  pthread_t thread_id;
  pthread_t thread_id2;
  pthread_t thread_id3;

  pthread_attr_init(&attr);

  pthread_create(&thread_id, &attr, &SERVO, &br);
  pthread_create(&thread_id2, &attr, &DC, &br.angle);
  pthread_create(&thread_id3, &attr, &CAMERA, &br);
  pthread_join(thread_id,NULL);
  pthread_join(thread_id2,NULL);
  pthread_join(thread_id3,NULL);
printf("\ndaskda");
}

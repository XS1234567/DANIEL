#include "SystemaCV.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
double to_degrees(double radians) {
    return radians * (180.0 / 3.14);
}

void ID(FS*fs,Image*i)
{
Color C,B;
init_color(&C,255,255,255);
init_color(&B,255,0,0);
int j;
int contorR=0;
for(j=0;j<fs->segments;j++)
{
    if(*(fs->x_seg+j)>300 && *(fs->x_seg+j)!=400)
    {
        contorR++;
    }
    
}
if(contorR>fs->segments-10 )
{
    putText(i,"INTERSECTION STATE",50,10,C,B);

}
else putText(i,"LANE KEEPING STATE",50,10,C,B);
}
void IID(FS*fs,Image*i)
{
    Color C,B;
    init_color(&C,255,255,255);
    init_color(&B,255,0,0);

    if(fs->DreaptaLim==539 && fs->StangaLim==101  )
    {
        putText(i,"INTERSECTION ENTRY POINTS STATE",50,10,C,B);
    }
   
}
double length(Point A,Point B)//sqrt((P1x - P2x)2 + (P1y - P2y)2)
{
    return sqrt(pow((A.x-B.x),2)+pow((A.y-B.y),2));
}
double getAngle(Point A,Point B,Point C)
{

    double a = B.x - A.x;
    double b = B.y - A.y;
    double c = B.x - C.x;
    double d = B.y - C.y;

    double atanA = atan2(a, b);
    double atanB = atan2(c, d);

    return atanB - atanA;

}
//arccos((P12^2 + P13^2 - P23^2) / (2 * P12 * P13))
void init_fs(FS*fs,int segments,STATE*st)
{
    fs->segments=segments;
    fs->x_seg=malloc(segments*sizeof(int));
    fs->y_seg=malloc(segments*sizeof(int));
    fs->mijloc=320;
    fs->st=st;
    
    
}
int Get_Array(FS*fs,Image*mask,Image*out)
{
    
    int ti,tensor=0,i,j;
   int angle=0;
    double sumx=0,sumy=0;
    
    Point A,B,C,D;
    init_point(&D,450,320);
    init_point(&A,640,450);
    init_point(&B,320,450);
    
    
    int w;
    w=fs->DreaptaLim-fs->StangaLim;
    w=(w/fs->segments);
    
   
   for(i=0;i<fs->segments;i++)
   {
       *(fs->y_seg+i)=(int)(fs->StangaLim+i*w);
       
   }
   
   
   
   
   
    //for i in range(self.height-5, 0, -1):
   //             if self.pic[i][y_form[j]]:
   //                 x_form[j] = i
   //       
   //          break
   unsigned char *px;
   unsigned char *rx;
    //int i;
    for (i=0;i<fs->segments;i++)
    {
        for(j=400;j>1;j--)
        {
            px = (mask->data.start+j*mask->width*3+fs->y_seg[i]*3);
			rx = (out->data.start+j*out->width*3+fs->y_seg[i]*3);
            
			if (px[0] )
            {
            rx[1]=255;
           
             break;

            }
            else{
                *(fs->x_seg+i)=j;
                rx[1]=255;
            }
            
        }
    }
    
    for(i=0;i<fs->segments;i++)
    {
        sumx=sumx+*(fs->y_seg+i);
        sumy=sumy+*(fs->x_seg+i);
        
    }
    
    sumx=sumx/(fs->segments+1);
    sumy=sumy/(fs->segments+1);
    sumx=(int)sumx;
    sumy=(int)sumy;
    int sx=(int)sumx;
    int sy=(int)sumy;
    Color c,b;
    char str[20],str1[20],str2[20];
    init_color(&c,255,255,255);
    init_color(&b,0,0,0);
    sprintf(str, "X : %.1f", sumx);
    sprintf(str1, "Y : %.1f", sumy);
    putText(out,str,50,50,c,b);
    putText(out,str1,50,70,c,b);
    
    ID(fs,out);
    IID(fs,out);
    
    
    init_point(&C,sumx,sumy);
    angle=getAngle(A,B,C);
    Point A1,B1;
    init_point(&A1,295,sx-5);
    init_point(&B1,305,sx+5);
    rectangle(out,B1,A1,&c,0);
    //float angba = atan((a.y - b.y) / (a.x - b.x));

    
   // rectangle(i,C,)
    //angle=to_degrees(angle);
    float ang=angle;
   // if (ang>25)
   // { ang=25;}
    
   /// else if (ang<-25) {ang=-25;}
   float an=angle;
    sprintf(str2, "Angle : %.1f", (an));
    putText(out,str2,50,200,c,b);
    

    //stateManager(&fs->st,&out);
    return (int)angle;
    //TODO fix angle
    

}
void Limits(FS*fs,Image*im,Image*out)

{
    fs->DreaptaLim=(int)(im->width/2);
    fs->StangaLim=(int)(im->width/2);
    fs->mijloc=(int)(im->width/2);
    int i,j;
    int StangaLim=320;
    int DreaptaLim=320;
    uint8_t *px,*pxRGB;
	for(i=fs->mijloc;i>10;i--)
	{
	
	
                                 //row         //col
			px = (im->data.start+350*im->width*3+i*3);
            pxRGB=(out->data.start+350*out->width*3+i*3);

            if(px[0])
            {
             pxRGB[1]=255;
             break;
            }
            else{
            StangaLim=i;
            pxRGB[1]=255;
            }		
	}
    pxRGB=(out->data.start+350*out->width*3+StangaLim*3);
    pxRGB[1]=255;
    pxRGB[2]=255;
    fs->StangaLim=StangaLim;
    for(i=fs->mijloc;i<im->width-10;i++)
	{
	
	
                                 //row         //col
			px = (im->data.start+350*im->width*3+i*3);
            pxRGB=(out->data.start+350*out->width*3+i*3);

            if(px[0])
            {
                pxRGB[1]=255;
                break;
            
            }
            else{
            DreaptaLim=i;
            pxRGB[1]=255;
            }	
	}
    
    pxRGB=(out->data.start+350*out->width*3+DreaptaLim*3);
    pxRGB[1]=255;
    pxRGB[2]=255;
    
    fs->DreaptaLim=DreaptaLim;
    fs->mijloc=(int)(fs->DreaptaLim/2+fs->StangaLim/2);
    
    
}




int Get_ArrayH(FS*fs,Image*mask,Image*out)
{
    
    int ti,tensor=0,oldtensor=0,i,j;
   int angle;
    double sumx=0,sumy=0;
    
    Point A,B,C;
    init_point(&A,640,400);
    init_point(&B,320,400);
    int mijloc=320,val=0;
    fs->StangaLim=100;
    fs->DreaptaLim=500;
    fs->mijloc=320;
    //circle(&out,B,10);
    unsigned char *px,*pxRGB;
   unsigned char *rx;
    for(j=400;j>50;j--)
        {
            px = (mask->data.start+j*mask->width*3+mijloc*3);
			rx = (out->data.start+j*out->width*3+mijloc*3);
            if (px[0] )
            {
            //rx[2]=255;
             break;

            }
            else{
                val=j;
               
                //rx[1]=255;
            }
        }
    //val=100;
    int opc;
    
    opc=fs->segments;
    int mijloc1[opc];
    int ratie=(int)((400-val)/opc);
    uint8_t* px_mid;
    if (ratie>1)
    {
    int Stangalim[opc];
    int Dreaptalim[opc];
    
    
    for(j=opc;j>0;j--)
    {
        for(i=fs->mijloc;i>10;i--)
	{
	
	
                                 //row         //col
			px = (mask->data.start+(-j*ratie)*mask->width*3+i*3);
            pxRGB=(out->data.start+(400-j*ratie)*out->width*3+i*3);

            if(px[0])
            {
             pxRGB[1]=255;
             break;
            }
            else{
           // StangaLim=i;
            pxRGB[1]=255;
            Stangalim[opc-j]=i;
            }		
	}
    
       for(i=fs->mijloc;i<out->width-10;i++)
	{
	
	
                                 //row         //col
			px = (mask->data.start+(400-j*ratie)*mask->width*3+i*3);
            pxRGB=(out->data.start+(400-j*ratie)*out->width*3+i*3);
             
            if(px[0])
            {
             pxRGB[1]=255;
             break;
            }
            else{
            Dreaptalim[j]=i;
            pxRGB[1]=255;

            }   		
	}
    //400-j*ratie
    //(mijloc1[opc-j])
    
   
    
    }
    uint8_t*px1,*px2;
    

     
 
    
    
  
    float x_a=0;
    float y_a=0;

    for(i=0;i<opc;i++)
    {
        x_a=x_a+Stangalim[i];
        y_a=y_a+Dreaptalim[i];

    }

    x_a=x_a/opc;
    y_a=y_a/opc;
    Color C,B;
     init_color(&C,255,255,255);
    init_color(&B,255,0,0);
    fs->DreaptaLim=(int)y_a;
    fs->StangaLim=(int)x_a;
    fs->mijloc=(int)(x_a/2+y_a/2);
    float accel=val/100;
    char str2[100];
    sprintf(str2, "Accel : %.1f", (3-accel));
    putText(out,str2,50,220,C,B);
    int a[opc];
    
    if (accel<=3)
    {
         for(j=opc;j>0;j--)
         {
           
         mijloc1[opc-j]=(Stangalim[opc-j]+Dreaptalim[j])/2;
         mijloc1[opc-j]=(int)mijloc1[opc-j];
         Color col;
         Point P1,P2;
         init_color(&col,255,255,255);
         int tr=3;
         /*
          if(Dreaptalim[j]==539&&Stangalim[opc-j]!=101 )
          {
                init_color(&col,255,255,0);
              
                init_point(&P1,(400-j*ratie)-tr,(mijloc1[opc-j])-tr);
    
                init_point(&P2,(400-j*ratie)+tr,(mijloc1[opc-j])+tr);
          }
          else if (Stangalim[opc-j]==101  )
          {
                init_color(&col,0,255,0);
         int a=(int)((mijloc1[opc-j])/2+50.5);
         int b=(int)(539/2-(mijloc1[opc-j])/2);
         init_point(&P1,(400-j*ratie)-tr,a-tr);
    
         init_point(&P2,(400-j*ratie)+tr,a+tr);
          }
          else
          {*/
          init_color(&col,255,0,0);
              
              init_point(&P1,(400-j*ratie)-tr,(mijloc1[opc-j])-tr);
    
                 init_point(&P2,(400-j*ratie)+tr,(mijloc1[opc-j])+tr);
        
         
         rectangle(out,P1,P2,&col,1);
         }
    }
    
    
    
    fs->angle=mijloc1[15]-230;
    if(fs->angle>180)
    {
    fs->angle=180;
    }
    else if (fs->angle<0)
    {
    fs->angle=0;
    }
    
    /*if( mijloc1[15]<310)
    {fs->angle=60;}
    else if(mijloc1[15]>330){fs->angle=120;}
    else {fs->angle=90;}*/
    
    
   // printf("\n%d\n",angle1); 
    //angle=to_degrees(angle);
    
    //TODO fix angle
    
    //printf("\n  %d %d  \n",fs->StangaLim,fs->DreaptaLim);
    }


}

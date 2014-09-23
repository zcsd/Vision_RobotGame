#ifndef __VISION_H
#define __VISION_H

#include "cv.h"
#include "highgui.h"

typedef struct
{
	int Hmin ; 
	int Hmax;
	int Smin; 
	int Smax;
	int Lmin; 
	int Lmax;
	
} VisionRange;

typedef struct
{
	int Xmin; 
	int Xmax;
	int Ymin; 
	int Ymax;
	
} BlobCoord;

int VISION_InitCam();
void VISION_DestoryCam();

int VISION_GrabFrame(); 
int VISION_ShowOriginalFrame();
void VISION_EnableTuneMode();
void VISION_DisableTuneMode();

int VISION_PrcessFrame(VisionRange range,VisionRange range1,VisionRange range2, BlobCoord *blob, BlobCoord *blob1,char *gameplay,char *color);

int VISION_PrcessFrame1(VisionRange range,VisionRange range1,VisionRange range2, BlobCoord *blob, BlobCoord *blob1,char *gameplay,char *color);
#endif
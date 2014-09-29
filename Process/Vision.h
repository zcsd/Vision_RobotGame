#ifndef __VISION_H
#define __VISION_H

#include "cv.h"
#include "highgui.h"

typedef struct
{
	int invert;	
	int Hmin;
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

typedef struct
{
	int set;
}Color;

int VISION_InitCam();
void VISION_DestoryCam();

int VISION_GrabFrame();
int VISION_ShowOriginalFrame();
void VISION_EnableTuneMode();
void VISION_DisableTuneMode();
void LoadColor(Color color);
void LoadRangeSD(VisionRange RangeSD, VisionRange RangeSD1, VisionRange RangeSD2);

int VISION_Tune_Color1(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color);
int VISION_Tune_Color2(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color);
int VISION_Tune_Color3(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color);
int VISION_Game_1Color(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color);
int VISION_Game_ArrowDetect(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color);
int VISION_Game_OBS(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,BlobCoord *blob1,char *gameplay, Color color);

#endif

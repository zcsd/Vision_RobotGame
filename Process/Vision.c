#include <stdio.h>
#include <time.h>
#include "Vision.h"

#define NUMBER_OF_BLOB 3//the number of bounding box
#define BLOB_SIZE 50
#define N 1

unsigned char min[3]= {0};
unsigned char max[3]= {0};
unsigned char min1[3]= {0};
unsigned char max1[3]= {0};
unsigned char min2[3]= {0};
unsigned char max2[3]= {0};
unsigned int Invert[3]={0};

unsigned int ArX_min=0,ArX_max=0,ArY_min=0,ArY_max=0;

int colorNo;
CvMemStorage *storage=0;
IplImage  *frame;
CvCapture *capture;
CvFont font;

char TuneMode = 0;
char str[50]; 
char str1[50]; 
char str2[50]; 
char str3[50]; 

void LoadValueColor(VisionRange range, VisionRange range1, VisionRange range2)
{
	Invert[0] =range.invert;
	min[0]=range.Hmin;
	min[1]=range.Smin;
	min[2]=range.Lmin;
	max[0]=range.Hmax;
	max[1]=range.Smax;
	max[2]=range.Lmax;
	
	Invert[1] =range1.invert;
	min1[0]=range1.Hmin;
	min1[1]=range1.Smin;
	min1[2]=range1.Lmin;
	max1[0]=range1.Hmax;
	max1[1]=range1.Smax;
	max1[2]=range1.Lmax;
	
	Invert[2] =range2.invert;
	min2[0]=range2.Hmin;
	min2[1]=range2.Smin;
	min2[2]=range2.Lmin;
	max2[0]=range2.Hmax;
	max2[1]=range2.Smax;
	max2[2]=range2.Lmax;
}

void LoadColor(Color color)
{
	colorNo=color.set;
}

int VISION_InitCam()
{
	storage = cvCreateMemStorage( 0 );//setup memory buffer, needed by the face detector
	capture = cvCaptureFromCAM( CV_CAP_ANY );//initialize camera

	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,320);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,240);

	//cvNamedWindow("ColourDetection",1);

	return(0);
}

void VISION_DestoryCam()
{
	cvReleaseCapture( &capture );
	cvReleaseMemStorage( &storage );
}

int VISION_GrabFrame() // initialization
{
	//storage = cvCreateMemStorage( 0 );//hide this one when using webcam
	//frame = cvLoadImage("/home/odroid/photo/img.jpg",1);//for Pi cam
	frame = cvQueryFrame(capture);//hide this when using pi cam

	if( !frame )
	{
		fprintf( stderr, "Cannot query frame!\n" );
		return(0);
	}
	return(1);
}

int VISION_ShowOriginalFrame()
{
	//if(TuneMode==1)
	//count++;
	//if(count%3==0) //time to save pic
	cvShowImage("Original",frame);
//	cvMoveWindow("Original",850,50);
		//cvSaveImage("/mnt/rd/frame.ppm",frame,0);
	return(1);
}

void VISION_EnableTuneMode()
{
	TuneMode = 1;
}

void VISION_DisableTuneMode()
{
	TuneMode = 0;
}
	
int VISION_Tune_Color1(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)//so far, maximum detect 3 colors, return 2 blob coornidate (yellow, blue&Red)
{
	LoadValueColor(range,range1,range2);
	//printf("color1");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		//printf("i=%d, h1=%d",Invert[0],min[0]);
		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])  )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( (H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
		
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",850,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
		//printf("cnt1=%d\n",cnt1);
		for(i = N-1; i >= 0; --i)
		{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;//printf("m1=%d\n",m);
					}
				}
				break;
			}
		}
	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
		
	}
	
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Tune_Color2(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)//so far, maximum detect 3 colors, return 2 blob coornidate (yellow, blue&Red)
{
	LoadValueColor(range,range1,range2);
	//printf("color2");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[1]==0)
		{
			if( (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])  )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( (H <= min1[0] || H >= max1[0]) && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
		
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",850,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}

		cnt2++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
		//printf("cnt2=%d\n",cnt2);
		for(i = N-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;//printf("m2=%d\n",m);
					}
				}
				break;
			}
		}
	}
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 180, 0), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);	
	}
		
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Tune_Color3(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)//so far, maximum detect 3 colors, return 2 blob coornidate (yellow, blue&Red)
{
	LoadValueColor(range,range1,range2);
	//printf("color3");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[2]==0)
		{
			if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])  )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( (H <= min2[0] || H >= max2[0]) && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	cvShowImage("Threshold",imgThreshed);
//		cvMoveWindow("Threshold",850,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}

		cnt3++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
		//printf("cnt3=%d\n",cnt3);
		for(i = N-1; i >= 0; --i)
		{
			if(area1 > maxArea3[i])
			{
				maxArea3[i] = area1;
				contours3[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea3[m] < maxArea3[m+1])
					{
						tmp_area1 = maxArea3[m+1];
						tmp_cont = contours3[m+1];
						maxArea3[m+1] = maxArea3[m];
						contours3[m+1] = contours3[m];
						maxArea3[m] = tmp_area1;
						contours3[m] = tmp_cont;
					}
				}
				break;
			}
		}
	}
	
	if(cnt3 != 0)
	{
		CvRect rect = ((CvContour*)contours3[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
	}
	
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Game_1Color(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)//so far, maximum detect 3 colors, return 2 blob coornidate (yellow, blue&Red)
{
	LoadValueColor(range,range1,range2);
	//printf("Game color1\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		//printf("i=%d, h1=%d",Invert[0],min[0]);
		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])  )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( (H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	if(TuneMode)
		cvShowImage("Threshold",imgThreshed);
	//cvMoveWindow("Threshold",850,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
		//printf("cnt1=%d\n",cnt1);
		for(i = N-1; i >= 0; --i)
		{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;//printf("m1=%d\n",m);
					}
				}
				break;
			}
		}
	
	}
	
	if(cnt1 != 0)
		{
			CvRect rect = ((CvContour*)contours1[0])->rect;
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
			//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
			blob->Xmin = rect.x / 2;
			blob->Xmax = (rect.x + rect.width) / 2;
			blob->Ymin = rect.y;
			blob->Ymax = rect.y + rect.height;
		}
		else
		{
			blob->Xmin = 255;
			blob->Xmax = 255;
			blob->Ymin = 255;
			blob->Ymax = 255;
		}
		
	if(TuneMode)	
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Game_OBS(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,BlobCoord *blob1,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
	//printf("Game OBS\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m,k,tmp,tmp1; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])  )
			{
				data_ts[a*step_ts+b]=0;//Floor,black
			}else{
				data_ts[a*step_ts+b]=255;//others
				}		
	}
	
//	cvDilate(imgThreshed,imgThreshed,NULL,1);
//	cvErode(imgThreshed,imgThreshed,NULL,1); 

//	if(TuneMode)
//		cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",850,350);
	int max_y;
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}

			cnt1++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
			//printf("cnt1=%d\n",cnt1);
			for(i = N-1; i >= 0; --i)
			{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
				if(area1 > maxArea1[i])
				{
					maxArea1[i] = area1;
					contours1[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea1[m] < maxArea1[m+1])
						{
							tmp_area1 = maxArea1[m+1];
							tmp_cont = contours1[m+1];
							maxArea1[m+1] = maxArea1[m];
							contours1[m+1] = contours1[m];
							maxArea1[m] = tmp_area1;
							contours1[m] = tmp_cont;//printf("m1=%d\n",m);
						}
					}
					break;
				}
			}
			
		max_y=0;
		for(k=0;k<contours1[0]->total;++k)
		{
			pt=(CvPoint *)cvGetSeqElem(contours1[0],k);
			tmp=pt->y;
			if(tmp>max_y)
			{
				max_y=tmp;
			}
		}
		tmp1=max_y;		
			
	}	
	
	//For second color,red color folder
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		//printf("i=%d, h1=%d",Invert[0],min[0]);
		if(Invert[1]==0)
		{
			if( (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2]))
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( ((H <= min1[0] || H >= max1[0]) && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2]) )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
		
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt2++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
//		printf("cnt2=%d\n",cnt2);
		for(i = N-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;//printf("m2=%d\n",m);
					}
				}
				break;
			}
		}
	
	}
			
	if(cnt1 != 0)
	{
//		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(0, tmp1), cvPoint(320, 240),CV_RGB(0, 0, 255), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
		blob->Xmin = 0;
		blob->Xmax = 160; //320/2
		blob->Ymin = tmp1;
		blob->Ymax = 240;
	}
	else
	{
		blob->Xmin = 0;
		blob->Xmax = 160;//320/2
		blob->Ymin = 0;
		blob->Ymax = 240;
	}
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 180, 0), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);	
		blob1->Xmin = rect.x / 2;
		blob1->Xmax = (rect.x + rect.width) / 2;
		blob1->Ymin = rect.y;
		blob1->Ymax = rect.y + rect.height;
	}
	else
	{
		blob1->Xmin = 255;
		blob1->Xmax = 255;
		blob1->Ymin = 255;
		blob1->Ymax = 255;
	}	
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Game_BSK(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,BlobCoord *blob1,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,0.5f, 0.5f,0,2,8);
//	printf("Game Basketball\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
//		printf("INVERT0=%d\n",Invert[0]);
//		printf("INVERT1=%d\n",Invert[1]);
		//printf("i=%d, h1=%d",Invert[0],min[0]);
		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
	
//	cvDilate(imgThreshed,imgThreshed,NULL,1);
//	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
//	if(TuneMode)
//		cvShowImage("Threshold",imgThreshed);
	//cvMoveWindow("Threshold",850,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
//			printf("cnt1=%d\n",cnt1);
		for(i = N-1; i >= 0; --i)
		{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;//printf("m1=%d\n",m);
					}
				}
				break;
			}
		}
			
	
	}
	
		//For second color,red color folder
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		//printf("i=%d, h1=%d",Invert[0],min[0]);
		if(Invert[1]==0)
		{
			if( (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2]))
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
		else
		{
			if( ((H <= min1[0] || H >= max1[0]) && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2]) )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
		
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt2++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
//		printf("cnt2=%d\n",cnt2);
		for(i = N-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;//printf("m2=%d\n",m);
					}
				}
				break;
			}
		}
	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
//		printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
		blob->Xmin = rect.x / 2;
		blob->Xmax = (rect.x + rect.width) / 2;
		blob->Ymin = rect.y;
		blob->Ymax = rect.y + rect.height;
		
	}
	else
	{
		blob->Xmin = 255;
		blob->Xmax = 255;
		blob->Ymin = 255;
		blob->Ymax = 255;
	}
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 180, 0), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);	
		blob1->Xmin = rect.x / 2;
		blob1->Xmax = (rect.x + rect.width) / 2;
		blob1->Ymin = rect.y;
		blob1->Ymax = rect.y + rect.height;
	}
	else
	{
		blob1->Xmin = 255;
		blob1->Xmax = 255;
		blob1->Ymin = 255;
		blob1->Ymax = 255;
	}
	
	
	
	if(TuneMode)
	{
	
		sprintf(str,"%d",blob->Xmin);
		sprintf(str1,"%d",blob->Xmax);
		sprintf(str2,"%d",blob->Ymin);
		sprintf(str3,"%d",blob->Ymax);
		
		cvPutText(frame, "X1", cvPoint(5,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "X2", cvPoint(45,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "Y1", cvPoint(85,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "Y2", cvPoint(125,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, str, cvPoint(5,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str1, cvPoint(45,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str2, cvPoint(85,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str3, cvPoint(125,40), &font, CV_RGB(255,0,0));
		
		VISION_ShowOriginalFrame();
	}
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);


	return(1);	
	
}

int VISION_Game_ArrowDetect(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)//so far, maximum detect 3 colors, return 2 blob coornidate (yellow, blue&Red)
{
	LoadValueColor(range,range1,range2);
//	printf("Game Arrow\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;
	CvSeq *contours1[N]={0},*contours2[N]={0},*contours3[N]={0},*contours4[N]={0};
	CvPoint *pt;
	CvScalar data;
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; //a & b = for loop for height and width, l = for loop for contour, i & m= for compare area
	int maxX[6]={0},maxY[6]={0};
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
	double area1=0,tmp_area1=0;
	int corner_X[4],corner_Y[4],cntcorner1=0,cntcorner2=0,cntcorner3=0,cntcorner4=0,xxxx=0;
	
	int LnX_min=0,LnX_max=0,LnY_min=0,LnY_max=0;
	int flag=0,flag1;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		//printf("i=%d, h1=%d\n",Invert[0],min[0]);

		if((H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
		|| (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]) )
		{
			data_ts[a*step_ts+b]=255;
			//printf("%d\n",data_ts[a*step_ts+b]);
		}else
		{
			data_ts[a*step_ts+b]=0;
			//printf("%d\n",data_ts[a*step_ts+b]);
		}
	}
		
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
//	if(TuneMode)
//		cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",500,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
	
		total =0;
		total_S=0;
		total_V=0;

		for(l = 0;l<contour->total;++l)
		{
			pt = (CvPoint *)cvGetSeqElem(contour,l);
			H = data_hsv[step_hsv*pt->y+chanels_hsv*pt->x+0];
			S = data_hsv[step_hsv*pt->y+chanels_hsv*pt->x+1];
			V = data_hsv[step_hsv*pt->y+chanels_hsv*pt->x+2];
			total = H + total;
			total_S= S + total_S;
			total_V= V + total_V;
		}

		avg = total / (contour->total);
		avg_S=total_S / (contour->total);
		avg_V=total_V / (contour->total);

		if((avg>=min1[0])&&(avg<=max1[0])&&(avg_S>=min1[1])&&(avg_S<=max1[1])&&(avg_V>=min1[2])&&(avg_V<=max1[2]))
		{
			cnt2++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
//			printf("cnt2=%d\n",cnt2);
			for(i = N-1; i >= 0; --i)
			{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
				if(area1 > maxArea2[i])
				{
					maxArea2[i] = area1;
					contours2[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea2[m] < maxArea2[m+1])
						{
							tmp_area1 = maxArea2[m+1];
							tmp_cont = contours2[m+1];
							maxArea2[m+1] = maxArea2[m];
							contours2[m+1] = contours2[m];
							maxArea2[m] = tmp_area1;
							contours2[m] = tmp_cont;//printf("m1=%d\n",m);
						}
					}
					break;
				}
			}
			cvSeqRemove(contour,0);
		}
		else if((avg>=min2[0])&&(avg<=max2[0])&&(avg_S>=min2[1])&&(avg_S<=max2[1])&&(avg_V>=min2[2])&&(avg_V<=max2[2]))
		{
			cnt3++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
//			printf("cnt3=%d\n",cnt3);
			for(i = N-1; i >= 0; --i)
			{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
				if(area1 > maxArea3[i])
				{
					maxArea3[i] = area1;
					contours3[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea3[m] < maxArea3[m+1])
						{
							tmp_area1 = maxArea3[m+1];
							tmp_cont = contours3[m+1];
							maxArea3[m+1] = maxArea3[m];
							contours3[m+1] = contours3[m];
							maxArea3[m] = tmp_area1;
							contours3[m] = tmp_cont;//printf("m1=%d\n",m);
						}
					}
					break;
				}
			}
			cvSeqRemove(contour,0);
		}
	}
	
	for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
	for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
//		printf("INVERT0=%d\n",Invert[0]);
//		printf("INVERT1=%d\n",Invert[1]);
		//printf("i=%d, h1=%d",Invert[0],min[0]);
		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}else
			{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);
				}
		}
	}
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;// after threhold , back to trace color cluster. file 0 data read in. if fulfil, then cnt=1 and it is 1st color
//		printf("cnt1=%d\n",cnt1);
		for(i = N-1; i >= 0; --i)
		{//printf("area1=%d,maxArea=%d\n",area1,maxArea1[i]);
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;//printf("m1=%d\n",m);
					}
				}
				break;
			}
		}	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
		LnX_min = rect.x / 2;
		LnX_max = (rect.x + rect.width) / 2;
		LnY_min = rect.y;
		LnY_max = rect.y + rect.height;
	}
		
	CvSeq* result;
	int f;
	
	if(cnt2 != 0)
	{
		result = cvApproxPoly(contours2[0],sizeof(CvContour),storage,CV_POLY_APPROX_DP,cvContourPerimeter(contours2[0])*0.085,0);
//		printf("%d\n",result->total);//cvContourPerimeter(contours2[0])*0.02
		if(result->total == 4)
		{
			CvPoint *pt1[4],pt2[4];
			for(f=0;f<4;f++)
			{
				pt1[f]=(CvPoint*)cvGetSeqElem(result,f);
//				printf("%d %d \n",pt1[f]->x,pt1[f]->y);
			}
//			printf("%d %d \n",pt1[0]->x,pt1[0]->y); 
//			printf("%d %d \n",pt1[1]->x,pt1[1]->y);
//			printf("%d %d \n",pt1[2]->x,pt1[2]->y);
//			printf("%d %d \n",pt1[3]->x,pt1[3]->y);
//			
			cvLine(frame,*pt1[0],*pt1[1],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[1],*pt1[2],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[2],*pt1[3],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[3],*pt1[0],CV_RGB(0,255,0),2,8,0);
			
			int midX[4]={0},midY[4]={0},cent=0;
			
			pt2[0].x = midX[0]=(pt1[0]->x + pt1[1]->x)/2;
			pt2[0].y = midY[0]=(pt1[0]->y + pt1[1]->y)/2;
				
			pt2[1].x = midX[1]=(pt1[1]->x + pt1[2]->x)/2;
			pt2[1].y = midY[1]=(pt1[1]->y + pt1[2]->y)/2;
			
			pt2[2].x = midX[2]=(pt1[2]->x + pt1[3]->x)/2;
			pt2[2].y = midY[2]=(pt1[2]->y + pt1[3]->y)/2;
			
			pt2[3].x = midX[3]=(pt1[3]->x + pt1[0]->x)/2;
			pt2[3].y = midY[3]=(pt1[3]->y + pt1[0]->y)/2;
			
			cent = 	(pt1[0]->x + pt1[2]->x)/2;
//			printf("cent=%d",cent);	
//			printf("mid0= %d %d\n",midX[0],midY[0]);
//			printf("mid1= %d %d\n",midX[1],midY[1]);
//			printf("mid2= %d %d\n",midX[2],midY[2]);
//			printf("mid3= %d %d\n",midX[3],midY[3]);
//			
//			unsigned char *data_hsv = (unsigned char *)imgHSV->imageData;  
//			int step_hsv = imgHSV->widthStep/sizeof(unsigned char) ;  
//			int chanels_hsv = imgHSV->nChannels;
			int a,b,cnt[4]={0},c=0,tmp1_c=0,tmp2_c=0,cnt_tmp=0,cntwhite=0,tmp3_c,counter[4]={0},d=0,count=0;  
			unsigned char H,S,V;
			unsigned char temp[6];
			
			for(d=0;d<4;d++)
			{
				if(pt1[d]->y +6 <241){
				for(a=(pt1[d]->y)-5;a<(pt1[d]->y)+6;a++)
				for(b=(pt1[d]->x)-5;b<(pt1[d]->x)+6;b++)
				{
					H = data_hsv[a*step_hsv+b*chanels_hsv+0];
					S = data_hsv[a*step_hsv+b*chanels_hsv+1];
					V = data_hsv[a*step_hsv+b*chanels_hsv+2];
					
					if(H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
					{
							counter[d]++;
//							printf("%d counter = %d \n",d,counter[d]);
							if(counter[d] == 20)
							{
								count++;
							}
//							printf("count = %d\n",count);
							if(d==3 && count==4 && counter[3]==20)
							{
//								printf("Black BG %d\n",counter[3]);
								temp[0] = min2[0];
								temp[1] = max2[0];
								temp[2] = min2[1];
								temp[3] = max2[1];
								temp[4] = min2[2];
								temp[5] = max2[2];								
								
							}							
					}
					else
					if(H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
					{
							counter[d]++;
//							printf("%d counter = %d \n",d,counter[d]);
							if(counter[d] == 20)
							{
								count++;
							}
//							printf("count = %d\n",count);
							if(d==3 && count==4 && counter[3] == 20)
							{
//								printf("White BG %d\n",counter[3]);
								temp[0] = min1[0];
								temp[1] = max1[0];
								temp[2] = min1[1];
								temp[3] = max1[1];
								temp[4] = min1[2];
								temp[5] = max1[2];								
							}							
					}			
				}}
			}
			
			for(c=0;c<4;c++)
			{
				if(midY[c]+6 < 241){
				for(a=midY[c]-5;a<midY[c]+6;a++)
				for(b=midX[c]-5;b<midX[c]+6;b++)
				{
					H = data_hsv[a*step_hsv+b*chanels_hsv+0];
					S = data_hsv[a*step_hsv+b*chanels_hsv+1];
					V = data_hsv[a*step_hsv+b*chanels_hsv+2];
					
					if(H >= temp[0] && H <= temp[1] && S >= temp[2] && S<= temp[3] && V>=temp[4] && V<=temp[5])
					{
						cnt[c]++;
//						printf("cnt=%d\n",cnt[c]);
//						if(cnt[c] > 5)	
//							cvCircle(frame,pt2[c],7,CV_RGB(0,100,60*c),2,8,0);								
					}			
				}
				}   
				if(cnt[c]>5)
				{	
					cntwhite++;						
					//printf("cntwhite = %d\n",cntwhite);
					if(cntwhite == 1) tmp1_c = c;
					if(cntwhite == 2) tmp2_c = c;
					
					//printf("tmp1_c = %d \n",tmp1_c);
					//printf("tmp2_c = %d \n",tmp2_c);
					//printf("c = %d \n",c);	
					if(cntwhite ==2)
					{
						//printf("pt2[tmp1_c].y = %d, pt2[tmp2_c].y = %d \n",pt2[tmp1_c].y,pt2[tmp2_c].y);
						tmp3_c=tmp1_c -tmp2_c;
						
						//if(tmp3_c==4) tmp3_c=0;
						//if(tmp3_c==5) tmp3_c=1;
						//printf("tmp3_c=%d\n",tmp3_c);
						
						if(pt2[tmp1_c].y > pt2[tmp2_c].y)
						{
							cvCircle(frame,pt2[tmp2_c],7,CV_RGB(0,100,60*c),2,8,0);
							//printf("pt2[tmp2_c].x = %d ,pt2[tmp3_c].x= %d\n", pt2[tmp2_c].x,pt2[tmp3_c].x);
							//if(pt2[tmp2_c].x<pt2[tmp3_c].x)							
							if(tmp3_c == 2 || tmp3_c == -2)
							{
							printf("Go straight\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 80;
							ArX_max = 80;
							ArY_min = 110;
							ArY_max = 130;
							}
							else if((tmp3_c == 1 || tmp3_c == 3 || tmp3_c == -1 || tmp3_c == -3)&& pt2[tmp2_c].x > cent)
							{
							printf("turn right\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 120;
							ArX_max = 120;
							ArY_min = 110;
							ArY_max = 130;
					
							}
							
						}
						if(pt2[tmp2_c].y > pt2[tmp1_c].y)
						{
							cvCircle(frame,pt2[tmp1_c],7,CV_RGB(0,100,60*c),2,8,0); 
							if(tmp3_c == 2 || tmp3_c == -2)
							{
							printf("Go straight\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 80;
							ArX_max = 80;
							ArY_min = 110;
							ArY_max = 130;
					
							}
							else if(pt2[tmp1_c].x < cent)
							{
							printf("turn left\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 40;
							ArX_max = 40;
							ArY_min = 110;
							ArY_max = 130;
					
							}
						}
						if(pt2[tmp2_c].y == pt2[tmp1_c].y) 
						printf("Same position\n");
						cntwhite=0;
					}   
				}
			}		
		}	

	}	
	
	if(cnt3 != 0)
	{
		result = cvApproxPoly(contours3[0],sizeof(CvContour),storage,CV_POLY_APPROX_DP,cvContourPerimeter(contours3[0])*0.085,0);
//		printf("%d\n",result->total);//cvContourPerimeter(contours2[0])*0.02
		if(result->total == 4)
		{
			CvPoint *pt1[4],pt2[4];
			for(f=0;f<4;f++)
			{
				pt1[f]=(CvPoint*)cvGetSeqElem(result,f);
//				printf("%d %d \n",pt1[f]->x,pt1[f]->y);
			}
//			printf("%d %d \n",pt1[0]->x,pt1[0]->y); 
//			printf("%d %d \n",pt1[1]->x,pt1[1]->y);
//			printf("%d %d \n",pt1[2]->x,pt1[2]->y);
//			printf("%d %d \n",pt1[3]->x,pt1[3]->y);
//			
			cvLine(frame,*pt1[0],*pt1[1],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[1],*pt1[2],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[2],*pt1[3],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[3],*pt1[0],CV_RGB(0,255,0),2,8,0);
			
			int midX[4]={0},midY[4]={0},cent=0;
			
			pt2[0].x = midX[0]=(pt1[0]->x + pt1[1]->x)/2;
			pt2[0].y = midY[0]=(pt1[0]->y + pt1[1]->y)/2;
				
			pt2[1].x = midX[1]=(pt1[1]->x + pt1[2]->x)/2;
			pt2[1].y = midY[1]=(pt1[1]->y + pt1[2]->y)/2;
			
			pt2[2].x = midX[2]=(pt1[2]->x + pt1[3]->x)/2;
			pt2[2].y = midY[2]=(pt1[2]->y + pt1[3]->y)/2;
			
			pt2[3].x = midX[3]=(pt1[3]->x + pt1[0]->x)/2;
			pt2[3].y = midY[3]=(pt1[3]->y + pt1[0]->y)/2;
			
			cent = 	(pt1[0]->x + pt1[2]->x)/2;
//			printf("cent=%d",cent);	
//			printf("mid0= %d %d\n",midX[0],midY[0]);
//			printf("mid1= %d %d\n",midX[1],midY[1]);
//			printf("mid2= %d %d\n",midX[2],midY[2]);
//			printf("mid3= %d %d\n",midX[3],midY[3]);
//			
			int a,b,cnt[4]={0},c=0,tmp1_c=0,tmp2_c=0,cnt_tmp=0,cntwhite=0,tmp3_c,counter[4]={0},d=0,count=0;  
			unsigned char H,S,V;
			unsigned char temp[6];
			
			for(d=0;d<4;d++)
			{
				if(pt1[d]->y +6 <241){
				for(a=(pt1[d]->y)-5;a<(pt1[d]->y)+6;a++)
				for(b=(pt1[d]->x)-5;b<(pt1[d]->x)+6;b++)
				{
					H = data_hsv[a*step_hsv+b*chanels_hsv+0];
					S = data_hsv[a*step_hsv+b*chanels_hsv+1];
					V = data_hsv[a*step_hsv+b*chanels_hsv+2];
					
					if(H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
					{
							counter[d]++;
//							printf("%d counter = %d \n",d,counter[d]);
							if(counter[d] == 20)
							{
								count++;
							}
//							printf("count = %d\n",count);
							if(d==3 && count==4 && counter[3]==20)
							{
//								printf("White BG %d\n",counter[3]);
								temp[0] = min1[0];
								temp[1] = max1[0];
								temp[2] = min1[1];
								temp[3] = max1[1];
								temp[4] = min1[2];
								temp[5] = max1[2];								
								
							}							
					}
					else
					if(H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
					{
							counter[d]++;
//							printf("%d counter = %d \n",d,counter[d]);
							if(counter[d] == 20)
							{
								count++;
							}
//							printf("count = %d\n",count);
							if(d==3 && count==4 && counter[3] == 20)
							{
//								printf("Black BG %d\n",counter[3]);
								temp[0] = min2[0];
								temp[1] = max2[0];
								temp[2] = min2[1];
								temp[3] = max2[1];
								temp[4] = min2[2];
								temp[5] = max2[2];								
							}							
					}			
				}}
			}
			
			for(c=0;c<4;c++)
			{
				if(midY[c]+6 < 241){
				for(a=midY[c]-5;a<midY[c]+6;a++)
				for(b=midX[c]-5;b<midX[c]+6;b++)
				{
					H = data_hsv[a*step_hsv+b*chanels_hsv+0];
					S = data_hsv[a*step_hsv+b*chanels_hsv+1];
					V = data_hsv[a*step_hsv+b*chanels_hsv+2];
					//if(H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
					if(H >= temp[0] && H <= temp[1] && S >= temp[2] && S<= temp[3] && V>=temp[4] && V<=temp[5]) //white 135,black 246
					{
						cnt[c]++;
//						printf("cnt=%d\n",cnt[c]);
//						if(cnt[c] > 20)	
//							cvCircle(frame,pt2[c],7,CV_RGB(0,100,60*c),2,8,0);								
					}			
				}
				}   
				if(cnt[c]>5)
				{	
					cntwhite++;						
					//printf("cntwhite = %d\n",cntwhite);
					if(cntwhite == 1) tmp1_c = c;
					if(cntwhite == 2) tmp2_c = c;
					
					//printf("tmp1_c = %d \n",tmp1_c);
					//printf("tmp2_c = %d \n",tmp2_c);
					//printf("c = %d \n",c);	
					if(cntwhite ==2)
					{
						//printf("pt2[tmp1_c].y = %d, pt2[tmp2_c].y = %d \n",pt2[tmp1_c].y,pt2[tmp2_c].y);
						tmp3_c=tmp1_c -tmp2_c;
						
						//if(tmp3_c==4) tmp3_c=0;
						//if(tmp3_c==5) tmp3_c=1;
						//printf("tmp3_c=%d\n",tmp3_c);
						
						if(pt2[tmp1_c].y > pt2[tmp2_c].y)
						{
							cvCircle(frame,pt2[tmp2_c],7,CV_RGB(0,100,60*c),2,8,0);
							//printf("pt2[tmp2_c].x = %d ,pt2[tmp3_c].x= %d\n", pt2[tmp2_c].x,pt2[tmp3_c].x);
							//if(pt2[tmp2_c].x<pt2[tmp3_c].x)							
							if(tmp3_c == 2 || tmp3_c == -2)
							{
							printf("Go straight\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 80;
							ArX_max = 80;
							ArY_min = 110;
							ArY_max = 130;
							}
							else //if((tmp3_c == 1 || tmp3_c == 3 || tmp3_c == -1 || tmp3_c == -3)&& pt2[tmp2_c].x > cent)
							{
							printf("turn right\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 120;
							ArX_max = 120;
							ArY_min = 110;
							ArY_max = 130;
							}
							
						}
						if(pt2[tmp2_c].y > pt2[tmp1_c].y)
						{
							cvCircle(frame,pt2[tmp1_c],7,CV_RGB(0,100,60*c),2,8,0); 
							if(tmp3_c == 2 || tmp3_c == -2)
							{
							printf("Go straight\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 80;
							ArX_max = 80;
							ArY_min = 110;
							ArY_max = 130;
							}
							else //if(pt2[tmp1_c].x < cent)
							{
							printf("turn left\n");
							flag = 1;
							flag1 = 1;
							ArX_min = 40;
							ArX_max = 40;
							ArY_min = 110;
							ArY_max = 130;
							}
						}
						if(pt2[tmp2_c].y == pt2[tmp1_c].y) 
						printf("Same position\n");
						cntwhite=0;
					}   
				}
			}		
		}
	}
	
	if(cnt1 != 0)//have Line
	{
		if(flag==0)// no arrow
		{
			blob->Xmin = LnX_min;
			blob->Xmax = LnX_max;
			blob->Ymin = LnY_min;
			blob->Ymax = LnY_max;
			ArX_min=255;
			ArX_max=255;
			ArY_min=255;
			ArY_max=255;	
		}
		else  //have arrow
		{
			if(LnY_min < 200) //upper point smaller than 200, send Line
			{
				blob->Xmin = LnX_min;
				blob->Xmax = LnX_max;
				blob->Ymin = LnY_min;
				blob->Ymax = LnY_max;	
			}
			else// upper point greater than 200, send Arrow
			{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;	
			}
		}
	}
	else  //no line
	{
		if(flag == 0)//No arrow
		{
			/*if(flag1 != 1)//Initial blob,centroid
			{
				blob->Xmin = 75;
				blob->Xmax = 85;
				blob->Ymin = 110;
				blob->Ymax = 130;	
			}
			else//Send the last Arrow position
			{*/
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;
			//}		
				
		}
		else  //have arrow
		{
			blob->Xmin = ArX_min;
			blob->Xmax = ArX_max;
			blob->Ymin = ArY_min;
			blob->Ymax = ArY_max;	
		}		
	}
	
	flag = 0;
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

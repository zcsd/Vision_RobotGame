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

int colorNo;
CvMemStorage *storage=0;
IplImage  *frame;
CvCapture *capture;

char TuneMode = 0;

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
//For webcam
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

		if((avg>=min[0])&&(avg<=max[0])&&(avg_S>=min[1])&&(avg_S<=max[1])&&(avg_V>=min[2])&&(avg_V<=max[2]))
		{
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

		if((avg>=min2[0])&&(avg<=max2[0])&&(avg_S>=min2[1])&&(avg_S<=max2[1])&&(avg_V>=min2[2])&&(avg_V<=max2[2]))
		{
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
	//cvShowImage("Threshold",imgThreshed);
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

		if((avg>=min[0])&&(avg<=max[0])&&(avg_S>=min[1])&&(avg_S<=max[1])&&(avg_V>=min[2])&&(avg_V<=max[2]))
		{
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
	//VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Game_OBS(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,BlobCoord *blob1,char *gameplay, Color color)
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
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	cvShowImage("Threshold",imgThreshed);
	//cvMoveWindow("Threshold",850,350);
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
		
	if(cnt1 != 0)
	{
//		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(0, tmp1), cvPoint(320, 240),CV_RGB(0, 0, 255), 2, 8, 0);
		//printf("red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
		blob->Xmin = 0;
		blob->Xmax = 320;
		blob->Ymin = tmp1;
		blob->Ymax = 240;
	}
	else
	{
		blob->Xmin = 0;
		blob->Xmax = 320;
		blob->Ymin = 0;
		blob->Ymax = 240;
	}
	
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

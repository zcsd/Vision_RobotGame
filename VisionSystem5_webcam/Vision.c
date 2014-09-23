#include <stdio.h>
#include <time.h>
#include "Vision.h"

#define NUMBER_OF_BLOB 3   //the number of bounding box
#define BLOB_SIZE 50
#define N 3
unsigned char min[3]= {0};
unsigned char max[3]= {0};
	
unsigned char min1[3]= {0};
unsigned char max1[3]= {0};
	
unsigned char min2[3]= {0};
unsigned char max2[3]= {0};
	
unsigned char min3[3]= {0};
unsigned char max3[3]= {0};

CvMemStorage *storage=0;
IplImage  *frame;
CvCapture *capture;
//CvMemStorage *storage = cvCreateMemStorage(0);    //setup memory buffer, needed by find contours
CvSeq *contours[NUMBER_OF_BLOB], *tmp_cont, *contour;

int area, tmp_area, i, j, k,m;

char TuneMode = 0;

void LoadValueColor(VisionRange range,VisionRange range1,VisionRange range2)
{
	min[0]=range.Hmin;
	min[1]=range.Smin;
	min[2]=range.Lmin;
	max[0]=range.Hmax;
	max[1]=range.Smax;
	max[2]=range.Lmax;
	min1[0]=range1.Hmin;
	min1[1]=range1.Smin;
	min1[2]=range1.Lmin;
	max1[0]=range1.Hmax;
	max1[1]=range1.Smax;
	max1[2]=range1.Lmax;
	min2[0]=range2.Hmin;
	min2[1]=range2.Smin;
	min2[2]=range2.Lmin;
	max2[0]=range2.Hmax;
	max2[1]=range2.Smax;
	max2[2]=range2.Lmax;
}
	
      
//For webcam
int VISION_InitCam()
{
	storage = cvCreateMemStorage( 0 );                                 //setup memory buffer, needed by the face detector
	capture = cvCaptureFromCAM( CV_CAP_ANY );                                   //initialize camera
	
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

int VISION_GrabFrame()  // initialization
{
	//storage = cvCreateMemStorage( 0 );//hide this one when using webcam    
	//frame = cvLoadImage("/home/pi/photo/img.jpg",1);//for Pi cam
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
	if(TuneMode==1)
		cvShowImage("Original",frame);	
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


int VISION_PrcessFrame(VisionRange range,VisionRange range1,VisionRange range2, BlobCoord *blob, BlobCoord *blob1,char *gameplay,char *color)//so far, maximum detect 3 colors, return 2 blob coornidate (yellow, blue&Red)
{
	 
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV);   	
	
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		if(*color!='3')
		{
	 
	      switch(*color)
	     {
	       case'0':
	         cvInRangeS(imgHSV,cvScalar(range.Hmin,range.Smin,range.Lmin,0),cvScalar(range.Hmax,range.Smax,range.Lmax,0),imgThreshed);
	         break;
			 
	       case'1':
		     cvInRangeS(imgHSV,cvScalar(range1.Hmin,range1.Smin,range1.Lmin,0),cvScalar(range1.Hmax,range1.Smax,range1.Lmax,0),imgThreshed);
		     break;
			 
	       case'2':
	         cvInRangeS(imgHSV,cvScalar(range2.Hmin,range2.Smin,range2.Lmin,0),cvScalar(range2.Hmax,range2.Smax,range2.Lmax,0),imgThreshed);
	         break;
	     }
	     if(TuneMode==1)
	     {
	         cvShowImage("ColourDetection", imgThreshed);
		 }
    
        }
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;  
    int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char) ;  
    int chanels_hsv = imgHSV->nChannels;
    int a,b;  
    unsigned char H,S,V;
    int maxX[6]={0},maxY[6]={0};
	int o,p,q;
	
	LoadValueColor(range,range1,range2);
	
	 for(a=0;a<imgHSV->height;a++)// cut window in Y [0, 240]. if not, imgHSV->height read from above is 240
        for(b=0;b<imgHSV->width;b++)//cut window in X [0, 320] example for(b=30;b<300;b++)
        {
			H = data_hsv[a*step_hsv+b*chanels_hsv+0];
			S = data_hsv[a*step_hsv+b*chanels_hsv+1];
			V = data_hsv[a*step_hsv+b*chanels_hsv+2];
			
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])
			||  (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
			||  (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])  )
			{
				data_ts[a*step_ts+b]=255;
				//printf("%d\n",data_ts[a*step_ts+b]);
				
			}else{	
		
				data_ts[a*step_ts+b]=0;
				//printf("%d\n",data_ts[a*step_ts+b]);

			    }				
	     }  
	     
	int l;
	int total, avg,total_S, avg_S,total_V, avg_V; 
	double area1,tmp_area1;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0; 
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};

    CvSeq *contours1[N],*contours2[N],*contours3[N],*contours4[N];
	CvPoint *pt;
	CvScalar data;
	if(*color=='3')
	{
		if(TuneMode==1)
		{
		
	      cvShowImage("ColourDetection", imgThreshed);
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
			
            for(i = N-1; i >= 0; --i)
            {
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
                     contours1[m] = tmp_cont;
                    }
                }
              break;
              }
            }	   
			
		}
		else if((avg>=min1[0])&&(avg<=max1[0])&&(avg_S>=min1[1])&&(avg_S<=max1[1])&&(avg_V>=min1[2])&&(avg_V<=max1[2])) 	
		{
			cnt2++; // after threhold , back to trace color cluster. file 1 data read in. if fulfil, then cnt=2 and it is 2nd color
			
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
                     contours2[m] = tmp_cont;
                    }
                }
              break;
              }
            }	   
				
		}
		else if((avg>=min2[0])&&(avg<=max2[0])&&(avg_S>=min2[1])&&(avg_S<=max2[1])&&(avg_V>=min2[2])&&(avg_V<=max2[2])) 	
		{
			cnt3++;// after threhold , back to trace color cluster. file 2 data read in. if fulfil, then cnt=3 and it is 3rd color
			
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
	
	
switch(*gameplay)
	{
		
		case'B'://Mara
		switch(*color)
		{
			case'0':
				
			if(cnt1 != 0)
			{
				CvRect rect = ((CvContour*)contours1[0])->rect;
			  
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
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
	         break;
	         
	        case'1':
	         
			if(cnt2 != 0)
	        {
				CvRect rect = ((CvContour*)contours2[0])->rect;
		       
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
		
		         
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
			break;
			
			case'2':
			
			if(cnt3 != 0)
			{
				CvRect rect = ((CvContour*)contours3[0])->rect;
							 
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 255), 2, 8, 0);
							  
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
	          break;
	      
			default:
			
			if(cnt1 != 0)
			{		 
				CvRect rect = ((CvContour*)contours1[0])->rect;
							
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
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
	         break;
		}
		break;
	 
		case 'A': //Lift and Carry
		
			if(cnt1 != 0)
			{
				CvRect rect = ((CvContour*)contours1[0])->rect;
					
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
				maxX[0]=rect.x;
				maxX[1]=rect.x+rect.width;
				maxY[0]=rect.y;
				maxY[1]=rect.y+rect.height;
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
					
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
				//printf("Yellow: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
				maxX[2]=rect.x;
				maxX[3]=rect.x+rect.width;
				maxY[2]=rect.y;
				maxY[3]=rect.y+rect.height;
			}
			else
			{
				blob->Xmin = 255;				
				blob->Xmax = 255;
				blob->Ymin = 255;
				blob->Ymax = 255;
			}
	
			if(cnt3 != 0)
			{
				CvRect rect = ((CvContour*)contours3[0])->rect;
					
				cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 255), 2, 8, 0);
				//printf("Blue: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
				maxX[4]=rect.x;
				maxX[5]=rect.x+rect.width;
				maxY[4]=rect.y;
				maxY[5]=rect.y+rect.height;
			}
			else
			{
				blob->Xmin = 255;				
				blob->Xmax = 255;
				blob->Ymin = 255;
				blob->Ymax = 255;
			}
			break;
				
	  case'D':   //OBS
	 
		if(cnt2 != 0)//OBS Yellow
		{
			CvRect rect = ((CvContour*)contours2[0])->rect;
				
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
			//printf("Yellow: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
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
	
		if(cnt1!=0 && cnt3!=0) //red and blue
		{
			if(cnt1!=0)
			{
					CvRect rect = ((CvContour*)contours1[0])->rect;
					
					maxX[0]=rect.x;
					maxX[1]=rect.x+rect.width;
					maxY[0]=rect.y;
					maxY[1]=rect.y+rect.height;			
			}
			if(cnt3!=0)
			{	
				CvRect rect = ((CvContour*)contours3[0])->rect;
			
				maxX[2]=rect.x;
				maxX[3]=rect.x+rect.width;
				maxY[2]=rect.y;
				maxY[3]=rect.y+rect.height;
			}
		
			for(o=0;o<3;o++)
			{
				for(p=0;p<3;p++)
				{
					if(maxX[p]>maxX[p+1])
					{
						q=maxX[p];
						maxX[p]=maxX[p+1];
						maxX[p+1]=q;
					}
	
					if(maxY[p]>maxY[p+1])
					{
						q=maxY[p];
						maxY[p]=maxY[p+1];
						maxY[p+1]=q;
					}
				}
			}
		
			cvRectangle(frame, cvPoint(maxX[0], maxY[0]), cvPoint(maxX[3], maxY[3]),CV_RGB(255, 0, 255), 2, 8, 0);
			//printf("Blue and red: (%d , %d) (%d , %d)\n", maxX[0],maxY[0],maxX[3],maxY[3]);
					blob->Xmin = maxX[0]/ 2;				
					blob->Xmax = maxX[3] / 2;
					blob->Ymin = maxY[0];
					blob->Ymax = maxY[3];
	
		}
		else
		{
			blob->Xmin = 255;				
			blob->Xmax = 255;
			blob->Ymin = 255;
			blob->Ymax = 255;
		}
		
		if(cnt1 != 0) //red
		{
			CvRect rect = ((CvContour*)contours1[0])->rect;
			
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
			//printf("Red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
			   
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
		
		if(cnt3 != 0) //blue
		{
			CvRect rect = ((CvContour*)contours3[0])->rect;
			
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 255), 2, 8, 0);
			//printf("Blue: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
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
	
	break;
	
	case'P'://PK
		
		switch(*color)
		{
			case'0':
	
				if(cnt1 != 0)
				{
					CvRect rect = ((CvContour*)contours1[0])->rect;
							
					cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
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
			
	         break;
	         
			case'1':
							 
				if(cnt2 != 0)
				{
					CvRect rect = ((CvContour*)contours2[0])->rect;
								
					cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
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
			break;
	          
	          case'2':
				if(cnt3 != 0)
				{
					CvRect rect = ((CvContour*)contours3[0])->rect;
							 
					cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 255), 2, 8, 0);
							  
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
			break;
	      
			default:
	       
				if(cnt1 != 0)
				{
					CvRect rect = ((CvContour*)contours1[0])->rect;

					cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
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
			break;
		}
		break;
	}	
		
   if(TuneMode==1)
   {
	   cvShowImage("Original",frame);	
   }

	//cvReleaseImage(&frame); //hide this when using webcam
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);	
}








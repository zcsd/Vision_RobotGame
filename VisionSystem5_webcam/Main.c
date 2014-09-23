#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "Uart.h"
#include "File.h"
#include "Vision.h"

#include <cv.h>
#include <highgui.h>

#define MAX_DIFF_TYPE 3

int prog_quit = 0;

const int slider_max = 255;
int slider;
int h1_slider, h2_slider;
int s1_slider, s2_slider;
int l1_slider, l2_slider;
int bar;

char Filename[50] = "";
char File1name[50] = "";
char Foldername[50] = "/home/pi/conf/";
char Gamename[50] = "Game";
char Extname[10] = ".conf";

char Test[MAX_DIFF_TYPE][50];

VisionRange Range[3];
char key;
char color;

char FrameGrabDone = 0;


void *threadUARTRx(void *arg)
{	
	while(prog_quit!=-1)
	{
		UART_RX();
		if(rx_buffer[0] == 'q')
		prog_quit=-1;
	}
	//printf("UART has quit\r\n");
	return NULL;
}

void Readfile(char* Gname, int x)
{
	strcpy(Test[x],Gname);
	strcpy(File1name, Foldername);
	strcat(File1name, Gname);
	strcat(File1name, Extname);
	FILE_Read(File1name, &Range[x]);
}            
	
void *threadFrameGrab(void *arg)
{
	while(1)
	{
		//TODO: vision frame grabbing function
		VISION_GrabFrame();
		FrameGrabDone = 1;
	}
	//printf("Time - Frame Grabbing: %.2f\r\n", time_spent);
}

void TX_Char(char x, BlobCoord Blob, BlobCoord Blob1)
{
	UART_TX_Char(13);
	UART_TX_Char(x);

	UART_TX_Char(Blob.Xmin);
	UART_TX_Char(Blob.Xmax);
	UART_TX_Char(Blob.Ymin);
	UART_TX_Char(Blob.Ymax);

	if(x==3)
	{
		UART_TX_Char(13);
	    UART_TX_Char(4);	
	 
	    UART_TX_Char(Blob1.Xmin);
		UART_TX_Char(Blob1.Xmax);
		UART_TX_Char(Blob1.Ymin);
		UART_TX_Char(Blob1.Ymax);	
	}
}

void RunVision_ColorSeg_Struct(VisionRange Range, VisionRange Range1, VisionRange Range2,char *key1,char *color1)
{
	BlobCoord Blob;
	BlobCoord Blob1; 
	
	clock_t begin, end;
	double time_spent;
	begin = clock();	
	
	//if(FrameGrabDone==1);
	if(VISION_GrabFrame())
	{
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("GrabFrame: %.2f fps\t", 1/time_spent);
		VISION_ShowOriginalFrame();
		
		begin = clock();	
	
		VISION_PrcessFrame(Range,Range1,Range2,&Blob,&Blob1,key1,color1);		
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("ProcFrame: %.2f fps\t", 1/time_spent);
		
		switch(*key1)
		{
			case'A':
		    TX_Char(1,Blob,Blob1);
		    break;
			
		    case'B':
		    TX_Char(1,Blob,Blob1);
		    break;
		    
		    case'P':
		    TX_Char(2,Blob,Blob1);
		    break;
		    
		    case'D':
		    TX_Char(3,Blob,Blob1);
		    break;
		}
		
		FrameGrabDone=0;
		//printf("\r\nBlobX: %d\t%d\r\nBlobY: %d\t%d", Blob.Xmin, Blob.Xmax, Blob.Ymin, Blob.Ymax);
	}
}


/*void RunVision_ColorSeg_Num(int hmin, int hmax, int smin, int smax, int lmin, int lmax)
{
	VisionRange Range = {hmin, hmax, smin, smax, lmin, lmax};
	RunVision_ColorSeg_Struct1(Range);
	
}*/


void TunebtnHandler(int pos)
{
	switch(bar)
	{
		case 0: strcpy(Filename, Foldername);
				strcat(Filename, Test[bar]);
				strcat(Filename, Extname);
				color='0';
		//printf("%s",Filename);
		break;
		
		case 1: strcpy(Filename, Foldername);
				strcat(Filename, Test[bar]);
				strcat(Filename, Extname);
				color='1';
		//printf("%s",Filename);
		break;
		
		case 2: strcpy(Filename, Foldername);
				strcat(Filename, Test[bar]);
				strcat(Filename, Extname);
				color='2';
		//printf("%s",Filename);
		break;
		
		case 3:
		color='3';
		break;
	}
}

void SavebtnHandler(int pos)
{
    FILE *fp,*fp1;
	
	fp=fopen(Filename,"w");
	
	fprintf(fp, "%d\n%d\n", h1_slider, h2_slider);
	fprintf(fp, "%d\n%d\n", s1_slider, s2_slider);
	fprintf(fp, "%d\n%d\n", l1_slider, l2_slider);
	
	fclose(fp);
	printf("saved\n");
    cvSetTrackbarPos("Save", "Tuner", 0);
}

void LoadbtnHandler(int pos)
{
	FILE *fp;
	do
	{
		fp=fopen(Filename,"r");
	}while(fp==NULL);
	fscanf(fp,"%d %d %d %d %d %d", &h1_slider, &h2_slider, &s1_slider, &s2_slider, &l1_slider, &l2_slider);
	
	fclose(fp);
	
	printf("Loaded\n");
    
    //cvSetTrackbarPos("File", "Tuner", 0);
    cvSetTrackbarPos("Load", "Tuner", 0);
    
    cvSetTrackbarPos("HUE 1", "Tuner", h1_slider);
    cvSetTrackbarPos("HUE 2", "Tuner", h2_slider);
    cvSetTrackbarPos("SAT 1", "Tuner", s1_slider);
    cvSetTrackbarPos("SAT 2", "Tuner", s2_slider);
    cvSetTrackbarPos("LUM 1", "Tuner", l1_slider);
    cvSetTrackbarPos("LUM 2", "Tuner", l2_slider);
}

int main(int argc, char **argv)
{
	//init ialized component
	UART_Init();
	VISION_InitCam(); //open it when using webcam

	// setup thread identifier
	pthread_t pth_UARTRx, pthFrameGrab;	
	
	//start UARTRx thread
	pthread_create(&pth_UARTRx,NULL,threadUARTRx,"foo");
	
	//pthread_create(&pthFrameGrab,NULL,threadFrameGrab,"foo");
	
	double time_spent;
	
	printf("Setup complete, running main program now\n");

	h1_slider = 0;
	h2_slider = 255;
	s1_slider = 0;
	s2_slider = 255;
	l1_slider = 0;
	l2_slider = 255;
	 
	//to enable GUI for color tuning
	  
	if(argc==2)
	{
	
		VISION_EnableTuneMode();
		cvNamedWindow("Tuner", CV_WINDOW_AUTOSIZE);
		
		cvCreateTrackbar( "File", "Tuner", &bar, 3, TunebtnHandler);
		cvCreateTrackbar( "Save", "Tuner", &slider, 1, SavebtnHandler);
		cvCreateTrackbar( "Load", "Tuner", &slider, 1, LoadbtnHandler);
		
		cvCreateTrackbar( "HUE 1", "Tuner", &h1_slider, slider_max, SavebtnHandler);
		cvCreateTrackbar( "HUE 2", "Tuner", &h2_slider, slider_max, SavebtnHandler);
		cvCreateTrackbar( "SAT 1", "Tuner", &s1_slider, slider_max, SavebtnHandler);
		cvCreateTrackbar( "SAT 2", "Tuner", &s2_slider, slider_max, SavebtnHandler);
		cvCreateTrackbar( "LUM 1", "Tuner", &l1_slider, slider_max, SavebtnHandler);
		cvCreateTrackbar( "LUM 2", "Tuner", &l2_slider, slider_max, SavebtnHandler);
	}

	VisionRange Range_a = {0, 255, 0, 255, 0, 255};
    VisionRange Range_b = {0, 255, 0, 255, 0, 255};
	VisionRange Range_c = {0, 255, 0, 255, 0, 255};	
	
	while(1)//(prog_quit!=-1)
	{
		//vison program to be inserted here
		clock_t begin, end;
		begin = clock();
					
		switch (rx_buffer[0])
		{
			case 'B'://mara
			        key=rx_buffer[0];
		            
		            Readfile("Mara0",0);
		            Readfile("Mara1",1);
		            Readfile("Mara2",2);
		         				
					RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,&color);
					//UART_TX_Char('a');					
				break;
			
			case 'A'://LnC
			        key=rx_buffer[0];
		            
		            Readfile("LnC0",0);
		            Readfile("LnC1",1);
		            Readfile("LnC2",2);
					
					RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,&color);        					
				break;
				
			case 'P'://PK
			        key=rx_buffer[0]; //same as marathon
		            
		            Readfile("PK0",0);
		            Readfile("PK1",1);
		            Readfile("PK2",2);
		         	
					RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,&color);
					break;
					
			case 'D'://OBS
			        key=rx_buffer[0];
		            
		            Readfile("Obs0",0);
		            Readfile("Obs1",1);
		            Readfile("Obs2",2);
		         
					RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,&color);	        
				break;
				
			default:
			        key='D';
		            
		            Readfile("Obs0",0);
		            Readfile("Obs1",1);
		            Readfile("Obs2",2);

					RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,&color);
					//tune(bar);
					//strcpy(Filename, "caseA.conf");
				break;
				
		}
					//UART_TX_Char('d');
			         
		            //strcpy(Gamename,"Red");

		            //Readfile();
		            
		            /*FILE_Read("/home/pi/conf/LnC0.conf", &Range[0]);//Lift and carry
					FILE_Read("/home/pi/conf/LnC1.conf", &Range[1]);
					FILE_Read("/home/pi/conf/LnC2.conf", &Range[2]);*/

		end = clock(); 
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("Time - Frame Grabbing: %.2f fps\r\n", 1/time_spent);
		if ( (cvWaitKey(10) & 255) == 27 ) break;//quit if press 'Esc'
		
	}

	return 0;
}
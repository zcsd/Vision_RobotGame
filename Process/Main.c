//////////////////////////////////////////////////////////////
//															//
//			 VisionSystem for Humanoid Robot				//
//															//
//						  V2.1								//
//															//
//				    For ODROID U3+							//
//															//
//////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "Uart.h"
#include "File.h"
#include "Vision.h"

#include <cv.h>
#include <highgui.h>

int prog_quit = 0;
int z,colorNo;
Color color;
VisionRange Range[3],RangeSD[3],rangeSD,rangeSD1,rangeSD2;
char key,key_Game;
char TuneTest = 0;

char FrameGrabDone = 0;
char GameName[3][50],Path[50]="",Dir[50]="/mnt/rd/",ExtName[10]=".txt",Filename[50]="";
char PathSD[50]="",DirSD[50]="/home/odroid/Vision/Data/";

unsigned char Min[3]= {0};
unsigned char Max[3]= {0};
unsigned char Min1[3]= {0};
unsigned char Max1[3]= {0};
unsigned char Min2[3]= {0};
unsigned char Max2[3]= {0};
unsigned int InvertIni[3]={0};

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

void TX_Char(char x, BlobCoord Blob,BlobCoord Blob1)
{
	UART_TX_Char(13);
	UART_TX_Char(x);

	UART_TX_Char(Blob.Xmin);
	UART_TX_Char(Blob.Xmax);
	UART_TX_Char(Blob.Ymin);
	UART_TX_Char(Blob.Ymax);
	printf("\r\nBlobX: %d\t%d\r\nBlobY: %d\t%d", Blob.Xmin, Blob.Xmax, Blob.Ymin, Blob.Ymax);	
}

void RunVision_ColorSeg_Struct(VisionRange Range,VisionRange Range1,VisionRange Range2,char *key1, Color color)
{
	BlobCoord Blob,Blob1;
	LoadColor(color);
	clock_t begin, end;
	double time_spent;
	begin = clock();

	//if(FrameGrabDone==1);
	if(VISION_GrabFrame())
	{
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("GrabFrame: %.2f fps\t", 1/time_spent);
		//VISION_ShowOriginalFrame();
		
		begin = clock();
		//printf("Range: %d\n",Range.Hmin);
		
		if((key == 'B' || key== 'D' || key == 'W' || key== 'K') && colorNo==0)
		VISION_Tune_Color1(Range,Range1,Range2,&Blob,key1, color);
		
		else if((key== 'D' || key== 'K' ||key == 'B') && colorNo==1)
		VISION_Tune_Color2(Range,Range1,Range2,&Blob,key1, color);
		
		else if((key == 'D' || key == 'B') && colorNo==2)
		VISION_Tune_Color3(Range,Range1,Range2,&Blob,key1, color);
		
		switch(key_Game)
		{
			case 'B'://Fira Mara
//			VISION_Game_ArrowDetect(Range,Range1,Range2,&Blob,key1, color);
			VISION_Game_1Color(Range,Range1,Range2,&Blob,key1, color);
			break;
			
			case 'W':
			VISION_Game_1Color(Range,Range1,Range2,&Blob,key1, color);
			
			break;
			case 'D': //Fira Obs
			VISION_Game_OBS(Range,Range1,Range2,&Blob,&Blob1,key1, color);
			
			break;
			case 'K':
			
			break;
			
		}
		
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("ProcFrame: %.2f fps\t", 1/time_spent);

		switch(key_Game)//*key1)
		{			
			case'B':
			TX_Char(1,Blob,Blob1);
			break;
			
			case'D':
			TX_Char(3,Blob,Blob1);
			TX_Char(4,Blob,Blob1);
			break;
		}

		FrameGrabDone=0;
//		printf("\r\nBlobX: %d\t%d\r\nBlobY: %d\t%d", Blob.Xmin, Blob.Xmax, Blob.Ymin, Blob.Ymax);
	}
}

void Readfile(char* Game, int x)
{
	strcpy(GameName[x],Game);
	strcpy(Path,Dir);
	strcat(Path,Game);
	strcat(Path,ExtName);
	FILE_Read(Path, &Range[x]);
}

void ReadfileGame(char* Game, int x)
{
	strcpy(GameName[x],Game);
	strcpy(Path,Dir);
	strcat(Path,Game);
	strcat(Path,ExtName);
	FILE_ReadGame(Path, &Range[x],x);
}

void ReadColor()
{
	FILE_ReadColor(&color);
}

void ReadWriteColortoRD()
{
	FILE_ReadSDWriteRDColor(&color);
}

void ReadSDWriteRD(char* game)
{
	strcpy(PathSD,DirSD);
	strcat(PathSD,game);
	strcat(PathSD,ExtName);
	FILE_ReadSDWriteRD(PathSD);
}

void WriteGameRD()
{
	FILE *file;
	file=fopen("/mnt/rd/Game.txt","w");
	fprintf(file,"%c",key);
	
	fclose(file);
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

	double time_spent;
	char *Argv= argv[1];
	
	printf("Setup complete, running main program now\n");

	if(argc==2)
	{
		VISION_EnableTuneMode();
		key = *Argv;
		key_Game = 0;
		
		FILE *file;
		file=fopen("/mnt/rd/tuning.txt","w");
		fclose(file);			
	}
	else
	{
		key_Game = 'D';//rx_buffer[0];
		key= 0;
	}
	switch (key)
		{
			case 'B'://mara
			ReadWriteColortoRD();
			WriteGameRD();
			ReadSDWriteRD("ColorM");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");
			//printf("m\r\n");
									
			break;
					
			case 'D': 
			ReadWriteColortoRD();
			WriteGameRD();	
			ReadSDWriteRD("ColorO");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");
			//printf("o\r\n");
													
			break;
				
			case 'W':
			ReadWriteColortoRD();
			WriteGameRD(); 
			ReadSDWriteRD("ColorW");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");	
			//printf("w\r\n");
									
			break;
				
			case 'K':
			ReadWriteColortoRD(); 
			WriteGameRD(); 
			ReadSDWriteRD("ColorB");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");					
			break;
		}
	switch (key_Game)
		{
			case 'B'://mara
			ReadWriteColortoRD();
			WriteGameRD();
			ReadSDWriteRD("ColorM");
			//system("/home/odroid/QT-VS/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");
			//printf("m\r\n");
									
			break;
					
			case 'D': 
			ReadWriteColortoRD();
			WriteGameRD();	
			ReadSDWriteRD("ColorO");
			//system("/home/odroid/QT-VS/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");
			//printf("o\r\n");
													
			break;
				
			case 'W':
			ReadWriteColortoRD();
			WriteGameRD(); 
			ReadSDWriteRD("ColorW");
			//system("/home/odroid/QT-VS/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");	
			//printf("w\r\n");
									
			break;
				
			case 'K':
			ReadWriteColortoRD(); 
			WriteGameRD(); 
			ReadSDWriteRD("ColorB");
			//system("/home/odroid/QT-VS/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");					
			break;
		}	
	while(1)
	{	
		clock_t begin, end;
		begin = clock();		

		switch (key)
		{
			case 'B'://mara
			ReadColor();
			Readfile("tuning",0);
			Readfile("tuning",1);
			Readfile("tuning",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
			break;
					
			case 'D': 
			ReadColor();
			Readfile("tuning",0);
			Readfile("tuning",1);
			Readfile("tuning",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'W':
			ReadColor();
			Readfile("tuning",0);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'K': 
			ReadColor();
			Readfile("tuning",0);
			Readfile("tuning",1);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
		}			
		switch (key_Game)
		{
			case 'B'://mara
			//ReadColor();
			ReadfileGame("Color",0);
			ReadfileGame("Color",1);
			ReadfileGame("Color",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
			break;
					
			case 'D': 
			//ReadColor();
			ReadfileGame("Color",0);
			ReadfileGame("Color",1);
			ReadfileGame("Color",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'W':
			//ReadColor();
			Readfile("Color",0);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'K': 
			//ReadColor();
			Readfile("Color",0);
			Readfile("Color",1);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
		}
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//		printf("Time - Frame Grabbing: %.2f fps\r\n", 1/time_spent);
		if ( (cvWaitKey(10) & 255) == 27 ) break;//quit if press 'Esc'
	}	
	

	return 0;
}

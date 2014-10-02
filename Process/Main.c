//////////////////////////////////////////////////////////////
//															//
//			 VisionSystem for Humanoid Robot				//
//															//
//						  V2.4								//
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
char key=0,key_Game=0;
char TuneTest = 0, LOOP=0;

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
	if( x == 1 || x == 10 || x == 12)//x=1(Mara) or x=10(BSK ball) or x=12(OBS floor)
	{ 
		UART_TX_Char(13);
		UART_TX_Char(x);
		UART_TX_Char(Blob.Xmin);
		UART_TX_Char(Blob.Xmax);
		UART_TX_Char(Blob.Ymin);
		UART_TX_Char(Blob.Ymax);
		printf("\nBlob X: %d %d , Blob Y: %d %d\n", Blob.Xmin, Blob.Xmax, Blob.Ymin, Blob.Ymax);
	}
	else // x = 11(BSK Basket) or x = 13(OBS red folder)
	{
		UART_TX_Char(13);
		UART_TX_Char(x);
		UART_TX_Char(Blob1.Xmin);
		UART_TX_Char(Blob1.Xmax);
		UART_TX_Char(Blob1.Ymin);
		UART_TX_Char(Blob1.Ymax);
		printf("Blob1 X: %d %d , Blob1 Y: %d %d\n", Blob1.Xmin, Blob1.Xmax, Blob1.Ymin, Blob1.Ymax);
	}
			
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
			VISION_Game_ArrowDetect(Range,Range1,Range2,&Blob,key1, color);
//			VISION_Game_1Color(Range,Range1,Range2,&Blob,key1, color);
			break;
			
			case 'W'://FIRA WNL
			VISION_Game_1Color(Range,Range1,Range2,&Blob,key1, color);
			break;
			
			case 'D': //Fira OBS
			VISION_Game_OBS(Range,Range1,Range2,&Blob,&Blob1,key1, color);
			break;
			
			case 'K': //FIRA BSK
			VISION_Game_BSK(Range,Range1,Range2,&Blob,&Blob1,key1, color);
			break;
			
		}
		
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("ProcFrame: %.2f fps\t", 1/time_spent);

		switch(key_Game)//*key1)
		{			
			case'B': // Send Mara Blob(Line and Arrow)
			TX_Char(1,Blob,Blob1);
			break;
			
			case'D': // Send OBS Blob(Floor) and Blob1(Red folder)
			TX_Char(12,Blob,Blob1); // Floor
			TX_Char(13,Blob,Blob1); // Red Folder
			break;
			
			case'K': // Send BSK Bolb(Ball) and Blob1(Basket)
			TX_Char(10,Blob,Blob1);//Ball
			TX_Char(11,Blob,Blob1);//Basket
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
    
	if(argc==2) //Tuning Mode
	{
		VISION_EnableTuneMode();
		key = *Argv;
		
		FILE *file;
		file=fopen("/mnt/rd/tuning.txt","w");
		fclose(file);			
	}
 
	switch (key) //Tuning Mode
		{
			case 'B': //FIRA Mara
			ReadWriteColortoRD();
			WriteGameRD();
			ReadSDWriteRD("ColorM");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");
			//printf("m\r\n");
									
			break;
					
			case 'D': //FIRA OBS
			ReadWriteColortoRD();
			WriteGameRD();	
			ReadSDWriteRD("ColorO");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");
			//printf("o\r\n");
													
			break;
				
			case 'W': // FIRA WNL
			ReadWriteColortoRD();
			WriteGameRD(); 
			ReadSDWriteRD("ColorW");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");	
			//printf("w\r\n");
									
			break;
				
			case 'K': //FIRA BSK
			ReadWriteColortoRD(); 
			WriteGameRD(); 
			ReadSDWriteRD("ColorB");
			system("/home/odroid/Vision/Tuner/Tuner &");
			//wait(100);
			//system("./Tuner");					
			break;
		}
		
		if(key==0 || key=='b' || key=='d' || key=='w' || key=='k' )
		{
			if(key==0)
				key_Game = 'b';
			else
				key_Game = key;
			
			printf("Default Game is %c\n",key_Game);
			
			switch (key_Game) // Game Mode
			{
				case 'b'://FIRA Mara
				key_Game='B';
				ReadWriteColortoRD();
				WriteGameRD();
				ReadSDWriteRD("ColorM");
				//system("/home/odroid/QT-VS/Tuner/Tuner &");
				//wait(100);
				//system("./Tuner");
				//printf("m\r\n");
										
				break;
						
				case 'd': //FIRA OBS
				key_Game='D';
				ReadWriteColortoRD();
				WriteGameRD();	
				ReadSDWriteRD("ColorO");
				//system("/home/odroid/QT-VS/Tuner/Tuner &");
				//wait(100);
				//system("./Tuner");
				//printf("o\r\n");
														
				break;
					
				case 'w': //FIRA WNL
				key_Game='W';
				ReadWriteColortoRD();
				WriteGameRD(); 
				ReadSDWriteRD("ColorW");
				//system("/home/odroid/QT-VS/Tuner/Tuner &");
				//wait(100);
				//system("./Tuner");	
				//printf("w\r\n");
										
				break;
					
				case 'k': //FIRA BSK
				key_Game='K';
				ReadWriteColortoRD(); 
				WriteGameRD(); 
				ReadSDWriteRD("ColorB");
				//system("/home/odroid/QT-VS/Tuner/Tuner &");
				//wait(100);
				//system("./Tuner");					
				break;
			}
		}		

	while(1)
	{	
		clock_t begin, end;
		begin = clock();
			
		if( LOOP == 0 )
		{
			if(key == 0 && ( rx_buffer[0] == 'B' || rx_buffer[0] == 'D' || rx_buffer[0] == 'K') )
//			if(key == 0 && rx_buffer[0] != '\0') 
			{
				key_Game = rx_buffer[0];
				printf("key_Game is %c\n",key_Game);
				
				switch (key_Game) // Game Mode
				{
					case 'B'://FIRA Mara
					ReadWriteColortoRD();
					WriteGameRD();
					ReadSDWriteRD("ColorM");
					//system("/home/odroid/QT-VS/Tuner/Tuner &");
					//wait(100);
					//system("./Tuner");
					//printf("m\r\n");
											
					break;
							
					case 'D': //FIRA OBS
					ReadWriteColortoRD();
					WriteGameRD();	
					ReadSDWriteRD("ColorO");
					//system("/home/odroid/QT-VS/Tuner/Tuner &");
					//wait(100);
					//system("./Tuner");
					//printf("o\r\n");
															
					break;
						
					case 'W': //FIRA WNL
					ReadWriteColortoRD();
					WriteGameRD(); 
					ReadSDWriteRD("ColorW");
					//system("/home/odroid/QT-VS/Tuner/Tuner &");
					//wait(100);
					//system("./Tuner");	
					//printf("w\r\n");
											
					break;
						
					case 'K': //FIRA BSK
					ReadWriteColortoRD(); 
					WriteGameRD(); 
					ReadSDWriteRD("ColorB");
					//system("/home/odroid/QT-VS/Tuner/Tuner &");
					//wait(100);
					//system("./Tuner");					
					break;
				}
				LOOP = 1;
			}
		}	
		
		switch (key) //Tuning Mode
		{
			case 'B'://FIRA Mara
			ReadColor();
			Readfile("tuning",0);
			Readfile("tuning",1);
			Readfile("tuning",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
			break;
					
			case 'D': //FIRA OBS
			ReadColor();
			Readfile("tuning",0);
			Readfile("tuning",1);
			Readfile("tuning",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'W': // FIRA WNL
			ReadColor();
			Readfile("tuning",0);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'K':  //FIRA BSK
			ReadColor();
			Readfile("tuning",0);
			Readfile("tuning",1);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
		}			
		switch (key_Game) //Game Mode
		{
			case 'B'://FIRA Mara
			//ReadColor();
			ReadfileGame("Color",0);
			ReadfileGame("Color",1);
			ReadfileGame("Color",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
			break;
					
			case 'D': //FIRA OBS
			//ReadColor();
			ReadfileGame("Color",0);
			ReadfileGame("Color",1);
			ReadfileGame("Color",2);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'W': //FIRA WNL
			//ReadColor();
			ReadfileGame("Color",0);
			//printf("color=%d",color);
			RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
								
			break;
				
			case 'K': //FIRA BSK
			//ReadColor();
			ReadfileGame("Color",0);
			ReadfileGame("Color",1);
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

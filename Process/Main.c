//////////////////////////////////////////////////////////////
//															//
//			 VisionSystem for Humanoid Robot				//
//															//
//						  V2.5c								//
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

//b for Marathon, d for Obsticle, k for Basketball, w for Weight and lifting
#define DEFAULT_GAME 'b'

int prog_quit=0;
int colorNo;

char key=0,key_Game=0;// pressed key, receive key_Game buffer
char RX_DONE=0, READ_DONE=0;//rx_buffer received(1) or not received(0)
char GameName[3][50],Path[50]="",Dir[50]="/mnt/rd/",ExtName[10]=".txt",Filename[50]="";
char PathSD[50]="",DirSD[50]="/home/odroid/Vision/Data/";

unsigned char Min[3]= {0};
unsigned char Max[3]= {0};
unsigned char Min1[3]= {0};
unsigned char Max1[3]= {0};
unsigned char Min2[3]= {0};
unsigned char Max2[3]= {0};

Color color;
VisionRange Range[3],RangeSD[3],rangeSD,rangeSD1,rangeSD2;

void *threadUARTRx(void *arg)
{
	while(prog_quit!=-1)
	{
		UART_RX();
		if(rx_buffer[0] == 'q')
			prog_quit=-1;
	}
//	printf("UART has quit\r\n");
	return NULL;
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

void TX_Char(char object, BlobCoord Blob,BlobCoord Blob1)
{
	if( object == 1 || object == 10 || object == 12)//object=1(Mara) or object=10(BSK ball) or object=12(OBS floor)
	{ 
		UART_TX_Char(13);
		UART_TX_Char(object);
		UART_TX_Char(Blob.Xmin);
		UART_TX_Char(Blob.Xmax);
		UART_TX_Char(Blob.Ymin);
		UART_TX_Char(Blob.Ymax);
		printf("\n%d Blob X: %d %d , Blob Y: %d %d\n",object, Blob.Xmin, Blob.Xmax, Blob.Ymin, Blob.Ymax);
	}
	
	if( object == 11 || object == 13 ) // object = 11(BSK Basket) or object = 13(OBS red folder)
	{
		UART_TX_Char(13);
		UART_TX_Char(object);
		UART_TX_Char(Blob1.Xmin);
		UART_TX_Char(Blob1.Xmax);
		UART_TX_Char(Blob1.Ymin);
		UART_TX_Char(Blob1.Ymax);
		printf("%d Blob1 X: %d %d , Blob1 Y: %d %d\n",object, Blob1.Xmin, Blob1.Xmax, Blob1.Ymin, Blob1.Ymax);
	}		
}

void RunVision_ColorSeg_Struct(VisionRange Range,VisionRange Range1,VisionRange Range2,char *key1, Color color)
{
	BlobCoord Blob,Blob1;
	LoadColor(color);

	if(VISION_GrabFrame())
	{
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
			TX_Char(1,Blob,Blob1);//Line and Arrow
			break;

			case 'D': //Fira OBS
			VISION_Game_OBS(Range,Range1,Range2,&Blob,&Blob1,key1, color);
			TX_Char(12,Blob,Blob1); // Floor
			TX_Char(13,Blob,Blob1); // Red Folder
			break;
			
			case 'K': //FIRA BSK
			VISION_Game_BSK(Range,Range1,Range2,&Blob,&Blob1,key1, color);
			TX_Char(10,Blob,Blob1);//Ball
			TX_Char(11,Blob,Blob1);//Basket
			break;	
			
			case 'W'://FIRA WNL
			VISION_Game_1Color(Range,Range1,Range2,&Blob,key1, color);
			TX_Char(1,Blob,Blob1);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	//init ialized component
	UART_Init();
	VISION_InitCam(); 
	
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
		break;
				
		case 'D': //FIRA OBS
		ReadWriteColortoRD();
		WriteGameRD();	
		ReadSDWriteRD("ColorO");
		system("/home/odroid/Vision/Tuner/Tuner &");										
		break;
			
		case 'W': // FIRA WNL
		ReadWriteColortoRD();
		WriteGameRD(); 
		ReadSDWriteRD("ColorW");
		system("/home/odroid/Vision/Tuner/Tuner &");					
		break;
			
		case 'K': //FIRA BSK
		ReadWriteColortoRD(); 
		WriteGameRD(); 
		ReadSDWriteRD("ColorB");
		system("/home/odroid/Vision/Tuner/Tuner &");					
		break;
	}
		
	if(key==0 || key=='b' || key=='d' || key=='w' || key=='k' )
	{
		if(key==0)
			key_Game = DEFAULT_GAME;
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
			break;
					
			case 'd': //FIRA OBS
			key_Game='D';
			ReadWriteColortoRD();
			WriteGameRD();	
			ReadSDWriteRD("ColorO");									
			break;
				
			case 'w': //FIRA WNL
			key_Game='W';
			ReadWriteColortoRD();
			WriteGameRD(); 
			ReadSDWriteRD("ColorW");					
			break;
				
			case 'k': //FIRA BSK
			key_Game='K';
			ReadWriteColortoRD(); 
			WriteGameRD(); 
			ReadSDWriteRD("ColorB");				
			break;
		}
	}		

	while(1)
	{	
		clock_t begin, end;
		begin = clock();
			
		if( RX_DONE == 0 )
		{
			if(key == 0 && ( rx_buffer[0] == 'B' || rx_buffer[0] == 'D' || rx_buffer[0] == 'K' || rx_buffer[0] == 'W') )// rx_buffer[0] != '\0'
			{
				key_Game = rx_buffer[0];
				printf("key_Game is %c\n",key_Game);
				
				switch (key_Game) // Game Mode
				{
					case 'B'://FIRA Mara
					ReadWriteColortoRD();
					WriteGameRD();
					ReadSDWriteRD("ColorM");						
					break;
							
					case 'D': //FIRA OBS
					ReadWriteColortoRD();
					WriteGameRD();	
					ReadSDWriteRD("ColorO");										
					break;
						
					case 'W': //FIRA WNL
					ReadWriteColortoRD();
					WriteGameRD(); 
					ReadSDWriteRD("ColorW");						
					break;
						
					case 'K': //FIRA BSK
					ReadWriteColortoRD(); 
					WriteGameRD(); 
					ReadSDWriteRD("ColorB");				
					break;
				}
				RX_DONE = 1;
				READ_DONE = 0;
			}
		}	
		
		if(key != 0)
		{
			switch (key) //Tuning Mode
			{
				case 'B'://FIRA Mara
				ReadColor();
				Readfile("tuning",0);
				Readfile("tuning",1);
				Readfile("tuning",2);
				RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
				break;
						
				case 'D': //FIRA OBS
				ReadColor();
				Readfile("tuning",0);
				Readfile("tuning",1);
				Readfile("tuning",2);
				RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);					
				break;
					
				case 'W': // FIRA WNL
				ReadColor();
				Readfile("tuning",0);
				RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);				
				break;
					
				case 'K':  //FIRA BSK
				ReadColor();
				Readfile("tuning",0);
				Readfile("tuning",1);
				RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);				
				break;
			}	
		}
		
		if( ( key==0 || key=='b' || key=='d' || key=='w' || key=='k') && READ_DONE==0)
		{		
			switch (key_Game) //Game Mode
			{
				case 'B'://FIRA Mara
				ReadfileGame("Color",0);
				ReadfileGame("Color",1);
				ReadfileGame("Color",2);
				break;
						
				case 'D': //FIRA OBS
				ReadfileGame("Color",0);
				ReadfileGame("Color",1);
				ReadfileGame("Color",2);				
				break;
					
				case 'W': //FIRA WNL
				ReadfileGame("Color",0);				
				break;
					
				case 'K': //FIRA BSK
				ReadfileGame("Color",0);
				ReadfileGame("Color",1);			
				break;
			}
			READ_DONE=1;
		}
		
		RunVision_ColorSeg_Struct(Range[0],Range[1],Range[2],&key,color);
		
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//		printf("Time - Frame Grabbing: %.2f fps\r\n", 1/time_spent);
		
		if ( (cvWaitKey(10) & 255) == 27 ) break; //quit if press 'Esc'
	}	
	
	return 0;
}

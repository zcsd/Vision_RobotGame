#include <stdio.h>
#include "File.h"

int FILE_Read(char Filename[50], VisionRange *temp)
{
	FILE *fp;
	
	fp=fopen(Filename,"r");
	
	//fp=fopen("/home/pi/Raspi/VisionSystem2/caseA.conf","r");
	if(fp)
		fscanf(fp,"%d %d %d %d %d %d", &temp->Hmin, &temp->Hmax, &temp->Smin, &temp->Smax, &temp->Lmin, &temp->Lmax);
	
	//printf("fileread: %d %d %d %d %d %d\r\n", temp->Hmin, temp->Hmax, temp->Smin, temp->Smax, temp->Lmin, temp->Lmax);
	
	fclose(fp);
	
	return(1);
}
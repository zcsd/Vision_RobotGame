#include <stdio.h>
#include "File.h"

int FILE_Read(char Filename[50], VisionRange *temp)
{
	/*FILE *fp;

	fp=fopen(Filename,"r");

	//fp=fopen("/mnt/rd/Mara.conf","r");
	if(fp)
		fscanf(fp,"%d %d %d %d %d %d %d", &temp->invert, &temp->Hmin, &temp->Hmax, &temp->Smin, &temp->Smax, &temp->Lmin, &temp->Lmax);

	//printf("fileread: %d %d %d %d %d %d %d\r\n", temp->Hmin, temp->Hmax, temp->Smin, temp->Smax, temp->Lmin, temp->Lmax, temp->invert);

	fclose(fp);*/
	int set;
	Color color;
	FILE_ReadColor(&color);
	char xx[5];
	int ii,tvalue[21];
	FILE *fp;
	fp=fopen(Filename,"r");         
	
	for(ii=0;ii<21;ii++)
	{
		fgets(xx,sizeof(xx),fp);
		tvalue[ii]=atoi(xx);
		//printf("%d\n",tvalue[ii]);		
	}
	fclose(fp);
	
	switch(color.set)
	{
		case 0:
		{
			(temp->invert) = tvalue[0];
			(temp->Hmin) = tvalue[1];
			(temp->Hmax) = tvalue[2];
			(temp->Smin) = tvalue[3];
			(temp->Smax) = tvalue[4];
			(temp->Lmin) = tvalue[5];
			(temp->Lmax) = tvalue[6];
		}
		break;
		case 1:
		{
			(temp->invert) = tvalue[7];
			(temp->Hmin) = tvalue[8];
			(temp->Hmax) = tvalue[9];
			(temp->Smin) = tvalue[10];
			(temp->Smax) = tvalue[11];
			(temp->Lmin) = tvalue[12];
			(temp->Lmax) = tvalue[13];
		}
		break;
		case 2:
		{
			(temp->invert) = tvalue[14];
			(temp->Hmin) = tvalue[15];
			(temp->Hmax) = tvalue[16];
			(temp->Smin) = tvalue[17];
			(temp->Smax) = tvalue[18];
			(temp->Lmin) = tvalue[19];
			(temp->Lmax) = tvalue[20];
		}
		break;
	}
	return(1);
}

int FILE_ReadGame(char Filename[50], VisionRange *temp, int x)
{
	char xx[5];
	int ii,tvalue[21];
	FILE *fp;
	fp=fopen(Filename,"r");         
	
	for(ii=0;ii<21;ii++)
	{
		fgets(xx,sizeof(xx),fp);
		tvalue[ii]=atoi(xx);
		//printf("%d\n",tvalue[ii]);		
	}
	fclose(fp);
	
	switch(x)
	{
		case 0:
		{
			(temp->invert) = tvalue[0];
			(temp->Hmin) = tvalue[1];
			(temp->Hmax) = tvalue[2];
			(temp->Smin) = tvalue[3];
			(temp->Smax) = tvalue[4];
			(temp->Lmin) = tvalue[5];
			(temp->Lmax) = tvalue[6];
		}
		break;
		case 1:
		{
			(temp->invert) = tvalue[7];
			(temp->Hmin) = tvalue[8];
			(temp->Hmax) = tvalue[9];
			(temp->Smin) = tvalue[10];
			(temp->Smax) = tvalue[11];
			(temp->Lmin) = tvalue[12];
			(temp->Lmax) = tvalue[13];
		}
		break;
		case 2:
		{
			(temp->invert) = tvalue[14];
			(temp->Hmin) = tvalue[15];
			(temp->Hmax) = tvalue[16];
			(temp->Smin) = tvalue[17];
			(temp->Smax) = tvalue[18];
			(temp->Lmin) = tvalue[19];
			(temp->Lmax) = tvalue[20];
		}
		break;
	}
	return(1);
}

int FILE_ReadColor(Color *colortemp)
{
	FILE *file;
	file=fopen("/mnt/rd/colorNo.txt","r");
	if(file)
		fscanf(file,"%d",&colortemp->set);
		//printf("read: %d",colortemp);
		fclose(file);
		
	return (1);
}

int FILE_ReadSDWriteRDColor(Color *Ctemp)
{
	FILE *file;
	file=fopen("/home/odroid/Vision/Data/colorNo.txt","r");
	if(file)
		fscanf(file,"%d",&Ctemp->set);
		//printf("read: %d",colortemp);
		fclose(file);
		
	FILE *f;
    f=fopen("/mnt/rd/colorNo.txt","w");
    if(f)
    fprintf(f,"%d",Ctemp->set);
    fclose(f);
		
	return (1);
}

int FILE_ReadSDWriteRD(char Filename[50])
{
	
	char xx[5];
	int ii,value[21];
	FILE *fp;
	fp=fopen(Filename,"r");         
	
	for(ii=0;ii<21;ii++)
	{
		fgets(xx,sizeof(xx),fp);
		value[ii]=atoi(xx);
		//printf("%d\n",value[ii]);
		
	}
	fclose(fp);	
	
	FILE *f;
	f=fopen("/mnt/rd/Color.txt","w");
	fprintf(f,"%d\n%d\n%d\n",value[0],value[1],value[2]);
	fprintf(f,"%d\n%d\n",value[3],value[4]);
	fprintf(f,"%d\n%d\n",value[5],value[6]);
	fprintf(f,"%d\n%d\n%d\n",value[7],value[8],value[9]);
	fprintf(f,"%d\n%d\n",value[10],value[11]);
	fprintf(f,"%d\n%d\n",value[12],value[13]);
	fprintf(f,"%d\n%d\n%d\n",value[14],value[15],value[16]);
	fprintf(f,"%d\n%d\n",value[17],value[18]);
	fprintf(f,"%d\n%d\n",value[19],value[20]);
	fclose(f);		
	return(1);
}



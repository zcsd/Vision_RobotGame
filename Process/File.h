#ifndef __FILE_H
#define __FILE_H

#include "Vision.h"

int FILE_Read(char Filename[30], VisionRange *temp);
int FILE_ReadColor(Color *colortemp);
int FILE_ReadSD(char Filename[50], VisionRange *temp);
#endif

/*
 * monitor.c
 *
 *  Created on: Sep 1, 2017
 *      Author: paramra
 */
#include "monitor.h"
#include <stdlib.h>

static char tmp[32];

int monitor(const char *cmd)
{
	uint16_t color = 0;
	char *p = strtok(cmd, " ");
	if(p == 0){
		return -1;
	}

	if(!strcmp(p, "color")){
		if((p = strtok(NULL, " ")) == NULL){
			return -2;
		}
		color = atoi(p);
		st7735FillRect(90, 110, 20, 20, color);
	}
	else{
		return -1;
	}

	return 0;
}


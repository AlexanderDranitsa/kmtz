
#ifndef LIDAR_H_
#define LIDAR_H_

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define INIT_TIME   3

#define PI 		  	3.1415
#define PWM_CONST 	4.8333	// equal to 1 degree of servo's rotation

int UART_init(int*);
void send_command(char*, int);
void parse_and_send_command(char*, int);

#endif

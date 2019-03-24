#include "lidar.h"


	int PAUSE = 10000;
//-------------------------------------------------------------------------------------------------------commands:

	char MS_00[]={"MS00\n"};
	char MS_01[]={"MS01\n"};
	char MS_02[]={"MS02\n"};
	char MS_03[]={"MS03\n"};
	char MS_04[]={"MS04\n"};
	char MS_05[]={"MS05\n"};
	char MS_06[]={"MS06\n"};
	char MS_07[]={"MS07\n"};
	char MS_08[]={"MS08\n"};
	char MS_09[]={"MS09\n"};
	char MS_10[]={"MS10\n"};

	char DS[]={"DS\n"};
	char DX[]={"DX\n"};
	char MZ[]={"MZ\n"};
	char MI[]={"MI\n"};
	char LI[]={"LI\n"};
	char IV[]={"IV\n"};
	char ID[]={"ID\n"};
	char RR[]={"RR\n"};

	char LR_01[]={"LR01\n"};
	char LR_02[]={"LR02\n"};
	char LR_03[]={"LR03\n"};

//----------------------------------------------------------------------------------------------------------------

int UART_init(int* x)
{
	*x = open( "/dev/ttyPS1", O_RDWR | O_NOCTTY );				// open port
		if( *x == -1 )
			{ printf( "Error while opening ttyPS1\n" ); }
		else
			printf( "------> ttyPS1 opened successfully\n" );

	struct termios options;										// struct to set options
		tcgetattr( *x, &options );
		cfsetispeed( &options, B115200 ); 						// set input speed
		cfsetospeed( &options, B115200 ); 						// set output speed

																//options for:
		options.c_cflag &= ~( PARENB | CSTOPB | CSIZE );		// parity, stop bit, message size
		options.c_cflag |= ( CLOCAL | CREAD | CS8 );			// receiver, set csize
		options.c_oflag &= ~( OPOST );							// post processing of output
		options.c_iflag &= ~( INLCR | IGNCR | ICRNL | IGNBRK ); // break, CR, NL
		options.c_iflag &= ~( IXON | IXOFF | IXANY ); 			// flow control
		options.c_lflag &= ~( ICANON | ECHO | ECHOE | ECHOK		// canonical mode, echo, signals
									 | ECHONL | ISIG );

		tcsetattr( *x, TCSANOW, &options ); 					// save options
		return 0;
}

void send_command(char* XX, int fd)
{
	int b_read; 												// number of bytes read
	char answer[32]; 										

								 	 	 	 	 	 	 		// check the command to send right number of bytes
	if (XX[0] == 'D' && XX[1] == 'X')
	{
		do
		{
			write (fd, XX, 3);
			b_read = read(fd, answer, (sizeof(answer)-1));		// get answer
			usleep(10000);
		}
		while ((answer[2] !='0') && (answer[3] != '0'));		// repeat until device give 'ok' answer	

		answer[b_read] = '\0'; 									// for string format
		printf("Read %d: %s", b_read, answer);
	}

	else if( (XX[0] == 'M' && XX[1] == 'S')    ||
			 (XX[0] == 'L' && XX[1] == 'R') )					// MS, LR commands are 5 bytes long
	{
		write(fd, XX, 5);
		b_read = read(fd, answer, (sizeof(answer)-1));
		answer[b_read] = 0;
		printf("Read %d: %s", b_read, answer);
	}

	else														// other commands are 3 bytes long
	{
		write(fd, XX, 3);
		b_read = read(fd, answer, (sizeof(answer)-1));
		answer[b_read] = 0;
		printf("Read %d: %s", b_read, answer);
	}

}

void parse_and_send_command(char* command, int fd)
{
		if (command[0]=='D')
		{
			if (command[1]=='S')
			{
				char rotate_command[] =
						"/media/scripts/start_pwm0.sh  560";	// 560 ~ equal to zero position

				int rotate_step = 0;							// rotate_step = degrees between planes
				int plane_points = 0;							// number of points per plane

				int points_now = 0;								// counter for number of points we already got
				int collisions = 0;								// counter for UART collisions
				char change = 0;								// for single byte
				int distance;									// distance, cm
				int lidar_angle_int;							// lidar angle, degrees
				float lidar_angle_rad;							// lidar angle, radians

				float x_coord; 									// for horizontal
				float y_coord; 									// for depth
				float z_coord; 									// for vertical

				FILE *fp;

				system(rotate_command);							// set zero position
				if (strlen(command) > 3){
					int size = 0;
					int i = 2;

					if (command[i] == '_'){
						size = 0;
						i++;
						while (command[i] != '_'){
							size++;
							i++;
						}
						char temp_rotate_step[size + 1];
						for (int i = 0; i < size; i++){
							temp_rotate_step[i] = command[3 + i];
						}
						temp_rotate_step[size + 1] = '\0';
						rotate_step = atoi(temp_rotate_step);
						printf("%d\n", rotate_step);
					}

					int points_start = 0;
					if (command[i] == '_'){
						size = 0;
						i++;
						points_start = i;
						while (command[i] != '\0'){
							size++;
							i++;
						}
					}

					char temp_plane_points[size + 1];
					for (int i = 0; i < size; i++){
						temp_plane_points[i] = command[points_start + i];
					}
					temp_plane_points[size + 1] = '\0';
					plane_points = atoi(temp_plane_points);
					printf("%d\n", plane_points);
				}
				if (!(rotate_step && plane_points)) {
					do {
							printf("set rotate step and #points per plane\n");
							scanf("%d", &rotate_step);
							scanf("%d", &plane_points);
						}
			    	while( (plane_points <= 0) ||
			    		   (rotate_step <= 0)  ||
					   	   (360 % rotate_step != 0) );				// check values
				};
				int steps = 360 / rotate_step; 				
				if (steps < 36) PAUSE=100000;
				if (steps < 18) PAUSE=1000000;
				printf("%d\n", steps);

				float KMTZ_step_rad = (rotate_step)*(PI/180);   // angle between planes, in radians
									  	  	  	  	  	  	  	// KMTZ_step means degree to rotate all complex
				send_command(DS, fd);
				printf("asdadsad\n");
				fp = fopen("cloud.ply", "w");					// make file with '.ply' header
				fprintf(fp,
							"ply\n"
							"format ascii 1.0\n"
							"comment VCGLIB generated\n"
							"element vertex %d\n"
							"property float x\n"
							"property float y\n"
							"property float z\n"
							"property uchar red\n"
							"property uchar green\n"
							"property uchar blue\n"
							"end_header\n",
							(steps * plane_points + 1500));


					for(int step_now = 0; step_now < steps; step_now++)
					{
						points_now = 0;

						while (points_now < plane_points)
						{
							char data[7]; 															
							int chsum = 0;
							read(fd, data, 7);
							chsum = (data[0] + data[1] + data[2] + data[3] + data[4] + data[5]) % 255;	// count checksum..
								if (chsum == data[6])													// ..and compare
								{																		// if OK -> count data
									distance = (data[4] << 8) + (data[3]);
									lidar_angle_int = (data[2] << 8) + (data[1]);
									lidar_angle_rad = (lidar_angle_int / 16) * (PI/180);

									x_coord = cos(KMTZ_step_rad * step_now) * sin(lidar_angle_rad) * distance;
									y_coord = sin(KMTZ_step_rad * step_now) * sin(lidar_angle_rad) * distance;
									z_coord = cos(lidar_angle_rad) * distance;
									fprintf(fp, "%f %f %f 20 200 20\n", x_coord, y_coord, z_coord);
								}
								else																	// else search for the right message
								{
									do
									{
										data[0] = data[1];
										data[1] = data[2];
										data[2] = data[3];
										data[3] = data[4];
										data[4] = data[5];
										data[5] = data[6];

										read(fd, &change, 1);
										data[6] = change;
										chsum = (data[0] + data[1] + data[2] + data[3] + data[4] + data[5]) % 255;
									}
									while(chsum != data[6]);

									distance = (data[4] << 8) + (data[3]);
									lidar_angle_int = (data[2] << 8) + (data[1]);
									lidar_angle_rad = (lidar_angle_int / 16) * (PI/180);
									x_coord = cos(KMTZ_step_rad * step_now) * sin(lidar_angle_rad) * distance;
									y_coord = sin(KMTZ_step_rad * step_now) * sin(lidar_angle_rad) * distance;
									z_coord = cos(lidar_angle_rad) * distance;
									fprintf(fp, "%f %f %f 20 200 20\n", x_coord, y_coord, z_coord);

									collisions++;
									printf("COLLISION!\n");
								}
							points_now++;
						}
																										// rotate all complex each step
						int PWM_val = (step_now + 1) * (rotate_step * PWM_CONST) + 560;
						sprintf(rotate_command, "/media/scripts/start_pwm0.sh %d", PWM_val);

						system(rotate_command);
						printf("%s\n", rotate_command);
						usleep(PAUSE);


					}

					for(int i = 0; i < 500; i++) {
						x_coord = 0;
						y_coord = i;
						z_coord = 0;
						fprintf(fp, "%f %f %f 231 242 0\n", x_coord, y_coord, z_coord); //yellow
					}

					for(int i = 0; i < 500; i++) {
						x_coord = i;
						y_coord = 0;
						z_coord = 0;
						fprintf(fp, "%f %f %f 231 0 0\n", x_coord, y_coord, z_coord);  //red
											}

					for(int i = 0; i < 500; i++) {
						x_coord = 0;
						y_coord = 0;
						z_coord = i;
						fprintf(fp, "%f %f %f 0 0 0\n", x_coord, y_coord, z_coord);   //black
					}


					fclose(fp);


						printf("collisions: %d\n", collisions );

						int return_counter = 2300;
						while (return_counter > (560 + PWM_CONST))										// slowly return to zero position
						{
							sprintf(rotate_command, "/media/scripts/start_pwm0.sh %d", return_counter);
							system(rotate_command);
							return_counter -= (10 * PWM_CONST);
							usleep(100000);
						}
						sprintf(rotate_command, "/media/scripts/start_pwm0.sh %d", 560);
						system(rotate_command);
						usleep(100000);

						send_command(DX, fd);
																										// continue to check for other commands
			}
			else if (command[1]=='X')
			{
				send_command(DX, fd);
			}

			else
			{
				printf("Wrong D'?' command\n");
			}
		}


		else if (command[0]=='M')
		{
			if (command[1]=='Z')
			{
				send_command(MZ, fd);
			}
			else if (command[1]=='I')
			{
				send_command(MI, fd);
			}
			else if (command[1]=='S')
			{
				if (command[3]=='1' && command[4]=='0')
				{
					send_command(MS_10, fd);
				}
				else if (command[3] == '0')
				{
					switch(command[4])
					{
						case '0':send_command(MS_00, fd); break;
						case '1':send_command(MS_01, fd); break;
						case '2':send_command(MS_02, fd); break;
						case '3':send_command(MS_03, fd); break;
						case '4':send_command(MS_04, fd); break;
						case '5':send_command(MS_05, fd); break;
						case '6':send_command(MS_06, fd); break;
						case '7':send_command(MS_07, fd); break;
						case '8':send_command(MS_08, fd); break;
						case '9':send_command(MS_09, fd); break;
						default: printf("Wrong MS command\n");
					}
				}
				else
				{
					printf("Wrong MS command\n");
				}
			}

			else
			{
				printf("Wrong M'?' command\n");
			}
		}


		else if (command[0]=='L')
		{
			if (command[1]=='R')
			{
				if (command[3] == '0' && command[4] == '1')
				{
					send_command(LR_01, fd);
				}
				else if (command[3] == '0' && command[4] == '2')
				{
					send_command(LR_02, fd);
				}
				else if (command[3] == '0' && command[4] == '3')
				{
					send_command(LR_03, fd);
				}
			}
			else if (command[1]=='I')
			{
				send_command(LI, fd);
			}
			else
			{
				printf("Wrong L'?' command\n");
			}
		}


		else if (command[0]=='I')
		{
			if (command[1]=='V')
			{
				send_command(IV, fd);
			}
			else if (command[1]=='D')
			{
				send_command(ID, fd);
			}
			else
			{
				printf("Wrong I'?' command\n");
			}
		}


		else if (command[0]=='R' && command[1]=='R')
		{
			printf("------> Reset started, wait for ~10 seconds\n");
			write(fd, RR, 3);
		}

		else if (command[0]=='E' && command[1]=='X' && command[2]=='I' && command[3]=='T')
		{
			close(fd);										// close UART
			printf("PS1 closed\n");

			system("/media/scripts/gpio.sh 8");				// turn power off
			printf("------> Power disabled\n");
			exit(0);
		}

		else
		{
			printf("Wrong command\n");
		}
	usleep(1000);
}

#include "lidar.h"

int main(int argc, char** argv)
{
	int fd=0;
	UART_init(&fd);

	char GPIO_init[] = "/media/scripts/gpio.sh 12";
	system(GPIO_init);
	printf("------> Power enabled\n");

	printf("wait %d seconds to start..\n", INIT_TIME);
	usleep (10000000);

	char command[32];

	if (argc > 1){
		for (int i = 1; i < argc; i++){
			printf("%s\n", argv[i]);
			parse_and_send_command(argv[i], fd);
		}
	} else {
		for(;;)
		{
			printf ("Enter a command\n");
			scanf(" %30[^\n]s", command);
			parse_and_send_command(command, fd);
		}
	}
	return 0;
}

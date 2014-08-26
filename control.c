#include <stdio.h>
#include <unistd.h>
#include "control.h"
#include "LDAhid.h"


int main(int argc, char const *argv[])
{
	/* get the uid of caller */
	uid_t uid = geteuid();
	int device_count = 0;
	int version = fnLDA_LibVersion();

	if (uid != 0) {
		printf("brick needs to be run as root to access USB ports\n");
		printf("please execute again as root\n");
		return -1;
	}
	if (argc < 2){
		printf("Error: usage: brick [-option] \n");
		printf("for a list of options type brick -h\n");
		return -1;
	}
	if (argv[1] == "-h"){
		//print available options
		return 0;
	}

	fnLDA_Init();

	if (argv[1] == "-v"){
		printf("Lib version: %s\n", version);
		return 0;
	}

	fnLDA_SetTestMode(FALSE);
	device_count = fnLDA_GetNumDevices();
	printf("you are using libversion: %s\n", version);
	printf("There are %s atenuators connected\n", device_count);
	return 0;
}
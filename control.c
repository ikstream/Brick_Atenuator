#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "control.h"
#include "LDAhid.h"

#define FALSE 0

struct user_data ud;

int 
get_serial_and_name(int device_count, unsigned int serial, char *device_name)
{
	DEVID id;
	for (id = 1; id <= device_count; id++) {
		printf("Device %d has Modelname: ", id);
		fnLDA_GetModelName(id, device_name);
		serial = fnLDA_GetSerialNumber(id);
		printf("%s with serial number %d\n", device_name, serial);
	}
	return 0;
}

char * 
get_device_data(unsigned int *working_devices, int nr_active_devices)
{
	char *success = "All data has been set correctly";
	int status, id;

	for(id = 0; id < nr_active_devices; id++){
		status = fnLDA_GetAttenuation(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));

		status = fnLDA_GetMinAttenuation(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));
		
		status = fnLDA_GetMaxAttenuation(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));
		
		status = fnLDA_GetIdleTime(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));
		
		status = fnLDA_GetDwellTime(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));
		
		status = fnLDA_GetAttenuationStep(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));
		
		status = fnLDA_GetRF_On(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));
		
		status = fnLDA_GetRampStart(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));

		status = fnLDA_GetRampEnd(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
					return strncpy(errmsg ,fnLDA_perror(status), sizeof(errmsg));

		return success;
	}
	
}

int 
get_parameters(int argc, char const *argv[])
{
	int i;
	for (i = 0; i < argc; i++){
		if (strncmp(argv[i], "-t", sizeof(argv[i])) == 0) {
			ud.atime = atoi(argv[i + 1]);
			printf("time for attenuation set to %d seconds.\n",
				ud.atime);
		}
		if (strncmp(argv[i], "-a", sizeof(argv[i])) == 0) {
			ud.attenuation = atoi(argv[i + 1]);
			printf("attenuation set to %d dB\n", ud.attenuation);
		}
		if (strncmp(argv[i], "-f", sizeof(argv[i])) == 0) {
			if (strncmp(argv[i + 1], "ramp",
				sizeof(argv[i + 1])) == 0){
					printf("attenuation set to ramp\n");
					ud.ramp = 1;
			}
			else if (strncmp(argv[i + 1], "sine",
				sizeof(argv[i + 1])) == 0){
					printf("attenuation set to sine\n");
					ud.sine = 1;	
			}
			else if (strncmp(argv[i + 1],"triangle",
				sizeof(argv[i + 1])) == 0){
					printf("attenuation set to triangle\n");
					ud.triangle = 1;
			}
		}
	}
	return 0;
}

int 
main(int argc, char const *argv[])
{
	int device_count = 0;
	int id, nr_active_devices, status, i;
	int serial;
	DEVID working_devices[MAXDEVICES];
	char device_name[MAX_MODELNAME];
	char *messages, *tmp;
	/*
	 * TODO: make input overflow safe
	 */
	char *input = "";
	/* get the uid of caller */
	uid_t uid = geteuid();

	if (uid != 0) {
		printf("brick needs to be run as root to access USB ports\n");
		printf("please execute again as root\n");
		return -1;
	}
	if (argc < 2) {
		printf("Error: usage: brick [-option] \n");
		printf("for a list of options type brick -h\n");
		return -1;
	}
	if ((strncmp(argv[1], "-h", sizeof(argv[1]))) == 0) {
		//print available options
		printf("this is a stump for help\n");
		return 0;
	}

	get_parameters(argc, argv);
	fnLDA_Init();
	char *version = fnLDA_LibVersion();

	fnLDA_SetTestMode(FALSE);
	/* 
	 * TODO: check in intervalls if connected devices have been 
	 * exchanged or disconnected 
	 */
	device_count = fnLDA_GetNumDevices();

	printf("you are using libversion %s\n", version);
	printf("There is/are %d atenuator/s connected\n", device_count);
	get_serial_and_name(device_count, serial, device_name);
	nr_active_devices = fnLDA_GetDevInfo(working_devices);
	printf("%d active devices found\n", nr_active_devices);

	/*
	 * initiate devices
	 */
	for (id = 0; id < nr_active_devices; id++){
		status = fnLDA_InitDevice(working_devices[id]);
		if (status != 0){
			printf("initialisation of device %d unsucsessfull\n",
				id);
			continue;
		}
		printf("initialized device %d successfully\n", id);
	}

	messages = get_device_data(working_devices, nr_active_devices);
	printf("%s\n", messages);

	/*
	 * close any open device
	 */
	for (id = 0; id <= nr_active_devices; id++){
		status = fnLDA_CloseDevice(working_devices[id]);
		if (status != 0){
			printf("shut down of device %d unsucsessfull\n",
				id);
			continue;
		}
		printf("shut down of device %d was successfull\n", id);
	}

	return 0;
}
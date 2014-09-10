#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "control.h"
#include "input.h"
#include "LDAhid.h"

#define _GNU_SOURCE
#define FALSE 0
#define TRUE !FALSE
#define STRING_LENGTH 12
#define SLEEP_TIME 100000
#define LINE_LENGTH 256
#define TIME 1
#define ATT 2

struct user_data ud;

//TODO: write man page

//TODO: currently only useable with one device at some points
//even if it is possible to use several devices they will be handled 
//one after the other
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
	return 1;
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
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));

		status = fnLDA_GetMinAttenuation(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
		
		status = fnLDA_GetMaxAttenuation(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
		
		status = fnLDA_GetIdleTime(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
		
		status = fnLDA_GetDwellTime(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
		
		status = fnLDA_GetAttenuationStep(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
		
		status = fnLDA_GetRF_On(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
		
		status = fnLDA_GetRampStart(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));

		status = fnLDA_GetRampEnd(working_devices[id]);
			if (status == INVALID_DEVID
			    || status == DEVICE_NOT_READY)
				return strncpy(errmsg ,fnLDA_perror(status),
				     strlen(errmsg));
	}
	return success;
	
}

void
call_help(void)
{
	//TODO: bring options in sensible order
	printf("-set attenuation with\n");
	printf("\t-a <attenuation in dB>\n");
	printf("\r\n");

	printf("-set time for attenuation duration with\n");
	printf("\t-t <time in sec>\n");
	printf("\r\n");

	printf("-set starting attenuation stregth in dB with\n");
	printf("\t-start <dB>\n");
	printf("\r\n");

	printf("-set end attenuation stregth in dB with\n");
	printf("\t-end <dB>\n");
	printf("\r\n");

	printf("-set time per step with\n");
	printf("\t-step_time\n");
	printf("\r\n");

	printf("-set attenuation form with\n");
	printf("\t-p <ramp|sine|triangle>\n");
	printf("\r\n");

	printf("-to use a .csv file use\n");
	printf("\t-f path/to/file\n");
	printf("\r\n");

  	printf("repeat form, or file input until canceled by user\n");
  	printf("\t-r\n");
  	printf("\r\n");

	printf("-to show this overview use\n");
	printf("\t-h\n");
	printf("\r\n");

	return;
}

int 
set_ramp(int id)
{
	int i, cur_att;

	//TODO: check if all ramp options are covered
	if (ud.cont && (ud.start_att < ud.end_att)){
		for(;;){
			fnLDA_SetAttenuation(id, ud.start_att);
			for(i = 0; i <= (ud.end_att - ud.start_att); i++){
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att + ud.ramp_steps);
			}
		}
	}
	else if (ud.cont && (ud.start_att > ud.end_att)){
		for(;;){
			fnLDA_SetAttenuation(id, ud.start_att);
			for(i = 0; i <= (ud.start_att - ud.end_att); i++){
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att - ud.ramp_steps);
			}
		}
	}
	else if (ud.start_att < ud.end_att){
		fnLDA_SetAttenuation(id, ud.start_att);
		for(i = 0; i <= (ud.end_att - ud.start_att); i++){
			sleep(MIKRO_SEC(ud.step_time));
			cur_att = fnLDA_GetAttenuation(1);
			printf("cur_att %d\n", cur_att);
			fnLDA_SetAttenuation(id,
				cur_att + ud.ramp_steps);
		}
	}
	else if (ud.start_att > ud.end_att){
		fnLDA_SetAttenuation(id, ud.start_att);
		for(i = 0; i <= (ud.start_att - ud.end_att); i++){
			sleep(MIKRO_SEC(ud.step_time));
			cur_att = fnLDA_GetAttenuation(1);
			printf("cur_att %d\n", cur_att);
			fnLDA_SetAttenuation(id,
				cur_att - ud.ramp_steps);
		}
	}
}

/*
 * Sets attenuation to a level defined by user, if the
 * user set attenuation is not above Max or below Min
 * attenuation of the Brick connected is and will sleep
 * for the time given by the user or the standard sleeptime
 * After the function leaves the attenuation will be reset 
 * to 0 again.
 */
int
set_attenuation(unsigned int id)
{
	if (ud.attenuation < fnLDA_GetMinAttenuation(id)){
		printf("%d is below minumal attenuation of %d\n",
			ud.attenuation, fnLDA_GetMinAttenuation(id));
		printf("attenuation has been set to 0dB\n");
		fnLDA_SetAttenuation(id, 0);
		sleep(MIKRO_SEC(ud.atime));
		return 1;
	}
	if (ud.attenuation > fnLDA_GetMaxAttenuation(id)){
		printf("%d is above maximal attenuation of %d\n",
			ud.attenuation, fnLDA_GetMaxAttenuation(id));
		printf("attenuation has been set to %d\n",
			fnLDA_GetMaxAttenuation(id));
		fnLDA_SetAttenuation(id, fnLDA_GetMaxAttenuation(id));
		sleep(MIKRO_SEC(ud.atime));
		return 1;
	}
	fnLDA_SetAttenuation(id, (ud.attenuation * 4));
	printf("set device to %ddB attenuation\n", fnLDA_GetAttenuation(1));
	sleep(MIKRO_SEC(ud.atime));
	return 1;
}

int
set_triangle(unsigned int id)
{
	//TODO: check if triangle is working correctly

	//TODO: check for to high or to low attenuation

	//TODO: add non cont case
	int i, cur_att;

	fnLDA_SetAttenuation(id, ud.start_att);
	if (ud.cont && (ud.start_att < ud.end_att)) {
		for(;;) {
			for (i = 1; i <= (ud.end_att - ud.start_att); i++) {
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att + ud.ramp_steps);
			}
			for (i = ud.end_att; i > (ud.end_att - ud.start_att); i--) {
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att - ud.ramp_steps);
			}
			fnLDA_SetAttenuation(id, ud.start_att);
		}
	}	
	if (ud.cont && (ud.start_att > ud.end_att)) {
		for(;;){
			for (i = 0; i < (ud.start_att - ud.end_att); i++) {
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att - ud.ramp_steps);
			}
			for (i = 1; i <= (ud.start_att - ud.end_att); i++) {
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att + ud.ramp_steps);
			}
			fnLDA_SetAttenuation(id, ud.start_att);
		}
	}	
}

//TODO: implement set_sine function

//TODO: add function to show max/min att, stepsize and other device infos
int 
main(int argc, char *argv[])
{
	int device_count = 0;
	int id, nr_active_devices, status, i;
	int serial, parameter_status;
	DEVID working_devices[MAXDEVICES];
	char device_name[MAX_MODELNAME];
	char *messages, *tmp;
	
	//TODO: make input overflow safe
	char *input = "";
	
	/* get the uid of caller */
	uid_t uid = geteuid();
	clear_userdata();
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
	if ((strncmp(argv[1], "-h", strlen(argv[1]))) == 0) {
		call_help();	
		return 0;
	}
	if (!get_parameters(argc, argv))
		return 0;
	fnLDA_Init();

	
	char *version = fnLDA_LibVersion();

	fnLDA_SetTestMode(FALSE);
	
	//TODO: check in intervalls if connected devices have been 
	//exchanged or disconnected 
	device_count = fnLDA_GetNumDevices();

	printf("you are using libversion %s\n", version);
	if (device_count > 1)
		printf("There are %d atenuators connected\n", device_count);
	else
		printf("There is %d atenuator connected\n", device_count);
	get_serial_and_name(device_count, serial, device_name);
	nr_active_devices = fnLDA_GetDevInfo(working_devices);
	printf("%d active devices found\n", nr_active_devices);


	for (id = 0; id < nr_active_devices; id++) {
		if ((strncmp(argv[1], "-i", strlen(argv[1]))) == 0)
			print_dev_info(id);
	}

	/*
	 * initiate devices
	 */
	for (id = 0; id < nr_active_devices; id++){
		status = fnLDA_InitDevice(working_devices[id]);
		if (status != 0){
			printf("initialisation of device %d unsucsessfull\n",
				id + 1);
			continue;
		}
		printf("initialized device %d successfully\n", id + 1);
	}

	messages = get_device_data(working_devices, nr_active_devices);
	printf("%s\n", messages);
	printf("You can set attenuation steps in %ddB steps\n",
		(fnLDA_GetDevResolution(1) / 4));
	print_userdata();
	/*
	 * Set device as specified by user
	 */
	fnLDA_SetAttenuation(1, 0);
	for (id = 1; id <= nr_active_devices; id++){
		if (ud.sine == 1){
			/* TODO call sine_function which will set ramp form
		 	 * in intervall maybe with steps and set one step a
		 	 * second so it will be decided by step size and
		 	 * timehow many curve intervalls there will be */
		}
		else if (ud.triangle == 1)
			set_triangle(id);
		else if (ud.ramp == 1)
			set_ramp(id);
		else if (ud.simple == 1)
			set_attenuation(id);
		else if (ud.file && ud.cont) {
			for(;;)
				read_file(ud.path, id);
		}
		else if (ud.file == 1)
			read_file(ud.path, id);

		fnLDA_SetAttenuation(id, 0);
	}

	/*
	 * close any open device
	 */
	for (id = 0; id < nr_active_devices; id++){
		status = fnLDA_CloseDevice(working_devices[id]);
		if (status != 0){
			printf("shut down of device %d unsucsessfull\n",
				id + 1);
			continue;
		}
		printf("shut down of device %d was successfull\n", id + 1);
	}
	return 1;
	//TODO: find out, why libusb leaves devices open
}
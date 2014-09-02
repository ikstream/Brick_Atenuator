#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "control.h"
#include "LDAhid.h"

#define FALSE 0
#define TRUE !FALSE
#define MIKRO_SEC(step_time) ((1 * step_time) / 100000)

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
	}
	return success;
	
}

void
call_help(void)
{
	//print available options
	printf("-set attenuation with\n");
	printf("\t-a <attenuation in dB>\n");
	printf("\r\n");

	printf("-set time per step with\n");
	printf("\t-step_time\n");
	printf("\r\n");

	// printf("-set time for attenuation duration with\n");
	// printf("\t-t <time in sec>\n");
	// printf("\r\n");

	printf("-set starting attenuation stregth in dB with\n");
	printf("\t-start <dB>\n");
	printf("\r\n");

	printf("-set end attenuation stregth in dB with\n");
	printf("\t-end <dB>\n");
	printf("\r\n");

	printf("-set attenuation form with\n");
	printf("\t-f <ramp|sine|triangle>\n");
	printf("\r\n");

  	printf("repeat operation until canceled by user\n");
  	printf("\tcont\n");
  	printf("\r\n");

	printf("-to show this overview use\n");
	printf("\t-h\n");
	printf("\r\n");

	return;
}

int 
get_parameters(int argc, char const *argv[])
{
	int i;
	for (i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-t", sizeof(argv[i])) == 0) {
			ud.atime = atoi(argv[i + 1]);
			printf("time for attenuation set to %d seconds.\n",
				ud.atime);
		}
		if (strncmp(argv[i], "-step", sizeof(argv[i])) == 0) {
			ud.ramp_steps = atoi(argv[i + 1]);
			printf("ramp steps set to %d dB\n", ud.ramp_steps);
		}
		if (strncmp(argv[i], "-step_time", sizeof(argv[i])) == 0) {
			ud.step_time = atoi(argv[i + 1]);
			printf("time per step set to %d mikroseconds\n", ud.step_time);
		}
		if (strncmp(argv[i], "-start", sizeof(argv[i])) == 0) {
			ud.start_att = atoi(argv[i + 1]);
			printf("start attenuation set to %d dB\n", ud.start_att);
		}
		if (strncmp(argv[i], "-end", sizeof(argv[i])) == 0) {
			ud.end_att = atoi(argv[i + 1]);
			printf("end attenuation set to %d dB\n", ud.end_att);
		}
		if (strncmp(argv[i], "-a", sizeof(argv[i])) == 0) {
			ud.attenuation = atoi(argv[i + 1]);
			printf("attenuation set to %d dB\n", ud.attenuation);
		}
		if (strncmp(argv[i], "-f", sizeof(argv[i])) == 0) {
			if (strncmp(argv[i + 1], "ramp",
				sizeof(argv[i + 1])) == 0) {
					printf("attenuation set to ramp\n");
					ud.ramp = 1;
			}
			else if (strncmp(argv[i + 1], "sine",
				sizeof(argv[i + 1])) == 0) {
					printf("attenuation set to sine\n");
					ud.sine = 1;	
			}
			else if (strncmp(argv[i + 1],"triangle",
				sizeof(argv[i + 1])) == 0) {
					printf("attenuation set to triangle\n");
					ud.triangle = 1;
			}
			else {
				printf("invalid Argument after -f\n");
				printf("ues ramp, sine or triangle\n");
			}
		}
		if (ud.ramp || ud.triangle){
			if (strncmp(argv[i], "cont", sizeof(argv[i]))
			    == 0) {
				printf("continous behavior is set\n");
				ud.cont = 1;
			}
		}
		else {
			printf("%s is no valid option\n", argv[i]);
			printf("use -h option for available options\n");
			return 0;
		}
	}
	return 1;
}

int 
set_ramp(int id)
{
	int i, cur_att;

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
int
set_one_attenuation(unsigned int id)
{
	if (ud.attenuation > fnLDA_GetMaxAttenuation(id)){
		printf("%d is above maximal attenuation\n", ud.attenuation);
		return 0;
	}
}

int
set_triangle(unsigned int id)
{
	int i, cur_att;

	if (ud.cont && (ud.start_att < ud.end_att)) {
		for(;;) {
			fnLDA_SetAttenuation(id, ud.start_att);
			for (i = 0; i <= (ud.end_att - ud.start_att); i++) {
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att + ud.ramp_steps);
			}
			for (i = 0; i < (ud.start_att - ud.end_att); i++) {
				sleep(MIKRO_SEC(ud.step_time));
				cur_att = fnLDA_GetAttenuation(1);
				printf("cur_att %d\n", cur_att);
				fnLDA_SetAttenuation(id,
					cur_att - ud.ramp_steps);
			}
		}
	}	
	if (ud.cont && (ud.start_att > ud.end_att)) {
		for(;;){
			fnLDA_SetAttenuation(id, ud.start_att);
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

void
clear_userdata(void)
{
	ud.atime = 60;
	ud.attenuation = 0;
	ud.start_att = 0;
	ud.end_att = 0;
	ud.ramp = 0;
	ud.sine = 0;
	ud.triangle = 0;
	ud.ramp_steps = 1;
	ud.cont = 0;
	ud.step_time = 100000;
}

int 
main(int argc, char const *argv[])
{
	int device_count = 0;
	int id, nr_active_devices, status, i;
	int serial, parameter_status;
	DEVID working_devices[MAXDEVICES];
	char device_name[MAX_MODELNAME];
	char *messages, *tmp;
	/*
	 * TODO: make input overflow safe
	 */
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
	if ((strncmp(argv[1], "-h", sizeof(argv[1]))) == 0) {
		call_help();	
		return 0;
	}

	if (!get_parameters(argc, argv))
		return 0;

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
	 * Set device as specified by user
	 */
	fnLDA_SetAttenuation(1, 0);
	for (id = 1; id <= nr_active_devices; id++){
		printf("in for loop\n");
		printf("id is %d\n", id);
		printf("ud.ramp is set to%d\n", ud.ramp);
		if (ud.sine == 1)
			printf("sine bla\n");
			/* TODO call sine_function which will set ramp form
		 	 * in intervall mybe with steps and set one step a
		 	 * second so it will be decided by step size and
		 	 * timehow many curve intervalls there will be */
		else if (ud.triangle == 1){
			printf("triangle bla\n");
			set_triangle(id);
		}
		else if (ud.ramp == 1){
			printf("sending to set_ramp now\n");
			set_ramp(id);
			/* goes straight to max power. needs formular to make
			 * good steps if not set and step per time intervall
			 * needs to be set */
		}
	}

	/*
	 * close any open device
	 */
	for (id = 0; id < nr_active_devices; id++){
		status = fnLDA_CloseDevice(working_devices[id]);
		if (status != 0){
			printf("shut down of device %d unsucsessfull\n",
				id);
			continue;
		}
		printf("shut down of device %d was successfull\n", id);
	}
	return 1;
}
#ifndef _CONTROL_H_
#define _CONTROL_H_

char errmsg[64];

struct user_data
{
	unsigned int atime;
	unsigned int attenuation;
	unsigned int start_att;
	unsigned int end_att;
	unsigned int ramp;
	unsigned int sine;
	unsigned int triangle;
	unsigned int ramp_steps;
	unsigned int cont;
	unsigned int step_time;
};

int get_serial_and_name(int device_count, unsigned int serial,
	char *device_name);
char * get_device_data(unsigned int *working_devices, int nr_active_devices);

#endif
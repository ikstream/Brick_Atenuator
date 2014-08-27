#ifndef _CONTROL_H_
#define _CONTROL_H_

char errmsg[64];

struct user_data
{
	unsigned int atime;
	unsigned int attenuation;
	unsigned int ramp;
	unsigned int sine;
	unsigned int triangle;
};
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "input.h"
#include "LDAhid.h"

#define FALSE 0
#define TRUE !FALSE
#define STRING_LENGTH 12
#define SLEEP_TIME 100000
#define LINE_LENGTH 256
#define TIME 1
#define ATT 2

struct user_data ud;
/*
 * checks if a given string is a number
 */
int
check_if_number(char* str)
{
	
	for (str; str < (char*)strlen(str); str++) {

	}
}

/*
 * returns token on given place in .csv file
 */
char* 
get_entry(char* line, int entry)
{
	char* token;
	for (token = strtok(line, ";"); token && *token;
	     token = strtok(NULL, ";\n")) {
		if (!--entry)
			return token;
	}
	return NULL;
}

/*
 * follows the path to a given config/.csv file
 * and checks it for correct entries.
 * time is expected to be in the first entry followed by the
 * attenuation. 
 */
int
read_file(char *path, int id)
{
	//TODO: check if file in path is a .csv file

	int i = 0;
	int isDigit = 0;
	FILE *fp;
	char *tmp, *endptr;
	char line[LINE_LENGTH];
	char atime[250];
	char att[6];
	
	if(fopen(path, "r") == NULL){
		printf("fp == NULL\n");
		return -1;
	}
	fp = fopen(path, "r");

	while (fgets(line, LINE_LENGTH, fp)) {
		tmp = strdup(line);
		ud.atime = atoi(get_entry(tmp, TIME));
		tmp = strdup(line);
		ud.attenuation = (int)atof(get_entry(tmp, ATT));
		set_attenuation(id);
		tmp = strdup(line);
		if ((void*)atoi(get_entry(tmp, 3)) != NULL) {
			printf("There are more the two entries in the line: ");
			printf("%s\n", line);
			printf("but only two entries are permitted\n");
			printf("this values will be ignored\n");
		}
		free(tmp);
	}

	fclose(fp);
	return 1;
}

/*
 * gets the command line parameters and sets userdata parameters
 */
int 
get_parameters(int argc, char *argv[])
{
	int i;
	//TODO: let user set ramp, triangle and/or sine after each other

	//TODO: check for invalid input

	//TODO: check for order of arguments

	//TODO: fix Problem if argv[i + 1] does not exist
	//TODO: check if argv[i + 1] is what is expected
	if (argc == 2){
		if (strncmp(argv[1], "-i", strlen(argv[1])) != 0) {
			printf("you are missing parameters for %s\n", argv[1]);
			printf("\r\nusage:\n");
			return 0;
		}
	}

	for (i = 1; i < argc - 1; i++) {
		if (strncmp(argv[i], "-a", strlen(argv[i])) == 0) {
			ud.simple = 1;
			if ((i + 1) < argc)
				ud.attenuation = (int)(atof(argv[i + 1]) * 4);
			else
				printf("you set the -a switch, but missed to enter an attenuation\n");
		}

		if (strncmp(argv[i], "-i", strlen(argv[i])) == 0)
			ud.info = 1;

		if (strncmp(argv[i], "-t", strlen(argv[i])) == 0)
			if ((i + 1) > argc) {
				if (ud.time != 0)
					printf("Time set to: %d\n", ud.atime);
				break;
			}
			ud.atime = atoi(argv[i + 1]);

		if (strncmp(argv[i], "-step", strlen(argv[i])) == 0)
			if ((i + 1) > argc) {
				printf("Stepsize set to %.1fdB\n",
					((double)ud.ramp_steps) / 4);
				break;
			}
			ud.ramp_steps = (int)(atof(argv[i + 1]) * 4);

		if (strncmp(argv[i], "-step_time", strlen(argv[i])) == 0)
			if ((i + 1) > argc){
				printf("Steptime set to %d\n", ud.step_time);
				break;
			}
			ud.step_time = atoi(argv[i + 1]);

		if (strncmp(argv[i], "-start", strlen(argv[i])) == 0)
			if ((i + 1) > argc){
				//TODO: check if min_att can be set here
				//through get_min_att
				printf("Start attenuation set to %.1fdB\n",
					((double)ud.start_att) / 4);
				break;
			}
			ud.start_att = (int)(atof(argv[i + 1]) * 4);

		if (strncmp(argv[i], "-end", strlen(argv[i])) == 0)
			if ((i + 1) > argc){
				printf("Set end attenuation to %.1fdB\n",
					(double)ud.end_att / 4);
				break;
			}
			ud.end_att = (int)(atof(argv[i + 1]) * 4);

		if (strncmp(argv[i],"-f", strlen(argv[i])) == 0) {
			if ((i + 1) > argc) {
				printf("You probably wanted to use a file,");
				printf("but its seems like you forgot to ");
				printf("enter the path to the file\n");
				break;
			}
			ud.path = argv[i + 1];
			ud.file = 1;
		}

		if (strncmp(argv[i], "-r", strlen(argv[i])) == 0)
				ud.cont = 1;

		if(strncmp(argv[i], "-p", strlen(argv[i])) == 0) {	
			if (strncmp(argv[i + 1], "-ramp",
			    strlen(argv[i + 1])) == 0)
				ud.ramp = 1;
			else if (strncmp(argv[i + 1], "-sine",
			    strlen(argv[i + 1])) == 0)
				ud.sine = 1;	
			else if (strncmp(argv[i + 1],"-triangle",
			    strlen(argv[i + 1])) == 0)
				ud.triangle = 1;
		}
				
		if (strncmp(argv[i], "-r", strlen(argv[i])) == 0) {
			ud.cont = 1;
		}
	}
	return 1;
}

void
print_userdata(void)
{
	if (ud.simple == 1) {
		printf("attenuation set to %.1fdB\n", (double)ud.attenuation / 4);
		if (ud.atime != 0)
			printf("time for attenuation set to %d mikroseconds.\n", ud.atime);
	}
	if (ud.ramp == 1) {
		printf("attenuation set to ramp\n");
		printf("ramp steps set to %.1fdB\n", (double)ud.ramp_steps / 4);
		printf("start attenuation set to %.1fdB\n", (double)ud.start_att / 4);
		printf("end attenuation set to %.1fdB\n", (double)ud.end_att / 4);
		printf("time per step set to %d mikroseconds\n", ud.step_time);
	}
	if (ud.triangle == 1) {
		printf("attenuation form set to both sided ramp\n");
		printf("ramp steps set to %.1fdB\n", (double)ud.ramp_steps / 4);
		printf("start attenuation set to %.1fdB\n", (double)ud.start_att / 4);
		printf("maximal attenuation set to %.1fdB\n", (double)ud.end_att / 4);
		printf("time per step set to %d mikroseconds\n", ud.step_time);
	}
	if (ud.cont == 1)
		printf("continous behavior is set\n");
	if (ud.sine == 1)
		printf("attenuation set to sine\n");
}

void
clear_userdata(void)
{
	ud.atime = 0;
	ud.attenuation = 0;
	ud.start_att = 0;
	ud.end_att = 0;
	ud.ramp = 0;
	ud.sine = 0;
	ud.triangle = 0;
	ud.ramp_steps = 4;
	ud.cont = 0;
	ud.step_time = SLEEP_TIME;
	ud.simple = 0;
	ud.file = 0;
	ud.info = 0;
	ud.path = "";
}
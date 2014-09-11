#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
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
 * follows the path to a given .csv file
 * and checks it for correct entries.
 * time is expected to be in the first entry followed by the
 * attenuation. 
 */
int
read_file(char *path, int id)
{
	//TODO check if file in path is a .csv file
	printf("read file\n");
	int i = 0;
	FILE *fp;
	char *tmp;
	char *endptr;
	char line[512];
	
	printf("if check\n");
	if(fopen(path, "r") == NULL){
		printf("fp == NULL\n");
		return -1;
	}
	fp = fopen(path, "r");
	printf("opened file\n");
	fgets(line, LINE_LENGTH, fp);
	tmp = strdup(line);
	printf("line 60\n");
	if (strtol(get_entry(tmp, 3), &endptr, 10) != 0) {
		printf("more then two entries found in \"%s\"\n", path);
		printf("there are only two entries supported\n");
		printf("file should have the format:\n");
		printf("time per step;attenuation per step\n");
		printf("proceding with first two entries of:");
		printf("%s\n", path);
	}
	printf("before while\n");
	while (fgets(line, LINE_LENGTH, fp)) {
		
		tmp = strdup(line);
		strtol(get_entry(tmp, TIME), &endptr, 10);
		if ( endptr == NULL) {
			ud.atime = atoi(get_entry(tmp, TIME));
			printf("ud.atime: %d\n", ud.atime);
		}
		else
			printf("\"%s\" is no valid time\n", get_entry(tmp, 1));

		tmp = strdup(line);
		strtol(get_entry(tmp, ATT), &endptr, 10);
		if ( endptr == NULL) {
			ud.attenuation = atoi(get_entry(tmp,ATT));
			printf("ud.attenuation: %d\n", ud.attenuation);
		}
		else
			printf("\"%s\" is no valid attenuation\n",
				get_entry(tmp, ATT));
		set_attenuation(id);
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
	if (argc == 2){
		printf("you are missing parameters for %s\n", argv[1]);
		printf("\r\nusage:\n");
		return 0;
	}

	for (i = 1; i < argc - 1; i++) {
		printf("0.1\n");
		if (strncmp(argv[i], "-t", strlen(argv[i])) == 0)
			ud.atime = atoi(argv[i + 1]);

		if (strncmp(argv[i], "-step", strlen(argv[i])) == 0)
			ud.ramp_steps = atoi(argv[i + 1]);

		if (strncmp(argv[i], "-step_time", strlen(argv[i])) == 0)
			ud.step_time = atoi(argv[i + 1]);

		if (strncmp(argv[i], "-start", strlen(argv[i])) == 0)
			ud.start_att = atoi(argv[i + 1]);

		if (strncmp(argv[i], "-end", strlen(argv[i])) == 0)
			ud.end_att = atoi(argv[i + 1]);

		if (strncmp(argv[i],"-f", strlen(argv[i])) == 0) {
			ud.path = argv[i + 1];
			ud.file = 1;
			read_file(ud.path, 1);
		}

		if (strncmp(argv[i], "-a", strlen(argv[i])) == 0) {
			printf("0.2\n");
			ud.simple = 1;
			if ((i + 1) < argc){
				printf("argc: %d\n", argc);
				printf("i+1: %i\n", i +1);
				ud.attenuation = atoi(argv[i + 1]);
			}
			else
				printf("you set the -a switch, but missed to erter an attenuation\n");
		}

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

		if (ud.ramp || ud.triangle || ud.file) {
			if (strncmp(argv[i], "-r", strlen(argv[i]))
			    == 0)
				ud.cont = 1;
		}

	}
	return 1;
}

void
print_userdata(void)
{
	if (ud.simple == 1) {
		printf("attenuation set to %d dB\n", ud.attenuation);
		printf("time for attenuation set to %d seconds.\n", ud.atime);
	}
	if (ud.ramp == 1) {
		printf("attenuation set to ramp\n");
		printf("ramp steps set to %d dB\n", ud.ramp_steps);
		printf("start attenuation set to %d dB\n", ud.start_att);
		printf("end attenuation set to %d dB\n", ud.end_att);
		printf("time per step set to %d mikroseconds\n", ud.step_time);
	}
	if (ud.triangle == 1) {
		printf("attenuation form set to both sided ramp\n");
		printf("ramp steps set to %d dB\n", ud.ramp_steps);
		printf("start attenuation set to %d dB\n", ud.start_att);
		printf("maximal attenuation set to %d dB\n", ud.end_att);
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
	ud.atime = 60;
	ud.attenuation = 0;
	ud.start_att = 0;
	ud.end_att = 0;
	ud.ramp = 0;
	ud.sine = 0;
	ud.triangle = 0;
	ud.ramp_steps = 1;
	ud.cont = 0;
	ud.step_time = SLEEP_TIME;
	ud.simple = 0;
	ud.file = 0;
}
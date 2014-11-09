#ifndef _INPUT_H_
#define _INPUT_H_

#define MIKRO_SEC(step_time) ((1 * step_time) / 1000000)
#define MILLI_SEC(step_time) ((1 * step_time) / 1000)

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
	unsigned int simple;
	unsigned int file;
	unsigned int info;
	unsigned int runs;
	unsigned int ms;
	unsigned int us;
	unsigned int log;
	char *path;
	char *logfile;
};

int read_file(char *path, int id);
char * get_entry(char* line, int entry);
int get_parameters(int argc, char *argv[]);
void print_userdata(void);
void clear_userdata(void);

#endif

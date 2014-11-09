struct dev_data {
	unsigned int dev_max_att;
	unsigned int dev_min_att;
	unsigned int dev_res;
	char dev_name[64];
}


unsigned short 
get_dev_id()
{
	return 1;
}

void
set_device_data()
{
	/* get_dev_id and set max/min att according to its specs */
}

int
clear_device_data()
{

}

int
sent_data()
{
	/* TODO: needs to be found out how it works */
}

/* TODO: prob check for errors from device */
/* TODO: set Attenuation and time */
/* TODO: multithreaded possibility */
//#include <window.h>
#include <gtk/gtk.h>
#include "calcbmi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "input.h"
#include "control.h"

#define _OPEN_SYS_ITOA_EXT

/* decreases factor, if a checkbutton is unchecked */
void unset_factor(int n)
{
	factor -= n;
}

void unset_value(GtkSpinButton *spin)
{
	gtk_spin_button_set_value(spin,0.0);
}

/* unchecks checkbutton */
void uncheck(GtkToggleButton *check)
{
	gtk_toggle_button_set_active(check, FALSE);
}

/* resets all values */
void reset(GtkToggleButton *button, GtkBuilder *builder)
{
	GObject *checkbutton;
	GObject *spin;
	GObject *label;

	checkbutton = gtk_builder_get_object(builder,"sine");
	uncheck((GtkToggleButton *)checkbutton);

	checkbutton = gtk_builder_get_object(builder,"ramp");
	uncheck((GtkToggleButton *)checkbutton);
	
	checkbutton = gtk_builder_get_object(builder,"both_sided_ramp");
	uncheck((GtkToggleButton *)checkbutton);

	checkbutton = gtk_builder_get_object(builder,"contin");
	uncheck((GtkToggleButton *)checkbutton);

	spin = gtk_builder_get_object(builder,"attenuation");
	unset_value((GtkSpinButton *)spin );

	spin = gtk_builder_get_object(builder,"time");
	unset_value((GtkSpinButton *)spin );

	label = gtk_builder_get_object(builder,"time_in_step");
	gtk_label_set_text((GtkLabel *)label, " ");

	factor = height = weight = bmi = 0;
	message_value_w = message_value_h = 1;
}

/* increases factor, if a checkbutton is checked */
void set_factor(int n)
{
	factor += n;
}

/* checks if checkButton gets activated, or deactivated */
void toggle_button_callback(GtkToggleButton *check, gpointer data)
{
	if(gtk_toggle_button_get_active (check)){
		int cast = GPOINTER_TO_INT( data);
		set_factor(cast);
	} 
	else{
		int cast = GPOINTER_TO_INT( data);
		unset_factor(cast);
	}
}

/* gets the value of the spinbuttons as double/ integer */
void get_double_height_spin_button(GtkToggleButton *button, GtkSpinButton *spin)
{
	height = gtk_spin_button_get_value(spin);
	if(height == 0){
		message_value_h = 0;
	}
	else{
		message_value_h = 1;
	}
}

void get_int_weight_spin_button(GtkToggleButton *button, GtkSpinButton *spin)
{
	weight = gtk_spin_button_get_value_as_int(spin);
	g_print("%d Weight", weight);
	if(weight == 0){
		message_value_w = 0;
	}
	else{
		message_value_w = 1;
	}
}

/* calculates actual bmi */
void calculate(GtkToggleButton * button, gpointer data)
{
	g_print("\n%f", height);
	g_print("\n%d", weight);
	bmi = calc_bmi(weight, height, factor);
	g_print("\n %f", bmi);
}	

char * convert_bmi(void)
{

	char int_bmi_strg[7], decimal_strg[7];
	char *bmi_strg = malloc(sizeof(int_bmi_strg)+ 1 + sizeof(decimal_strg) +1);

	int int_bmi = bmi;
	int decimal = (bmi - int_bmi) * 100;
	snprintf(int_bmi_strg, 7, "%d", int_bmi);
	snprintf(decimal_strg, 7, "%d", decimal);
	strncpy(bmi_strg, int_bmi_strg, 7);
	strncat(bmi_strg, ".", 1);
	strncat(bmi_strg, decimal_strg, 7);
	g_print("\n%s\n", bmi_strg);
	return bmi_strg;

}

/* sets bmi value for Label */
void set_text(GtkToggleButton *button, GtkLabel *label)
{
	char *text = convert_bmi();
	g_print("%s\n",text );
	if(message_value_h && message_value_w){
		gtk_label_set_text(label, text);
	}
	if (!message_value_h){
		gtk_label_set_text(label, "Größe ist auf 0 gesetzt");
	}
	if (!message_value_w){
		gtk_label_set_text(label, "Gewicht ist auf 0 gesetzt");
	}
   	
}

int main (int   argc, char *argv[])
{
	GtkBuilder      *builder;
	GObject         *window;
	GObject         *button;
	GObject         *checkbutton;
	GObject 	*spin;
	GObject 	*label;

	gtk_init( &argc, &argv);

	/* Construct a GtkBuilder instance and load our UI description */
	builder = gtk_builder_new();
	gtk_builder_add_from_file( builder, "builder.ui", NULL);

	/* Connect signal handlers to the constructed widgets. */
	window = gtk_builder_get_object (builder, "window");
	
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	
	button = gtk_builder_get_object (builder, "quit");
	g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

	/* checkButton for torso */
	checkbutton = gtk_builder_get_object(builder,"torso");
	g_signal_connect(checkbutton,"clicked", G_CALLBACK(toggle_button_callback), GINT_TO_POINTER( torso));

	/* spinbutton for height */
	spin = gtk_builder_get_object(builder,"height");
	GtkSpinButton *height_spinner = (GtkSpinButton *) spin;
	gtk_spin_button_set_increments( height_spinner,1,1);
	gtk_spin_button_set_range( height_spinner,0,300);
	
	/* spinbutton for weight */
	spin = gtk_builder_get_object(builder,"weight");
	GtkSpinButton *weight_spinner = (GtkSpinButton *) spin;
	gtk_spin_button_set_increments( weight_spinner,1,1);
	gtk_spin_button_set_range( weight_spinner,0,500);

	/* calculate button */
	button = gtk_builder_get_object(builder,"calculate");
	weight = g_signal_connect(button,"clicked",G_CALLBACK(get_int_weight_spin_button),weight_spinner);
	height = g_signal_connect(button,"clicked",G_CALLBACK(get_double_height_spin_button),height_spinner);
	g_signal_connect(button,"clicked",G_CALLBACK(calculate),NULL);
	
	/* getting label for bmi */
	label = gtk_builder_get_object(builder,"bmi");
	GtkLabel *bmi_label = (GtkLabel *) label;
	g_signal_connect(button, "clicked", G_CALLBACK(set_text), bmi_label);

	button = gtk_builder_get_object(builder,"reset");
	g_signal_connect(button,"clicked", G_CALLBACK(reset), builder);
	//g_signal_connect(button,"clicked", G_CALLBACK(gtk_window_new),window);

	gtk_main ();
  
	return 0;
}

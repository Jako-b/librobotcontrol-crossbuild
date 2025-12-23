/**
 * @file motor.c
 * @author James Strawson
 * @date 2018
 */

#include <stdio.h>
#include <rc/motor.h>
#include <rc/model.h>
#include <rc/gpio.h>
#include <rc/pwm.h>
#include <rc/rc_find_pin.h>

#define unlikely(x) __builtin_expect(!!(x), 0)
#define CHANNELS		4
#define CHANNELS_POCKET		2


// polarity of the motor connections
static const double polarity[]={1.0,-1.0,-1.0,1.0};

static int init_flag = 0;
static int stby_state = 0;
static int dirA_chip[CHANNELS];
static int dirA_pin[CHANNELS];
static int dirB_chip[CHANNELS];
static int dirB_pin[CHANNELS];
static int pwmss[CHANNELS];
static int pwmch[CHANNELS];
static int channels = 0;
static int stby_chip;
static int stby_pin;


int rc_motor_init(void)
{
	return rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ);
}


int rc_motor_init_freq(int pwm_frequency_hz)
{
	int i;

	int chip, line;

	if(rc_model()==MODEL_BB_POCKET){
		channels = CHANNELS_POCKET;
	}
	else{
		channels = CHANNELS;
	}

	// MDIR1A
	if (rc_find_pin("MDIR1A", &chip, &line) != 0) return -1;
	dirA_chip[0] = chip;
	dirA_pin[0]  = line;

	// MDIR1B
	if (rc_find_pin("MDIR1B", &chip, &line) != 0) return -1;
	dirB_chip[0] = chip;
	dirB_pin[0]  = line;

	// PWM mapping for motor 1 (regression fix)
	pwmss[0] = 1;
	pwmch[0] = 'A';

	// MDIR2A
	if (rc_find_pin("MDIR2A", &chip, &line) != 0) return -1;
	dirA_chip[1] = chip;
	dirA_pin[1]  = line;

	// MDIR2B
	if (rc_find_pin("MDIR2B", &chip, &line) != 0) return -1;
	dirB_chip[1] = chip;
	dirB_pin[1]  = line;
	
	if(rc_model()==MODEL_BB_POCKET) {
		pwmss[1]=0;
		pwmch[1]='A';
	} else {
		pwmss[1]=1;
		pwmch[1]='B';
	}

	// MDIR3A
	if (rc_find_pin("MDIR3A", &chip, &line) != 0) return -1;
	dirA_chip[2] = chip;
	dirA_pin[2]  = line;

	// MDIR3B
	if (rc_find_pin("MDIR3B", &chip, &line) != 0) return -1;
	dirB_chip[2] = chip;
	dirB_pin[2]  = line;
	pwmss[2]=2;
	pwmch[2]='A';

	// MDIR4A
	if (rc_find_pin("MDIR4A", &chip, &line) != 0) return -1;
	dirA_chip[3] = chip;
	dirA_pin[3]  = line;

	// MDIR4B
	if (rc_find_pin("MDIR4B", &chip, &line) != 0) return -1;
	dirB_chip[3] = chip;
	dirB_pin[3]  = line;

	pwmss[3]=2;
	pwmch[3]='B';

	// MOT_STBY
	if (rc_find_pin("MOT_STBY", &chip, &line) != 0) return -1;
	stby_chip = chip;
	stby_pin  = line;

	// set up pwm channels
	if(unlikely(rc_pwm_init(0,pwm_frequency_hz))){
		fprintf(stderr,"ERROR in rc_motor_init, failed to initialize pwm subsystem 0\n");
		return -1;
	}
	if(unlikely(rc_pwm_init(1,pwm_frequency_hz))){
		fprintf(stderr,"ERROR in rc_motor_init, failed to initialize pwm subsystem 1\n");
		return -1;
	}
	if(unlikely(rc_pwm_init(2,pwm_frequency_hz))){
		fprintf(stderr,"ERROR in rc_motor_init, failed to initialize pwm subsystem 2\n");
		return -1;
	}

	// set up gpio pins
	if(unlikely(rc_gpio_init(stby_chip, stby_pin, GPIOHANDLE_REQUEST_OUTPUT))){
        fprintf(stderr,"ERROR: failed to init MOT_STBY pin\n");
		return -1;
	}
	for(i=0;i<channels;i++){
		if(unlikely(rc_gpio_init(dirA_chip[i],dirA_pin[i], GPIOHANDLE_REQUEST_OUTPUT))){
			fprintf(stderr,"ERROR in rc_motor_init, failed to set up gpio %d,%d\n", dirA_chip[i],dirA_pin[i]);
			return -1;
		}
		if(unlikely(rc_gpio_init(dirB_chip[i],dirB_pin[i], GPIOHANDLE_REQUEST_OUTPUT))){
			fprintf(stderr,"ERROR in rc_motor_init, failed to set up gpio %d,%d\n", dirB_chip[i],dirB_pin[i]);
			return -1;
		}
	}

	// now set all the gpio pins and pwm to something predictable
	stby_state = 0;
	init_flag = 1;
	if(unlikely(rc_motor_free_spin(0))){
		fprintf(stderr,"ERROR in rc_motor_init\n");
		init_flag = 0;
		return -1;
	}

	// make sure standby is off since most users won't use it
	if(unlikely(rc_gpio_set_value(stby_chip, stby_pin,1))){
        fprintf(stderr,"ERROR: failed to init MOT_STBY pin\n");
		return -1;
	}
	stby_state = 0;
	init_flag = 1;
	return 0;
}



int rc_motor_cleanup(void)
{
	int i;
	if(!init_flag) return 0;
	rc_motor_free_spin(0);
	rc_pwm_cleanup(0);
	rc_pwm_cleanup(1);
	rc_pwm_cleanup(2);
	rc_gpio_cleanup(stby_chip, stby_pin);
	for(i=0;i<channels;i++){
		rc_gpio_cleanup(dirA_chip[i],dirA_pin[i]);
		rc_gpio_cleanup(dirB_chip[i],dirB_pin[i]);
	}
	return 0;
}


int rc_motor_standby(int standby_en)
{
	int val;
	if(unlikely(!init_flag)){
		fprintf(stderr,"ERROR in rc_motor_standby, must call rc_motor_init first\n");
		return -1;
	}
	// if use is requesting standby
	if(standby_en){
		// return if already in standby
		if(stby_state) return 0;
		val=0;
		rc_motor_free_spin(0);
	}
	else{
		if(!stby_state) return 0;
		val=1;
	}
	if(unlikely(rc_gpio_set_value(stby_chip, stby_pin,val))){
		fprintf(stderr,"ERROR in rc_motor_standby, unable to write to gpio %d,%d\n", stby_chip, stby_pin);
		return -1;
	}
	stby_state = standby_en;
	return 0;
}


int rc_motor_set(int motor, double duty)
{
	int a,b,i;

	// sanity checks
	if(unlikely(motor<0 || motor>channels)){
		fprintf(stderr,"ERROR in rc_motor_set, motor argument must be between 0 & %d\n", channels);
		return -1;
	}
	if(unlikely(init_flag==0)){
		fprintf(stderr, "ERROR in rc_motor_set, call rc_motor_init first\n");
		return -1;
	}

	// check that the duty cycle is within +-1
	if	(duty > 1.0)	duty = 1.0;
	else if	(duty <-1.0)	duty =-1.0;

	if(motor==0){
		for(i=1;i<=channels;i++){
			if(rc_motor_set(i,duty)==-1) return -1;
		}
		return 0;
	}

	// determine the direction pins to H-bridge
	duty=duty*polarity[motor-1];
	if(duty>=0.0){	a=1; b=0;}
	else{		a=0; b=1; duty=-duty;}

	// set gpio and pwm for that motor
	if(unlikely(rc_gpio_set_value(dirA_chip[motor-1],dirA_pin[motor-1], a))){
		fprintf(stderr,"ERROR in rc_motor_set, failed to write to gpio pin %d,%d\n",dirA_chip[motor-1],dirA_pin[motor-1]);
		return -1;
	}
	if(unlikely(rc_gpio_set_value(dirB_chip[motor-1],dirB_pin[motor-1], b))){
		fprintf(stderr,"ERROR in rc_motor_set, failed to write to gpio pin %d,%d\n",dirB_chip[motor-1],dirB_pin[motor-1]);
		return -1;
	}
	if(unlikely(rc_pwm_set_duty(pwmss[motor-1], pwmch[motor-1], duty))){
		fprintf(stderr,"ERROR in rc_motor_set, failed to write to pwm %d%c\n",pwmss[motor-1], pwmch[motor-1]);
		return -1;
	}
	return 0;
}


int rc_motor_free_spin(int motor)
{
	int i;

	// sanity checks
	if(unlikely(motor<0 || motor>channels)){
		fprintf(stderr,"ERROR in rc_motor_free_spin, motor argument must be between 0 & %d\n", channels);
		return -1;
	}
	if(unlikely(init_flag==0)){
		fprintf(stderr, "ERROR in rc_motor_free_spin, call rc_motor_init first\n");
		return -1;
	}

	// case for all channels
	if(motor==0){
		for(i=1;i<=channels;i++){
			if(rc_motor_free_spin(i)==-1) return -1;
		}
		return 0;
	}

	// set gpio and pwm for that motor
	if(unlikely(rc_gpio_set_value(dirA_chip[motor-1],dirA_pin[motor-1], 0))){
		fprintf(stderr,"ERROR in rc_motor_free_spin, failed to write to gpio pin %d,%d\n",dirA_chip[motor-1],dirA_pin[motor-1]);
		return -1;
	}
	if(unlikely(rc_gpio_set_value(dirB_chip[motor-1],dirB_pin[motor-1], 0))){
		fprintf(stderr,"ERROR in rc_motor_free_spin, failed to write to gpio pin %d,%d\n",dirB_chip[motor-1],dirB_pin[motor-1]);
		return -1;
	}
	if(unlikely(rc_pwm_set_duty(pwmss[motor-1], pwmch[motor-1], 0.0))){
		fprintf(stderr,"ERROR in rc_motor_free_spin, failed to write to pwm %d%c\n",pwmss[motor-1], pwmch[motor-1]);
		return -1;
	}
	return 0;
}


int rc_motor_brake(int motor)
{
	int i;

	// sanity checks
	if(unlikely(motor<0 || motor>channels)){
		fprintf(stderr,"ERROR in rc_motor_brake, motor argument must be between 0 & %d\n", channels);
		return -1;
	}
	if(unlikely(init_flag==0)){
		fprintf(stderr, "ERROR in rc_motor_brake, call rc_motor_init first\n");
		return -1;
	}

	// case for all channels
	if(motor==0){
		for(i=1;i<=channels;i++){
			if(rc_motor_brake(i)==-1) return -1;
		}
		return 0;
	}

	// set gpio and pwm for that motor
	if(unlikely(rc_gpio_set_value(dirA_chip[motor-1],dirA_pin[motor-1], 1))){
		fprintf(stderr,"ERROR in rc_motor_brake, failed to write to gpio pin %d,%d\n",dirA_chip[motor-1],dirA_pin[motor-1]);
		return -1;
	}
	if(unlikely(rc_gpio_set_value(dirB_chip[motor-1],dirB_pin[motor-1], 1))){
		fprintf(stderr,"ERROR in rc_motor_brake, failed to write to gpio pin %d,%d\n",dirB_chip[motor-1],dirB_pin[motor-1]);
		return -1;
	}
	if(unlikely(rc_pwm_set_duty(pwmss[motor-1], pwmch[motor-1], 0.0))){
		fprintf(stderr,"ERROR in rc_motor_brake, failed to write to pwm %d%c\n",pwmss[motor-1], pwmch[motor-1]);
		return -1;
	}
	return 0;
}

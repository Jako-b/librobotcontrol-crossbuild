/**
 * @file pwm.c
 *
 * C pwm interface for Beaglebone boards
 */

#include <stdio.h>
#include <stdlib.h> // for atoi
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <glob.h>
#include <rc/pwm.h>
#include <rc/time.h>

#define MIN_HZ 1
#define MAX_HZ 1000000000
#define MAXBUF 128
#define SYS_DIR "/dev/bone/pwm/%d/%c"

// ocp only used for exporting right now, everything else through SYS_DIR
#define OCP_DIR "/sys/devices/platform/ocp/4830%d000.epwmss/4830%d200.pwm/pwm"
#define OCP_OFFSET 66

#define unlikely(x) __builtin_expect (!!(x), 0)

static int dutyA_fd[3];
static int dutyB_fd[3];
static unsigned int period_ns[3];
static int init_flag[3] = {0,0,0};

static int mode;
static int ssindex[3];

static int __export_channels(int ss)
{
    char buf[MAXBUF];
    int fd;

    // export channel A
    snprintf(buf, sizeof(buf), "/dev/bone/pwm/%d/export", ss);
    fd = open(buf, O_WRONLY);
    if(fd < 0){
        perror("ERROR: cannot open PWM export A");
        return -1;
    }
    write(fd, "0", 2);
    close(fd);

    // export channel B
    snprintf(buf, sizeof(buf), "/dev/bone/pwm/%d/export", ss);
    fd = open(buf, O_WRONLY);
    if(fd < 0){
        perror("ERROR: cannot open PWM export B");
        return -1;
    }
    write(fd, "1", 2);
    close(fd);

    return 0;
}

static int __unexport_channels(int ss)
{
    char buf[MAXBUF];
    int fd;

    // unexport A
    snprintf(buf, sizeof(buf), "/dev/bone/pwm/%d/unexport", ss);
    fd = open(buf, O_WRONLY);
    if(fd >= 0){
        write(fd, "0", 2);
        close(fd);
    }

    // unexport B
    snprintf(buf, sizeof(buf), "/dev/bone/pwm/%d/unexport", ss);
    fd = open(buf, O_WRONLY);
    if(fd >= 0){
        write(fd, "1", 2);
        close(fd);
    }

    return 0;
}


int rc_pwm_init(int ss, int frequency)
{
    int periodA_fd;
    int periodB_fd;
    int enableA_fd;
    int enableB_fd;
    int polarityA_fd;
    int polarityB_fd;
    char buf[MAXBUF];
    int len;

    if(ss<0 || ss>2){
        fprintf(stderr,"ERROR in rc_pwm_init, PWM subsystem must be 0 1 or 2\n");
        return -1;
    }
    if(frequency<MIN_HZ || frequency>MAX_HZ){
        fprintf(stderr,"ERROR in rc_pwm_init, frequency must be between %dHz and %dHz\n", MIN_HZ, MAX_HZ);
        return -1;
    }

    if(__unexport_channels(ss)==-1) return -1;
    if(__export_channels(ss)==-1) return -1;

    // --- FIXED PATHS START HERE ---

    // duty A
    snprintf(buf, sizeof(buf), SYS_DIR "/duty_cycle", ss, 'a');
    dutyA_fd[ss] = open(buf,O_WRONLY);

    if(unlikely(dutyA_fd[ss]==-1)){
        rc_usleep(600000);
        dutyA_fd[ss] = open(buf,O_WRONLY);
        if(unlikely(dutyA_fd[ss]==-1)){
            perror("ERROR opening duty_cycle A");
            fprintf(stderr,"tried accessing: %s\n", buf);
            return -1;
        }
    }

    // duty B
    snprintf(buf, sizeof(buf), SYS_DIR "/duty_cycle", ss, 'b');
    dutyB_fd[ss] = open(buf,O_WRONLY);
    if(unlikely(dutyB_fd[ss]==-1)){
        perror("ERROR opening duty_cycle B");
        fprintf(stderr,"tried accessing: %s\n", buf);
        return -1;
    }

    // enable A
    snprintf(buf, sizeof(buf), SYS_DIR "/enable", ss, 'a');
    enableA_fd = open(buf,O_WRONLY);
    if(unlikely(enableA_fd==-1)){
        perror("ERROR opening enable A");
        return -1;
    }

    // enable B
    snprintf(buf, sizeof(buf), SYS_DIR "/enable", ss, 'b');
    enableB_fd = open(buf,O_WRONLY);
    if(unlikely(enableB_fd==-1)){
        perror("ERROR opening enable B");
        return -1;
    }

    // period A
    snprintf(buf, sizeof(buf), SYS_DIR "/period", ss, 'a');
    periodA_fd = open(buf,O_WRONLY);
    if(unlikely(periodA_fd==-1)){
        perror("ERROR opening period A");
        return -1;
    }

    // period B
    snprintf(buf, sizeof(buf), SYS_DIR "/period", ss, 'b');
    periodB_fd = open(buf,O_WRONLY);
    if(unlikely(periodB_fd==-1)){
        perror("ERROR opening period B");
        return -1;
    }

    // polarity A
    snprintf(buf, sizeof(buf), SYS_DIR "/polarity", ss, 'a');
    polarityA_fd = open(buf,O_WRONLY);
    if(unlikely(polarityA_fd==-1)){
        perror("ERROR opening polarity A");
        return -1;
    }

    // polarity B
    snprintf(buf, sizeof(buf), SYS_DIR "/polarity", ss, 'b');
    polarityB_fd = open(buf,O_WRONLY);
    if(unlikely(polarityB_fd==-1)){
        perror("ERROR opening polarity B");
        return -1;
    }

    // --- FIXED PATHS END HERE ---

    period_ns[ss] = 1000000000/frequency;
    len = snprintf(buf, sizeof(buf), "%d", period_ns[ss]);
    write(periodA_fd, buf, len);
    write(periodB_fd, buf, len);

    write(polarityA_fd, "normal", 7);
    write(polarityB_fd, "normal", 7);

    write(dutyA_fd[ss], "0", 2);
    write(dutyB_fd[ss], "0", 2);

    write(enableA_fd, "1", 2);
    write(enableB_fd, "1", 2);

    close(enableA_fd);
    close(enableB_fd);
    close(periodA_fd);
    close(periodB_fd);
    close(polarityA_fd);
    close(polarityB_fd);

    init_flag[ss] = 1;
    return 0;
}

int rc_pwm_cleanup(int ss)
{
    int enableA_fd;
    int enableB_fd;
    char buf[MAXBUF];

    if(unlikely(ss<0 || ss>2)){
        fprintf(stderr,"ERROR in rc_pwm_close, subsystem must be between 0 and 2\n");
        return -1;
    }
    if(init_flag[ss]==0){
        return 0;
    }

    // enable A
    snprintf(buf, sizeof(buf), SYS_DIR "/enable", ss, 'a');
    enableA_fd = open(buf, O_WRONLY);
    if(unlikely(enableA_fd==-1)){
        perror("ERROR in rc_pwm_cleanup, failed to open pwm A enable fd");
        return -1;
    }

    // enable B
    snprintf(buf, sizeof(buf), SYS_DIR "/enable", ss, 'b');
    enableB_fd = open(buf, O_WRONLY);
    if(unlikely(enableB_fd==-1)){
        perror("ERROR in rc_pwm_cleanup, failed to open pwm B enable fd");
        close(enableA_fd);
        return -1;
    }

    // duty auf 0 setzen
    if(unlikely(write(dutyA_fd[ss], "0", 2)==-1)){
        perror("ERROR in rc_pwm_cleanup, failed to write to channel A duty fd");
        return -1;
    }
    if(unlikely(write(dutyB_fd[ss], "0", 2)==-1)){
        perror("ERROR in rc_pwm_cleanup, failed to write to channel B duty fd");
        return -1;
    }

    // Kanäle abschalten
    if(unlikely(write(enableA_fd, "0", 2)==-1)){
        perror("ERROR in rc_pwm_cleanup, failed to disable channel A");
        return -1;
    }
    if(unlikely(write(enableB_fd, "0", 2)==-1)){
        perror("ERROR in rc_pwm_cleanup, failed to disable channel B");
        return -1;
    }

    close(enableA_fd);
    close(enableB_fd);
    close(dutyA_fd[ss]);
    close(dutyB_fd[ss]);

    __unexport_channels(ss);

    init_flag[ss] = 0;
    return 0;
}


int rc_pwm_set_duty(int ss, char ch, double duty)
{
	int len, ret, duty_ns;
	char buf[MAXBUF];

	// sanity checks
	if(unlikely(ss<0 || ss>2)){
		fprintf(stderr,"ERROR in rc_pwm_set_duty, PWM subsystem must be between 0 and 2\n");
		return -1;
	}
	if(unlikely(init_flag[ss]==0)){
		fprintf(stderr, "ERROR in rc_pwm_set_duty, subsystem %d not initialized yet\n", ss);
		return -1;
	}
	if(unlikely(duty > 1.0 || duty<0.0)){
		fprintf(stderr,"ERROR in rc_pwm_set_duty, duty must be between 0.0f & 1.0f\n");
		return -1;
	}

	// set the duty
	duty_ns = duty*period_ns[ss];
	len = snprintf(buf, sizeof(buf), "%d", duty_ns);
	switch(ch){
	case 'A':
		ret=write(dutyA_fd[ss], buf, len);
		break;
	case 'B':
		ret=write(dutyB_fd[ss], buf, len);
		break;
	default:
		fprintf(stderr,"ERROR in rc_pwm_set_duty_ns, pwm channel must be 'A' or 'B'\n");
		return -1;
	}

	// make sure write worked
	if(unlikely(ret==-1)){
		perror("ERROR in rc_pwm_set_duty_ns, failed to write to duty_cycle fd");
		return -1;
	}
	return 0;
}


int rc_pwm_set_duty_ns(int ss, char ch, unsigned int duty_ns)
{
	int len, ret;
	char buf[MAXBUF];

	// sanity checks
	if(unlikely(ss<0 || ss>2)){
		fprintf(stderr,"ERROR in rc_pwm_set_duty_ns, PWM subsystem must be between 0 and 2\n");
		return -1;
	}
	if(unlikely(init_flag[ss]==0)){
		fprintf(stderr, "ERROR in rc_pwm_set_duty_ns, subsystem %d not initialized yet\n", ss);
		return -1;
	}
	if(unlikely(duty_ns>period_ns[ss])){
		fprintf(stderr,"ERROR in rc_pwm_set_duty_ns, duty must be between 0 & %d for current frequency\n", period_ns[ss]);
		return -1;
	}

	// set the duty
	len = snprintf(buf, sizeof(buf), "%d", duty_ns);
	switch(ch){
	case 'A':
		ret=write(dutyA_fd[ss], buf, len);
		break;
	case 'B':
		ret=write(dutyB_fd[ss], buf, len);
		break;
	default:
		fprintf(stderr,"ERROR in rc_pwm_set_duty_ns, pwm channel must be 'A' or 'B'\n");
		return -1;
	}

	// make sure write worked
	if(unlikely(ret==-1)){
		perror("ERROR in rc_pwm_set_duty_ns, failed to write to duty_cycle fd");
		return -1;
	}
	return 0;
}

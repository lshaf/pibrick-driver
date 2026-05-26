#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <termios.h>

static int power_key_state = 0;
static long power_key_start = 0;
static long power_key_update = 0;
static int uk_fd=-1;

static inline long atick() {
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now)) {
    return 0;
  }
  return ((long)(now.tv_sec * 1000 + now.tv_nsec / 1000000));
}

int uk_init(){
    system("rmmod gpio_keys");
    system("rmmod hyn_ts");
    system("modprobe hyn_ts");

    uk_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(uk_fd < 0){
        printf("Error opening /dev/uinput: %s\n", strerror(errno));
        return -1;
    }
    if(ioctl(uk_fd, UI_SET_EVBIT, EV_KEY) < 0){
        printf("Error setting EV_KEY: %s\n", strerror(errno));
        return -1;
    }
    printf("UI_SET_EVBIT OK\n");
    if(ioctl(uk_fd, UI_SET_KEYBIT, KEY_POWER) < 0){
        printf("Error setting KEY_POWER: %s\n", strerror(errno));
        return -1;
    }
    printf("UI_SET_KEYBIT OK\n");
    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "pibrickbtn");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;
    if(write(uk_fd, &uidev, sizeof(uidev)) < 0){
        printf("Error writing uinput_user_dev: %s\n", strerror(errno));
        return -1;
    }
    printf("uinput_user_dev OK\n");
    if(ioctl(uk_fd, UI_DEV_CREATE) < 0){
        printf("Error creating uinput device: %s\n", strerror(errno));
        return -1;
    }
    printf("UI_DEV_CREATE OK\n");
    
    return 0;
}
void uk_close(){
    if (uk_fd>=0){
        close(uk_fd);
        uk_fd=-1;
    }
}
int uk_send_key(int keycode, int keystate){
    if(uk_fd < 0){
        return -1;
    }
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = keystate;
    if(write(uk_fd, &ev, sizeof(struct input_event)) < 0){
        return -1;
    }
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    if(write(uk_fd, &ev, sizeof(struct input_event)) < 0){
        return -1;
    }
    return 0;
}

int exec_gpiomon(const char* command){
    FILE *fp;
    char buffer[15];
    fp = popen(command, "r");
    if (fp == NULL) {
        return -1;
    }
    int ret=-1;
    if (fgets(buffer, 15, fp) != NULL) {
        ret = (buffer[0]=='0') ? 0 : 1;
    }
    if (pclose(fp) == -1) {
        return -1;
    }
    return ret;
}

int gpiosel_btn(){
    return exec_gpiomon("gpioget --numeric -c gpiochip10 20");
}

int wait_release(){
    int rc = system("timeout 0.7 gpiomon --edges=rising --bias=pull-up --num-events=1 -c gpiochip0 23 >/dev/null 2>&1");
    return (rc == 0);
}

int monitor_keydown(){
    int state = exec_gpiomon("gpiomon --edges=falling --bias=pull-up --num-events=1 -c gpiochip0 23");
    if (state == 1){
        int sel = gpiosel_btn();
        if (sel){
            printf("POWER PRESS\n");
            system("bash /etc/pibrick/power-short.sh");
        }
        else{
            int released = wait_release();
            if (released){
                printf("USER SHORT KEYUP\n");
                system("bash /etc/pibrick/user-short.sh");
            }
            else{
                printf("USER LONG KEYUP\n");
                system("bash /etc/pibrick/user-long.sh");
            }
        }
        usleep(150000);
        return -1;
    }
    return 0;
}

int main() {

    setvbuf(stdout, NULL, _IONBF, 0);

    uk_init();
    while(1){
        monitor_keydown();
    }
    uk_close();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char state[2];
int main(){
    int file_desc;
    file_desc = open("/dev/simple_gpio_device", O_RDWR);
    while(1){
        int number;
        printf("Press enter");
        getchar();
	    read(file_desc, state, 2);
        printf("Switch 1=%d, Switch 2=%d\n",state[0],state[1]);
    }
    return 0;
}
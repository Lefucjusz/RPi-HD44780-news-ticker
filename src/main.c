/* Created by Lefucjusz, 01.09.2022, Gdańsk */
#include "GPIO.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    volatile unsigned int* const gpio = gpio_init();
   
    if(gpio == NULL) {
        printf("Failed to initialize GPIO! Errno: %d\n", errno);
        return -1;
    }

    printf("Apparently it works!\n");


    gpio_deinit();
    return 0;
}
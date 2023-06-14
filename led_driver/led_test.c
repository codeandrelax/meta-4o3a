#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define NUM_LEDS 26

int main() {
    int ledFile = open("/dev/tlc59731", O_WRONLY);

    if (ledFile == -1) {
        perror("Failed to open the LED driver file");
        return 1;
    }

    // Example values
    int indices[NUM_LEDS] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
    int reds[NUM_LEDS];
    int greens[NUM_LEDS];
    int blues[NUM_LEDS];

    // Seed the random number generator
    srand(time(NULL));

    // Generate random values for reds, greens, and blues arrays
    for (int i = 0; i < NUM_LEDS; i++) {
        reds[i] = rand() % 256;
        greens[i] = rand() % 256;
        blues[i] = rand() % 256;
    }

    // Write the values for each LED to the LED driver file
    for (int i = 0; i < NUM_LEDS; i++) {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%d %d %d %d\n", indices[i], reds[i], greens[i], blues[i]);

        ssize_t bytesWritten = write(ledFile, buffer, strlen(buffer));

        if (bytesWritten == -1) {
            perror("Failed to write to the LED driver file");
            close(ledFile);
            return 1;
        }
        usleep(500000);
    }

    close(ledFile);

    return 0;
}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_LEDS 26

int main() {
    int ledFile = open("/dev/leddriver", O_WRONLY);
    
    if (ledFile == -1) {
        perror("Failed to open the LED driver file");
        return 1;
    }
    
    // Example values
    int indices[NUM_LEDS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
    int reds[NUM_LEDS] = {255, 0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24};
    int greens[NUM_LEDS] = {0, 255, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24};
    int blues[NUM_LEDS] = {128, 0, 255, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24};
    
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
        usleep(500 000);
    }
    
    close(ledFile);
    
    return 0;
}

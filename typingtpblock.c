#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define KEYBOARD_DEV "/dev/input/by-id/[YOURDEVICE]"
#define DISABLE_CMD "hyprctl keyword \"device[YOURDEVICEID]:enabled\" false >/dev/null 2>&1"
#define ENABLE_CMD  "hyprctl keyword \"device[YOURDEVICEID]:enabled\" true >/dev/null 2>&1"
#define TIMEOUT_MS 350

int main() {
    system(ENABLE_CMD);
    
    int fd = open(KEYBOARD_DEV, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Failed to open keyboard device");
        return 1;
    }
    
    struct input_event ev;
    int touchpad_disabled = 0;
    struct timespec last_keypress = {0, 0};
    
    while (1) {
        // Non-blocking read
        ssize_t r = read(fd, &ev, sizeof(ev));
        
        if (r == sizeof(ev)) {
            if (ev.type == EV_KEY && ev.value == 1) {
                if (!touchpad_disabled) {
                    system(DISABLE_CMD);
                    touchpad_disabled = 1;
                }
                clock_gettime(CLOCK_MONOTONIC, &last_keypress);
            }
        } else if (r < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("Read error");
                close(fd);
                
                // Try to reopen the device
                sleep(1);
                fd = open(KEYBOARD_DEV, O_RDONLY | O_NONBLOCK);
                if (fd < 0) {
                    perror("Failed to reopen keyboard device");
                    return 1;
                }
            }
        } else if (r == 0) {
            fprintf(stderr, "Device disconnected, attempting to reconnect...\n");
            close(fd);
            sleep(1);
            fd = open(KEYBOARD_DEV, O_RDONLY | O_NONBLOCK);
            if (fd < 0) {
                perror("Failed to reopen keyboard device");
                return 1;
            }
        }
        
        if (touchpad_disabled) {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            long elapsed_ms = (now.tv_sec - last_keypress.tv_sec) * 1000 +
                              (now.tv_nsec - last_keypress.tv_nsec) / 1000000;
            
            if (elapsed_ms >= TIMEOUT_MS) {
                system(ENABLE_CMD);
                touchpad_disabled = 0;
            }
        }
        
        usleep(10000); // 10ms
    }
    
    close(fd);
    return 0;
}
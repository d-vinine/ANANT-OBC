#include <stdio.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
/* You are not limited to these headers. You are free to add as many headers as you want depending on your solution. */

/* TODO 1: Figure out and define the pins for GPIO in a BeagleBone Black. (Use the datasheet). Replace the "--" with the correct value. */

#define GPIO_CLK 30 // set to mode 7 
#define GPIO_DAT 60 // set to mode 7
#define SYSFS_GPIO_DIR "/sys/class/gpio"

int sysfs_write_file(char *path, char *value) 
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) 
    {
        printf("Error opening %s\n", path);
        return -1;
    }
    write(fd, value, strlen(value));
    close(fd);
    return 0;
}

void gpio_export(int gpio) 
{
    char buffer[50];
    char path[50];
    
    sprintf(path, SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    if (access(path, F_OK) != -1) return; 

    int fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    sprintf(buffer, "%d", gpio);
    write(fd, buffer, strlen(buffer));
    close(fd);
}

void gpio_direction(int gpio, char *dir) 
{
    char path[50];
    sprintf(path, SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
    sysfs_write_file(path, dir);
}

void gpio_write(int gpio, int value) 
{
    char path[50];
    sprintf(path, SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    
    if (value) 
        sysfs_write_file(path, "1");
    else 
        sysfs_write_file(path, "0");
}

/* TODO 2: Write a function for simultaneously converting the 8 bits of the signal to Gray Code. */
/* Your function goes here (no function header provided) */
void encode_gray_code(unsigned char *result, unsigned char data) {
    unsigned char *shared_bits = mmap(NULL, 8, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    for (int i = 0; i < 8; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            if (i == 7) {
                shared_bits[i] = (data >> 7) & 1;
            } else {
                shared_bits[i] = ((data >> i) & 1) ^ ((data >> (i + 1)) & 1);
            }
            _exit(0);
        }
    }

    for (int i = 0; i < 8; i++) {
        wait(NULL);
    }

    unsigned char final_gray = 0;
    for (int i = 0; i < 8; i++) {
        final_gray |= (shared_bits[i] << i);
    }

    *result = final_gray;

    munmap(shared_bits, 8);
}

/* TODO 3: Complete the logic for sending the data */
void driver_send_byte(unsigned char data) 
{
  unsigned char gray;
  encode_gray_code(&gray, data);

  for (int i = 7; i >=0; i--) {
    gpio_write(GPIO_DAT, (gray >> i)&1);
    gpio_write(GPIO_CLK, 1);
    gpio_write(GPIO_CLK, 0);
  }
}


int main() 
{
    gpio_export(GPIO_CLK);
    gpio_export(GPIO_DAT);
    
    gpio_direction(GPIO_CLK, "out");
    gpio_direction(GPIO_DAT, "out");

   /* TODO 4: Complete the main() function. */
    unsigned char data = 0b11100111;
    driver_send_byte(data);
   
    return 0;
}

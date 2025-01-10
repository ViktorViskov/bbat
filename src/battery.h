#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int get_battery_level(char * device) {
    // clear buffer
    char buffer[16];
    memset(buffer, 0, sizeof(buffer));

    // open file for reading
    FILE *file = fopen(device, "r");
    if (file == NULL) {
        return -1;
    }

    // read file
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        return -1;
    } 

    else {
        // remove newline character
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }

    fclose(file);

    // convert string number to integer
    return atoi(buffer);
}

int get_battery_status(char * device) {
    // clear buffer
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));

    // open file for reading
    FILE *file = fopen(device, "r");
    if (file == NULL) {
        return -1;
    }

    // read file
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        return -1;
    } 

    else {
        // remove newline character
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }

    fclose(file);

    // check for charging status
    if (strcmp(buffer, "Charging") == 0) {
        return 1;
    }
    else {
        return 0;
    }
}
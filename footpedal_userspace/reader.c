#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

int min(int a, int b){
    return a < b ? a : b;
}

char buf[100], buf2[200];

FILE* output;

char* strs[] = {
    "0",
    "1"
};

void set_state(bool b){
    //output = fopen("/dev/shm/footpedal", "w+");
    //fprintf(output, b ? "1" : "0");
    rewind(output);
    fwrite(strs[b], strlen(strs[b]), 1, output);
    fflush(output);
    printf("flush\n");
    //fclose(output);
}

int main(){
    FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("./get_hiddevices.sh", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    int num_chars = 0;

    char min_name[100];
    bool set = false;

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path), fp) != NULL) {
        sscanf(path, "%100s %n", buf, &num_chars);
        strncpy(buf2, path + num_chars, min(200, strlen(path) - num_chars));
        
        //printf("hidraw: %s\n", buf);
        //printf("name: %s\n", buf2);

        // possible names are:
        // PCsensor FootSwitch
        // QinHeng Electronics FootSwitch
        if(strstr(buf2, "FootSwitch") != NULL){
            if(!set || strcmp(buf, min_name) < 0){
                strcpy(min_name, buf);
                set = true;
            }
        }
    }

    /* close */
    pclose(fp);

    printf("best: %s\n", min_name);

    char target[106];

    sprintf(target, "/dev/%s", min_name);

    printf("target file: %s\n", target);
    FILE* device = fopen(target, "r");

    if(device == NULL){
        printf("Error: device is null! Make sure you are running this program in root mode (sudo).\n");
        return 0;
    }

    char stdin_buf[20];
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    int curr_buf[9];

    output = fopen("/dev/shm/footpedal", "w+");

    // assume that the pedal is not being pressed when this
    // program is started
    set_state(0);

    while(true){
        for(int i = 0; i < 9; i++)
            curr_buf[i] = fgetc(device);

        //rewind(output);
        //fseek(output, 0L, SEEK_SET); // move the offset back to start of file
        
        if(curr_buf[3] == 5){
            set_state(1);
            //fprintf(output, "1");
            printf("press\n");
        }else{
            set_state(0);
            //fprintf(output, "0");
            printf("release\n");
        }
        //fclose(output);

        int numRead = read(0, stdin_buf, 4);
        if (numRead > 0) {
            break;
        }
    }

    fclose(device);
    fclose(output);

    return 0;
}
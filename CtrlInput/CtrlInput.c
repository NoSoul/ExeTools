#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ToSring(x) #x

char *GetPath(char *line)
{
    int i;
    for(i = 0; line[i] != '\0'; ++i) {
        if(i && line[i - 1] == ' ') {
            return line + i;
        }
    }
    return NULL;
}

void ReadLine(char *s, int size, FILE *stream)
{
    fgets(s, size, stream);
    int i;
    for(i = 0; s[i] != '\n'; ++i);
    s[i] = '\0';
}

int OpenDevice(const char *path, int oflag)
{
    int fd = open(path, oflag);
    if(fd == -1) {
        printf("Can not open device %s\n", path);
    }
    return fd;
}

unsigned short GetKeyCode(char *s)
{
    if(strcmp(s, ToSring(KEY_ESC)) == 0) {
        return KEY_ESC;
    }
    if(strcmp(s, ToSring(KEY_LEFTCTRL)) == 0) {
        return KEY_LEFTCTRL;
    }
    if(strcmp(s, ToSring(BTN_LEFT)) == 0) {
        return BTN_LEFT;
    }
    if(strcmp(s, ToSring(BTN_RIGHT)) == 0) {
        return BTN_RIGHT;
    }

    if(strcmp(s, ToSring(KEY_0)) == 0) {
        return KEY_0;
    }
    if(strcmp(s, ToSring(KEY_1)) == 0) {
        return KEY_1;
    }
    if(strcmp(s, ToSring(KEY_2)) == 0) {
        return KEY_2;
    }
    if(strcmp(s, ToSring(KEY_3)) == 0) {
        return KEY_3;
    }
    if(strcmp(s, ToSring(KEY_4)) == 0) {
        return KEY_4;
    }
    if(strcmp(s, ToSring(KEY_5)) == 0) {
        return KEY_5;
    }
    if(strcmp(s, ToSring(KEY_6)) == 0) {
        return KEY_6;
    }
    if(strcmp(s, ToSring(KEY_7)) == 0) {
        return KEY_7;
    }
    if(strcmp(s, ToSring(KEY_8)) == 0) {
        return KEY_8;
    }
    if(strcmp(s, ToSring(KEY_9)) == 0) {
        return KEY_9;
    }
    if(strcmp(s, ToSring(KEY_F1)) == 0) {
        return KEY_F1;
    }
    if(strcmp(s, ToSring(KEY_F2)) == 0) {
        return KEY_F2;
    }
    if(strcmp(s, ToSring(KEY_F3)) == 0) {
        return KEY_F3;
    }
    if(strcmp(s, ToSring(KEY_F4)) == 0) {
        return KEY_F4;
    }
    if(strcmp(s, ToSring(KEY_F5)) == 0) {
        return KEY_F5;
    }
    if(strcmp(s, ToSring(KEY_F6)) == 0) {
        return KEY_F6;
    }
    if(strcmp(s, ToSring(KEY_F7)) == 0) {
        return KEY_F7;
    }
    if(strcmp(s, ToSring(KEY_F8)) == 0) {
        return KEY_F8;
    }
    if(strcmp(s, ToSring(KEY_F9)) == 0) {
        return KEY_F9;
    }
    if(strcmp(s, ToSring(KEY_F10)) == 0) {
        return KEY_F10;
    }
    if(strcmp(s, ToSring(KEY_F11)) == 0) {
        return KEY_F11;
    }
    if(strcmp(s, ToSring(KEY_F12)) == 0) {
        return KEY_F12;
    }
    if(strcmp(s, ToSring(KEY_A)) == 0) {
        return KEY_A;
    }
    if(strcmp(s, ToSring(KEY_B)) == 0) {
        return KEY_B;
    }
    if(strcmp(s, ToSring(KEY_C)) == 0) {
        return KEY_C;
    }
    if(strcmp(s, ToSring(KEY_D)) == 0) {
        return KEY_D;
    }
    if(strcmp(s, ToSring(KEY_E)) == 0) {
        return KEY_E;
    }
    if(strcmp(s, ToSring(KEY_F)) == 0) {
        return KEY_F;
    }
    if(strcmp(s, ToSring(KEY_G)) == 0) {
        return KEY_G;
    }
    if(strcmp(s, ToSring(KEY_H)) == 0) {
        return KEY_H;
    }
    if(strcmp(s, ToSring(KEY_I)) == 0) {
        return KEY_I;
    }
    if(strcmp(s, ToSring(KEY_J)) == 0) {
        return KEY_J;
    }
    if(strcmp(s, ToSring(KEY_K)) == 0) {
        return KEY_K;
    }
    if(strcmp(s, ToSring(KEY_L)) == 0) {
        return KEY_L;
    }
    if(strcmp(s, ToSring(KEY_M)) == 0) {
        return KEY_M;
    }
    if(strcmp(s, ToSring(KEY_N)) == 0) {
        return KEY_N;
    }
    if(strcmp(s, ToSring(KEY_O)) == 0) {
        return KEY_O;
    }
    if(strcmp(s, ToSring(KEY_P)) == 0) {
        return KEY_P;
    }
    if(strcmp(s, ToSring(KEY_Q)) == 0) {
        return KEY_Q;
    }
    if(strcmp(s, ToSring(KEY_R)) == 0) {
        return KEY_R;
    }
    if(strcmp(s, ToSring(KEY_S)) == 0) {
        return KEY_S;
    }
    if(strcmp(s, ToSring(KEY_T)) == 0) {
        return KEY_T;
    }
    if(strcmp(s, ToSring(KEY_U)) == 0) {
        return KEY_U;
    }
    if(strcmp(s, ToSring(KEY_V)) == 0) {
        return KEY_V;
    }
    if(strcmp(s, ToSring(KEY_W)) == 0) {
        return KEY_W;
    }
    if(strcmp(s, ToSring(KEY_X)) == 0) {
        return KEY_X;
    }
    if(strcmp(s, ToSring(KEY_Y)) == 0) {
        return KEY_Y;
    }
    if(strcmp(s, ToSring(KEY_Z)) == 0) {
        return KEY_Z;
    }
    return KEY_RESERVED;
}

void WriteKey(int fd, int value, unsigned short code)
{
    struct input_event event;
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.value = value;
    event.code = code;
    write(fd, &event, sizeof(event));
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    write(fd, &event, sizeof(event));
}

void WriteMove(int fd, int x, int y)
{
    struct input_event event;
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.value = x;
    event.code = REL_X;
    write(fd, &event, sizeof(event));
    event.type = EV_REL;
    event.value = y;
    event.code = REL_Y;
    write(fd, &event, sizeof(event));
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    write(fd, &event, sizeof(event));
}

int main(int argc, char *argv[])
{
    int maxcurLineLen = 1000000;
    char *curLine = (char*)malloc(maxcurLineLen * sizeof(char));
    unsigned short *keyStatck = (unsigned short*)malloc(maxcurLineLen * sizeof(unsigned short));
    if(argc != 2) {
        printf("Usage: %s ConfigFile\n", argv[0]);
        return 1;
    }
    FILE *fr = fopen(argv[1], "r");
    if(fr == NULL) {
        printf("Can not open ConfigFile %s\n", argv[1]);
        free(curLine);
        free(keyStatck);
        return 1;
    }
    char *strPtr;
    ReadLine(curLine, maxcurLineLen, fr);
    strPtr = GetPath(curLine);
    int kFd = OpenDevice(strPtr, O_WRONLY);
    if(kFd == -1) {
        free(curLine);
        free(keyStatck);
        return 1;
    }
    ReadLine(curLine, maxcurLineLen, fr);
    strPtr = GetPath(curLine);
    int mFd = OpenDevice(strPtr, O_WRONLY);
    if(mFd == -1) {
        close(kFd);
        free(curLine);
        free(keyStatck);
        return 1;
    }
    unsigned int times;
    while(fscanf(fr, "%u ", &times) == 1) {
        ReadLine(curLine, maxcurLineLen, fr);
        printf("exec %s\n", curLine);
        unsigned int stackSize = 0;
        if(strncmp(curLine, "REL", 3) == 0) {
            int x, y;
            sscanf(curLine, "%*s %d %d", &x, &y);
            while(times--) {
                WriteMove(mFd, x, y);
            }
        } else {
            strPtr = strtok(curLine, " ");
            if(strPtr) {
                keyStatck[stackSize++] = GetKeyCode(strPtr);
            }
            while(1) {
                strPtr = strtok(NULL, " ");
                if(strPtr == NULL) {
                    break;
                }
                keyStatck[stackSize++] = GetKeyCode(strPtr);
            }
            while(times--) {
                unsigned int i;
                for(i = 0; i < stackSize; ++i) {
                    WriteKey(keyStatck[i] >= BTN_MISC ? mFd : kFd, 1, keyStatck[i]);
                }
                if(stackSize) {
                    for(i = stackSize - 1; i > 0; --i) {
                        WriteKey(keyStatck[i] >= BTN_MISC ? mFd : kFd, 0, keyStatck[i]);
                    }
                    WriteKey(keyStatck[i] >= BTN_MISC ? mFd : kFd, 0, keyStatck[0]);
                }
            }
        }
        sleep(1);
    }
    close(mFd);
    close(kFd);
    fclose(fr);
    free(curLine);
    free(keyStatck);
    return 0;
}

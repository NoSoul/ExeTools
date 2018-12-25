#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define ERROR_EXIT          exit(1)
#define TEMP_FILE_NAME      "_ArchvingCFTemp"
#define TEMP_LEN            1024*1024
char g_Temp[TEMP_LEN];

void MySystem(const char *cmdstring)
{
    int status = system(cmdstring);
    if(status == -1) {
        fprintf(stderr, "Can not use system, errno: %d, %s!\n", errno, strerror(errno));
        ERROR_EXIT;
    } else {
        if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) != 0) {
                fprintf(stderr, "%s failed!, return [0x%x]!\n", cmdstring, WEXITSTATUS(status));
                ERROR_EXIT;
            }
        } else {
            fprintf(stderr, "Can not use %s, return [0x%x]!\n", cmdstring, WIFEXITED(status));
            ERROR_EXIT;
        }
    }
}

int GetMaxContestNo()
{
    int ret = 0;
    FILE *fr = popen("ls *.c", "r");
    char fileName[101];
    while(fscanf(fr, "%s", fileName) != EOF) {
        int contestNo = 0;
        for(int i = 0; fileName[i]; ++i) {
            if(fileName[i] >= '0' && fileName[i] <= '9') {
                contestNo *= 10;
                contestNo += fileName[i] - '0';
            } else {
                break;
            }
        }
        if(ret < contestNo) {
            ret = contestNo;
        }
    }
    pclose(fr);
    return ret;
}

int main()
{
    int maxContestNo = GetMaxContestNo();
    printf("maxContestNo: %d\n", maxContestNo);
    char suffix[][16] = {"[A-Z].c", "[A-Z][0-9].c"};
    for(int i = 1; i <= maxContestNo; ++i) {
        int count;
        sprintf(g_Temp, "ls ./*c | awk '/\\<%d%s/||/\\<%d%s/' | wc -l", i, suffix[0], i, suffix[1]);
        FILE *fr = popen(g_Temp, "r");
        while(fscanf(fr, "%d", &count) != EOF);
        pclose(fr);
        bool update = false;
        char contestName[TEMP_LEN];
        if(count >= 2) {
            sprintf(g_Temp, "wget -q http://codeforces.com/contest/%d -O %s", i, TEMP_FILE_NAME);
            MySystem(g_Temp);
            sprintf(g_Temp, "awk '/color: black/' %s | cut -d '>' -f 3  | cut -d '<' -f 1", TEMP_FILE_NAME);
            FILE *fr = popen(g_Temp, "r");
            while(fgets(contestName, sizeof(contestName), fr) != NULL);
            fclose(fr);
            for(int j = 0; contestName[j]; ++j) {
                if(contestName[j] == '\n') {
                    contestName[j] = '\0';
                    break;
                }
            }
            sprintf(g_Temp, "mkdir -p \"%s\"", contestName);
            MySystem(g_Temp);
            printf("Arichiving %d* %d file to %s\n", i, count, contestName);
            update = true;
        } else if(count == 1) {
            sprintf(g_Temp, "find . -name \"*.c\" | awk '/\\<%d%s/||/\\<%d%s/' | wc -l", i, suffix[0], i, suffix[1]);
            fr = popen(g_Temp, "r");
            while(fscanf(fr, "%d", &count) != EOF);
            pclose(fr);
            if(count >= 3) {
                sprintf(g_Temp, "find . -mindepth 2 -name \"*.c\" | awk '/\\<%d%s/||/\\<%d%s/' | tail -n 1 | cut -d '/' -f 2", i, suffix[0], i, suffix[1]);
                fr = popen(g_Temp, "r");
                char contestName[TEMP_LEN];
                while(fgets(contestName, sizeof(contestName), fr) != NULL);
                fclose(fr);
                for(int j = 0; contestName[j]; ++j) {
                    if(contestName[j] == '\n') {
                        contestName[j] = '\0';
                        break;
                    }
                }
                printf("Arichiving %d* 1 file to %s\n", i, contestName);
                update = true;
            }
        }
        if(update) {
            for(int j = 0; j < 2; ++j) {
                sprintf(g_Temp, "ls ./*c | awk '/\\<%d%s/' | wc -l", i, suffix[j]);
                fr = popen(g_Temp, "r");
                while(fscanf(fr, "%d", &count) != EOF);
                pclose(fr);
                if(count >= 1) {
                    sprintf(g_Temp, "mv %d%s \"%s\"", i, suffix[j], contestName);
                    MySystem(g_Temp);
                }
            }
        }
    }
    sprintf(g_Temp, "rm -f %s", TEMP_FILE_NAME);
    MySystem(g_Temp);
    return 0;
}

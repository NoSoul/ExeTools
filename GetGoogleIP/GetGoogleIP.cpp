#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
using namespace std;

sem_t g_SemRes;
pthread_spinlock_t g_Mutex;
vector<unsigned int> g_IPSet;

unsigned char Get(unsigned int ip, int idx)
{
    return ((ip & (0x00ff << (8 * idx))) >> (8 * idx));
}

int my_system(const char * cmdstring)
{
    pid_t pid;
    int status;
    if(cmdstring == NULL) {
        return 1;
    }
    if((pid = vfork()) < 0) {
        status = -1;
    } else if(pid == 0) {
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        _exit(127);
    } else {
        while(waitpid(pid, &status, 0) < 0) {
            if(errno != EINTR) {
                status = -1;
                break;
            }
        }
    }
    return status;
}

bool Check(const char *Cmd)
{
#ifdef _DEBUGLOG
    puts(Cmd);
#endif
    int status = my_system(Cmd);
    if(status == -1) {
        fprintf(stderr, "Can not use system, errno: %d, %s!!!!\n", errno, strerror(errno));
    } else {
        if(WIFEXITED(status)) {
            return WEXITSTATUS(status) ? false : true;
        } else {
            fprintf(stderr, "Can not use curl, return [0x%x]!!!!\n", WIFEXITED(status));
        }
    }
    return true;
}

bool CheckHTTP(unsigned int ip)
{
    char Cmd[100];
    sprintf(Cmd, "curl -s -I http://%hhu.%hhu.%hhu.%hhu -m 3 --connect-timeout 3 | grep \"200 OK\"", Get(ip, 3), Get(ip, 2), Get(ip, 1), Get(ip, 0));
    return Check(Cmd);
}

bool CheckHTTPS(unsigned int ip)
{
    char Cmd[100];
    sprintf(Cmd, "curl -s -I https://%hhu.%hhu.%hhu.%hhu -m 3 --connect-timeout 3 | grep \"200 OK\"", Get(ip, 3), Get(ip, 2), Get(ip, 1), Get(ip, 0));
    return Check(Cmd);
}

void *Func(void *arg)
{
    while(1) {
        sem_wait(&g_SemRes);
        pthread_spin_lock(&g_Mutex);
        unsigned int testIP = g_IPSet.front();
        g_IPSet.erase(g_IPSet.begin());
        pthread_spin_unlock(&g_Mutex);
        if(CheckHTTP(testIP)) {
            fprintf(stderr, "http://%d.%d.%d.%d\n", Get(testIP, 3), Get(testIP, 2), Get(testIP, 1), Get(testIP, 0));
        }
        if(CheckHTTPS(testIP)) {
            fprintf(stderr, "https://%d.%d.%d.%d\n", Get(testIP, 3), Get(testIP, 2), Get(testIP, 1), Get(testIP, 0));
        }
    }
    return arg;
}

void EnumAllIP(const char *str)
{
    unsigned int ip = 0;
    int num = 0;
    int offset = 24;
    for(int i = 0; str[i] != '\0'; ++i) {
        if(str[i] == '.' || str[i] == '/') {
#ifdef _DEBUGLOG
            printf("%d%c", num, str[i]);
#endif
            ip |= num << offset;
            offset -= 8;
            num = 0;
        } else {
            num *= 10;
            num += str[i] - '0';
        }
    }
#ifdef _DEBUGLOG
    printf("%d\n", num);
#endif
    unsigned int bound = (1 << (31 - num)) - 1;
    for(unsigned int i = 1; i < bound; ++i) {
        unsigned int testIP = ip | i;
        pthread_spin_lock(&g_Mutex);
        g_IPSet.push_back(testIP);
        sem_post(&g_SemRes);
        pthread_spin_unlock(&g_Mutex);
    }
}

int main(int argc, char *argv[])
{
    int threadNums = 1;
    if(argc == 2) {
        int num = atoi(argv[1]);
        if(num != 0) {
            threadNums = num;
        }
    }
    sem_init(&g_SemRes, 0, 0);
    pthread_spin_init(&g_Mutex, 0);
    pthread_t task[threadNums];
    for(int i = 0; i < threadNums; ++i) {
        pthread_create(&task[i], NULL, Func, NULL);
    }
    FILE *fr = popen("nslookup -q=TXT _netblocks.google.com 8.8.8.8 | grep text", "r");
    const int maxTextLen = 10000;
    char strText[maxTextLen];
    char curIPGroup[20];
    int curIPGroupSize;
    while(fgets(strText, maxTextLen, fr)) {
#ifdef _DEBUGLOG
        printf("%s", strText);
#endif
        for(int i = 0; strText[i + 4] != '\0'; ++i) {
            if(strText[i + 4] != '\0' && strText[i] == 'i' && strText[i + 1] == 'p' && strText[i + 2] == '4' && strText[i + 3] == ':') {
                i += 4;
                curIPGroupSize = 0;
                while(strText[i] != ' ') {
                    curIPGroup[curIPGroupSize++] = strText[i++];
                }
                curIPGroup[curIPGroupSize] = '\0';
                EnumAllIP(curIPGroup);
            }
        }
    }
    pclose(fr);
    while(1) {
        if(g_IPSet.size() == 0) {
            break;
        }
        sleep(1);
    }
    for(int i = 0; i < threadNums; ++i) {
        pthread_cancel(task[i]);
    }
    for(int i = 0; i < threadNums; ++i) {
        pthread_join(task[i], NULL);
    }
    sem_destroy(&g_SemRes);
    pthread_spin_destroy(&g_Mutex);
    return 0;
}

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
using namespace std;

#define TARGRT_DIR          "LocalCodeForces"
#define TEMP_FILE_NAME      "_CFATemp"
#define SOLVED_FILE_NAME    "_CFASolvedTemp"
#define ERROR_EXIT          exit(1)
#define SOLVED_DIR          "/home/nosoul/DEV/Git/NoSoul/CodeForces"
#define MAX_GET_THREAD_NUM  10
#define TEMP_LEN            1024*1024
char g_Temp[TEMP_LEN];
sem_t g_SemRes;
pthread_spinlock_t g_Mutex;
typedef struct {
    unsigned m_Div;
    char m_Name[16];
} Problem_t;
vector<Problem_t> g_Set;
typedef struct {
    unsigned char m_Idx;
    char m_Cmd[1024 * 2];
    char m_Content[5 * 1024 * 1024];
    char m_PngPath[1024];
} ProblemRes_t;
bool g_TaskIDLE[MAX_GET_THREAD_NUM];

static int VSystem(const char *cmdstring)
{
    pid_t pid;
    int status;
    if(cmdstring == NULL) {
        return 1;
    }
    if((pid = vfork()) < 0) {
        status = -1;
    } else if(pid == 0) {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
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

void MySystem(const char *cmdstring)
{
    int status = VSystem(cmdstring);
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

int GetNum()
{
    fgets(g_Temp, TEMP_LEN, stdin);
    for(int i = 0; g_Temp[i] != '\n'; ++i) {
        if(!(g_Temp[i] >= '0' && g_Temp[i] <= '9')) {
            return -1;
        }
    }
    int ret = 0;
    for(int i = 0; g_Temp[i] != '\n'; ++i) {
        ret *= 10;
        ret += g_Temp[i] - '0';
    }
    return ret;
}

int GetLevel()
{
    puts("\tlevel = 0: the easiest unsolved problem Number");
    puts("\tlevel = 1: all unsolved problem content");
    puts("\tlevel = 2: all problem content");
    printf("Please input level(0~2): ");
    int ret = GetNum();
    if(ret < 0 || ret > 2) {
        puts("level error");
        ERROR_EXIT;
    }
    return ret;
}

void GetCSS()
{
    puts("Get CSS File...");
    sprintf(g_Temp, "%s/css/problem-statement.css", TARGRT_DIR);
    FILE *fExist = fopen(g_Temp, "r");
    if(fExist == NULL) {
        sprintf(g_Temp, "mkdir -p %s/css", TARGRT_DIR);
        MySystem(g_Temp);
        sprintf(g_Temp, "wget -q http://st.codeforces.com/css/problem-statement.css -O %s/css/problem-statement.css", TARGRT_DIR);
        MySystem(g_Temp);
    } else {
        fclose(fExist);
    }
}

int GetPages()
{
    printf("Get Pages...\n");
    sprintf(g_Temp, "wget -q http://codeforces.com/problemset -O %s", TEMP_FILE_NAME);
    MySystem(g_Temp);
    sprintf(g_Temp, "grep page-index %s | cut -d '\"' -f 4", TEMP_FILE_NAME);
    int ret;
    FILE *fr = popen(g_Temp, "r");
    while(fscanf(fr, "%d", &ret) != EOF);
    pclose(fr);
    printf("There are %d pages\n", ret);
    return ret;
}

int GetStartPageNum(int bound)
{
    printf("Please input start page(1~%d): ", bound);
    int ret = GetNum();
    if(ret <= 0 || ret > bound) {
        puts("start page error");
        ERROR_EXIT;
    }
    return ret;
}

void GetAllSolved()
{
    sprintf(g_Temp, "find %s/ -name \"*.c\" > %s", SOLVED_DIR, SOLVED_FILE_NAME);
    MySystem(g_Temp);
}

void *Func(void *arg)
{
    ProblemRes_t *mem = (ProblemRes_t *)arg;
    while(1) {
        g_TaskIDLE[mem->m_Idx] = true;
        sem_wait(&g_SemRes);
        pthread_spin_lock(&g_Mutex);
        g_TaskIDLE[mem->m_Idx] = false;
        Problem_t cur = g_Set.front();
        g_Set.erase(g_Set.begin());
        pthread_spin_unlock(&g_Mutex);
        printf("Try Get %u%s (http://codeforces.com/problemset/problem/%u/%s)\n", cur.m_Div, cur.m_Name, cur.m_Div, cur.m_Name);
        sprintf(mem->m_Cmd, "wget -q http://codeforces.com/problemset/problem/%u/%s -O %u%s.orig", cur.m_Div, cur.m_Name, cur.m_Div, cur.m_Name);
        MySystem(mem->m_Cmd);
        sprintf(mem->m_Cmd, "grep \\\"ttypography %u%s.orig > %u%s.orig.html", cur.m_Div, cur.m_Name, cur.m_Div, cur.m_Name);
        MySystem(mem->m_Cmd);
        sprintf(mem->m_Cmd, "%u%s.orig.html", cur.m_Div, cur.m_Name);
        FILE *fr = fopen(mem->m_Cmd, "r");
        fgets(mem->m_Content, sizeof(mem->m_Content), fr);
        fclose(fr);
        sprintf(mem->m_Cmd, "%s/problemset/%u%s", TARGRT_DIR, cur.m_Div, cur.m_Name);
        FILE *fw = fopen(mem->m_Cmd, "w");
        fprintf(fw, "<html>\n\t<body>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
        fprintf(fw, "<link rel=\"stylesheet\" href=\"../css/problem-statement.css\" type=\"text/css\" charset=\"utf-8\" />\n\n");
        int len = strlen(mem->m_Content);
        int pngCnt = 0;
        for(int i = 0; i < len; ++i) {
            if(i < len - 17) {
                if(mem->m_Content[i] == 's' && mem->m_Content[i + 1] == 'r' && mem->m_Content[i + 2] == 'c' && mem->m_Content[i + 3] == '=' && mem->m_Content[i + 4] == '"') {
                    fprintf(fw, "src=\"");
                    i += 5;
                    memset(mem->m_PngPath, 0, sizeof(mem->m_PngPath));
                    for(int j = 0; mem->m_Content[i + j] != '"'; ++j) {
                        mem->m_PngPath[j] = mem->m_Content[i + j];
                    }
                    fprintf(fw, "../images/%u%s_%d.png\"", cur.m_Div, cur.m_Name, pngCnt);
                    if(strstr(mem->m_PngPath, "codeforces.com") != NULL) {
                        sprintf(mem->m_Cmd, "wget -q %s -O %s/images/%u%s_%d.png", mem->m_PngPath, TARGRT_DIR, cur.m_Div, cur.m_Name, pngCnt);
                    } else {
                        sprintf(mem->m_Cmd, "wget -q http://codeforces.com%s -O %s/images/%u%s_%d.png", mem->m_PngPath, TARGRT_DIR, cur.m_Div, cur.m_Name, pngCnt);
                    }
                    MySystem(mem->m_Cmd);
                    ++pngCnt;
                    i += strlen(mem->m_PngPath) + 1;
                } else {
                    fprintf(fw, "%c", mem->m_Content[i]);
                }
            } else {
                fprintf(fw, "%c", mem->m_Content[i]);
            }
        }
        fprintf(fw, "\n\t</body>\n</html>");
        fclose(fw);
    }
    return arg;
}

bool UnSolved(Problem_t &cur)
{
    int ret;
    sprintf(g_Temp, "awk '/\\<%u%s.c/' %s | wc -l", cur.m_Div, cur.m_Name, SOLVED_FILE_NAME);
    FILE *fr = popen(g_Temp, "r");
    while(fscanf(fr, "%d", &ret) != EOF);
    pclose(fr);
    return ret == 0;
}

void JudgeGet(Problem_t &cur)
{
    sprintf(g_Temp, "%s/problemset/%u%s", SOLVED_DIR, cur.m_Div, cur.m_Name);
    FILE *fExist = fopen(g_Temp, "r");
    if(fExist != NULL) {
        fclose(fExist);
        return;
    }
    pthread_spin_lock(&g_Mutex);
    g_Set.push_back(cur);
    sem_post(&g_SemRes);
    pthread_spin_unlock(&g_Mutex);
}

bool ParserPageOver(int pageIdx, int level)
{
    printf("Parser Page %d by SolvedDesc (http://codeforces.com/problemset/page/%d?tags=500-5000&order=BY_RATING_ASC)\n", pageIdx, pageIdx);
    sprintf(g_Temp, "wget -q \"http://codeforces.com/problemset/page/%d?tags=500-5000&order=BY_RATING_ASC\" -O %s", pageIdx, TEMP_FILE_NAME);
    MySystem(g_Temp);
    sprintf(g_Temp, "grep problemset/problem %s | cut -d '\"' -f 2 | cut -d '/' -f 4,5", TEMP_FILE_NAME);
    FILE *fr = popen(g_Temp, "r");
    while(fscanf(fr, "%s", g_Temp) != EOF) {
        fscanf(fr, "%s", g_Temp);
        Problem_t cur;
        memset(&cur, 0, sizeof(cur));
        cur.m_Div = 0;
        int i, j;
        for(i = 0; g_Temp[i] != '/'; ++i) {
            cur.m_Div *= 10;
            cur.m_Div += g_Temp[i] - '0';
        }
        for(++i, j = 0; g_Temp[i] != '\0'; ++i, ++j) {
            cur.m_Name[j] = g_Temp[i];
        }
        bool unSolved = UnSolved(cur);
        if(level == 0) {
            if(unSolved) {
                JudgeGet(cur);
                pclose(fr);
                return true;
            }
        } else if(level == 1) {
            if(unSolved) {
                JudgeGet(cur);
            }
        } else {
            JudgeGet(cur);
        }
    }
    pclose(fr);
    return false;
}

int main()
{
    sprintf(g_Temp, "mkdir -p %s/problemset && mkdir -p %s/css && mkdir -p %s/images", TARGRT_DIR, TARGRT_DIR, TARGRT_DIR);
    MySystem(g_Temp);
    int level = GetLevel();
    int threadNum = MAX_GET_THREAD_NUM;
    GetCSS();
    int pages = GetPages();
    int startPage = GetStartPageNum(pages);
    GetAllSolved();

    sem_init(&g_SemRes, 0, 0);
    pthread_spin_init(&g_Mutex, 0);
    ProblemRes_t *mem = (ProblemRes_t *)malloc(threadNum * sizeof(ProblemRes_t));
    pthread_t *task = (pthread_t *)malloc(threadNum * sizeof(pthread_t));
    for(int i = 0; i < threadNum; ++i) {
        mem[i].m_Idx = i;
        pthread_create(&task[i], NULL, Func, &mem[i]);
    }

    for(int i = startPage; i <= pages; ++i) {
        if(ParserPageOver(i, level)) {
            break;
        }
    }

    while(1) {
        bool exit = g_Set.size() == 0;
        for(int i = 0; i < threadNum; ++i) {
            exit &= g_TaskIDLE[i];
        }
        if(exit) {
            break;
        }
        sleep(1);
    }
    for(int i = 0; i < threadNum; ++i) {
        pthread_cancel(task[i]);
    }
    for(int i = 0; i < threadNum; ++i) {
        pthread_join(task[i], NULL);
    }
    free(task);
    free(mem);
    pthread_spin_destroy(&g_Mutex);
    sem_destroy(&g_SemRes);
    sprintf(g_Temp, "rm -f *.html *.orig %s %s", TEMP_FILE_NAME, SOLVED_FILE_NAME);
    MySystem(g_Temp);
    return 0;
}

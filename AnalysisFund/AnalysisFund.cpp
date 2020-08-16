#include <algorithm>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
using namespace std;
#define TEMP_FILE_NAME      "_FundTemp"
#define TEMP_LEN            1024
#define MAX_GET_THREAD_NUM  8
class Fund
{
public:
    char m_Code[7];
    char m_Name[TEMP_LEN];
    char m_Type[128];
    vector<float> m_NianDuRatio;
    vector<int> m_NianDuRankA;
    vector<int> m_NianDuRankB;
    float m_MinRatio;
    float m_MaxRatio;
    float m_AvgRatio;
    void Update()
    {
        m_MinRatio = *m_NianDuRatio.begin();
        m_MaxRatio = *m_NianDuRatio.begin();
        float sum = 1;
        for(auto it : m_NianDuRatio) {
            if(m_MinRatio > it) {
                m_MinRatio = it;
            }
            if(m_MaxRatio < it) {
                m_MaxRatio = it;
            }
            sum *= (1 + it / 100);
        }
        sum /= (1 + m_MaxRatio / 100);
        if(m_NianDuRatio.size() > 1) {
            m_AvgRatio = (pow(sum, 1.0 / (m_NianDuRatio.size() - 1)) - 1) * 100;
        } else {
            m_AvgRatio = 0;
        }
    }
};
bool cmpMin(const Fund &lhs, const Fund &rhs)
{
    return lhs.m_MinRatio > rhs.m_MinRatio;
}
bool cmpAvg(const Fund &lhs, const Fund &rhs)
{
    return lhs.m_AvgRatio > rhs.m_AvgRatio;
}
sem_t g_SemRes;
pthread_spinlock_t g_Mutex;
vector<Fund> g_AllFund;
vector<unsigned> g_Set;
bool g_TaskIDLE[MAX_GET_THREAD_NUM];
char g_FileContent[MAX_GET_THREAD_NUM][TEMP_LEN * TEMP_LEN * 4];
void GetAllFundCode()
{
    printf("%s...", __func__);
    char cmd[TEMP_LEN];
    sprintf(cmd, "wget -q http://fund.eastmoney.com/js/fundcode_search.js -O %s", TEMP_FILE_NAME);
    system(cmd);
    sprintf(cmd, "sed -i 's/\",\"/ \",\" /g' %s", TEMP_FILE_NAME);
    system(cmd);
    sprintf(cmd, "sed -i 's/\"]/ \"]/g' %s", TEMP_FILE_NAME);
    system(cmd);
    FILE *fr = fopen(TEMP_FILE_NAME, "r");
    fscanf(fr, "%*s %*s = [");
    Fund cur;
    while(fscanf(fr, "[\"%s \",\"%*s \",\"%s \",\"%s \",\"%*s \"]", cur.m_Code, cur.m_Name, cur.m_Type) == 3) {
        g_AllFund.push_back(cur);
        fscanf(fr, "%*c");
    }
    fclose(fr);
    printf(" %lu Over\n", g_AllFund.size());
}
void *FuncGetJiNianDuZhangFu(void *arg)
{
    int idx = *(int *)arg;
    char cmd[TEMP_LEN];
    while(1) {
        g_TaskIDLE[idx] = true;
        sem_wait(&g_SemRes);
        pthread_spin_lock(&g_Mutex);
        g_TaskIDLE[idx] = false;
        int code = g_Set.front();
        g_Set.erase(g_Set.begin());
        pthread_spin_unlock(&g_Mutex);
        char fileName[TEMP_LEN];
        sprintf(fileName, "%s%d", TEMP_FILE_NAME, idx);
        sprintf(cmd, "wget -q --waitretry=3 http://fund.eastmoney.com/%06u.html -O %s", code, fileName);
        system(cmd);
        sprintf(cmd, "sed -i 's/<\\/div>/<\\/div>\\n/g' %s", fileName);
        system(cmd);
        int increaseAmountCnt = 0;
        int typeNameCnt = 0;
        FILE *fr = fopen(fileName, "r");
        vector<float> ratio;
        vector<int> rankA;
        vector<int> rankB;
        while(fgets(g_FileContent[idx], sizeof(g_FileContent[idx]), fr) != NULL) {
            if(strstr(g_FileContent[idx], "increaseAmount") != NULL) {
                ++increaseAmountCnt;
            } else if(increaseAmountCnt == 3) {
                if(strstr(g_FileContent[idx], "Rdata") != NULL) {
                    int len = strlen(g_FileContent[idx]);
                    int starPos = 0;
                    int endPos = 0;
                    bool find = false;
                    for(int i = len - 1; i >= 0; --i) {
                        if(g_FileContent[idx][i] == '/') {
                            find = true;
                        }
                        if(find) {
                            if(g_FileContent[idx][i] == '<') {
                                endPos = i - 1;
                            }
                            if(g_FileContent[idx][i] == '>') {
                                starPos = i + 1;
                                break;
                            }
                        }
                    }
                    //for(int i = starPos; i <= endPos; ++i) {
                    //    printf("%c", g_FileContent[idx][i]);
                    //}
                    //puts("");
                    if(typeNameCnt == 1) {
                        if(g_FileContent[idx][endPos] == '%') {
                            char cur[16];
                            strncpy(cur, g_FileContent[idx] + starPos, endPos - starPos);
                            cur[endPos - starPos] = '\0';
                            ratio.push_back(atof(cur));
                        }
                    } else if(typeNameCnt == 4) {
                        if(g_FileContent[idx][starPos] >= '0' && g_FileContent[idx][starPos] <= '9') {
                            char cur[32];
                            strncpy(cur, g_FileContent[idx] + starPos, endPos - starPos + 1);
                            cur[endPos - starPos + 1] = '\0';
                            int a, b;
                            sscanf(cur, "%d | %d", &a, &b);
                            rankA.push_back(a);
                            rankB.push_back(b);
                        }
                    }
                } else if(strstr(g_FileContent[idx], "typeName") != NULL) {
                    ++typeNameCnt;
                    //printf("typeNameCnt %d\n", typeNameCnt);
                }
            }
        }
        fclose(fr);
        if(ratio.size()) {
            for(auto it = g_AllFund.begin(); it != g_AllFund.end(); ++it) {
                if(atoi(it->m_Code) == code) {
                    it->m_NianDuRatio = ratio;
                    it->m_NianDuRankA = rankA;
                    it->m_NianDuRankB = rankB;
                    printf("%s\t%s\t%s\n", it->m_Code, it->m_Name, it->m_Type);
                    fflush(stdout);
                    break;
                }
            }
        }
    }
    return arg;
}
void GetNianDuZhangFu()
{
    pthread_t task[MAX_GET_THREAD_NUM];
    int idx[MAX_GET_THREAD_NUM];
    for(int i = 0; i < MAX_GET_THREAD_NUM; ++i) {
        idx[i] = i;
        pthread_create(&task[i], NULL, FuncGetJiNianDuZhangFu, &idx[i]);
    }
    for(auto it : g_AllFund) {
        pthread_spin_lock(&g_Mutex);
        g_Set.push_back(atoi(it.m_Code));
        sem_post(&g_SemRes);
        pthread_spin_unlock(&g_Mutex);
    }
    while(1) {
        bool exit = g_Set.size() == 0;
        for(int i = 0; i < MAX_GET_THREAD_NUM; ++i) {
            exit &= g_TaskIDLE[i];
        }
        if(exit) {
            break;
        }
        usleep(10000);
    }
    for(int i = 0; i < MAX_GET_THREAD_NUM; ++i) {
        pthread_cancel(task[i]);
        pthread_join(task[i], NULL);
    }
}
void GenData()
{
    char dataFile[TEMP_LEN] = "2020-06-30.dat";
    FILE *fr = fopen(dataFile, "r");
    if(fr == NULL) {
        sem_init(&g_SemRes, 0, 0);
        pthread_spin_init(&g_Mutex, 0);
        GetAllFundCode();
        GetNianDuZhangFu();
        system("date");
        FILE *fw = fopen(dataFile, "w");
        for(auto it = g_AllFund.begin(); it != g_AllFund.end(); ++it) {
            if(it->m_NianDuRatio.size()) {
                char msg[TEMP_LEN] = {0};
                int num = it->m_NianDuRatio.size();
                for(int i = 0; i < num; ++i) {
                    sprintf(msg + strlen(msg), "%.2f(%d/%d) ", *(it->m_NianDuRatio.begin() + i), *(it->m_NianDuRankA.begin() + i), *(it->m_NianDuRankB.begin() + i));

                }
                fprintf(fw, "%s\t%s\t%s\t%d\t%s\n", it->m_Code, it->m_Name, it->m_Type, num, msg);
            }
        }
        fclose(fw);
        pthread_spin_destroy(&g_Mutex);
        sem_destroy(&g_SemRes);
        char cmd[TEMP_LEN];
        sprintf(cmd, "rm -f %s*", TEMP_FILE_NAME);
        system(cmd);
        fr = fopen(dataFile, "r");
    }
    Fund cur;
    int year;
    g_AllFund.clear();
    while(fscanf(fr, "%s %s %s %d", cur.m_Code, cur.m_Name, cur.m_Type, &year) != EOF) {
        cur.m_NianDuRatio.clear();
        cur.m_NianDuRankA.clear();
        cur.m_NianDuRankB.clear();
        float ratio;
        int rankA, rankB;
        for(int i = 0; i < year; ++i) {
            fscanf(fr, "%f(%d/%d)", &ratio, &rankA, &rankB);
            cur.m_NianDuRatio.push_back(ratio);
            cur.m_NianDuRankA.push_back(rankA);
            cur.m_NianDuRankB.push_back(rankB);
        }
        cur.Update();
        g_AllFund.push_back(cur);
    }
    fclose(fr);
}
int main()
{
    system("date");
    GenData();
    unsigned yearBound = 5;
    printf("\nBy MinRatio && AvgRatio\n");
    sort(g_AllFund.begin(), g_AllFund.end(), cmpMin);
    for(auto it : g_AllFund) {
        if(it.m_NianDuRatio.size() >= yearBound && it.m_MinRatio > 3 && it.m_AvgRatio > 10) {
            printf("MinRatio %6.2f AvgRatio %6.2f MaxRatio %6.2f %s %s\n", it.m_MinRatio, it.m_AvgRatio, it.m_MaxRatio, it.m_Code, it.m_Name);
        }
    }
    printf("\nBy AvgRatio\n");
    sort(g_AllFund.begin(), g_AllFund.end(), cmpAvg);
    for(auto it : g_AllFund) {
        if(it.m_NianDuRatio.size() >= yearBound && it.m_AvgRatio > 15 && it.m_MinRatio > -10) {
            printf("MinRatio %6.2f AvgRatio %6.2f MaxRatio %6.2f %s %s\n", it.m_MinRatio, it.m_AvgRatio, it.m_MaxRatio, it.m_Code, it.m_Name);
        }
    }
    system("date");
    return 0;
}

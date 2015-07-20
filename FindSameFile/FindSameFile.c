#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXN        1024*1024
#define MD5LEN      32
#define TEMP_FILE   "__res.txt"

int main(int argc, char *argv[])
{
    if(argc == 1) {
        puts("parm error");
        return 1;
    }
    char *Path = (char*)malloc(MAXN);
    char *Cmd = (char*)malloc(MAXN);
    FILE *fr;
    char Tokens[] = {" \n\"#&'()-/;<>`|"};
    int p, i;
    for(p = 1; p < argc; ++p) {
        sprintf(Cmd, "rm -f %s", TEMP_FILE);
        system(Cmd);
        sprintf(Cmd, "find . -name \"*.%s\"", argv[p]);
        fr = popen(Cmd, "r");
        int Cnt = 0;
        while(fgets(Path, MAXN, fr) != NULL) {
            sprintf(Cmd, "md5sum ");
            int j = 7;
            for(i = 0; Path[i] != '\n'; ++i) {
                if(strchr(Tokens, Path[i]) != NULL) {
                    Cmd[j++] = '\\';
                }
                Cmd[j++] = Path[i];
            }
            sprintf(Cmd + j, " >> %s", TEMP_FILE);
            system(Cmd);
            ++Cnt;
        }
        pclose(fr);
        char Cur[MD5LEN];
        char *List = (char*)malloc(Cnt * (MD5LEN + 1));
        int Len = 0;
        fr = fopen(TEMP_FILE, "r");
        while(fscanf(fr, "%s", Cur) != EOF) {
            fgets(Path, MAXN, fr);
            int i;
            for(i = 0; i < Len; ++i) {
                if(strncmp(Cur, List + i * (MD5LEN + 1), MD5LEN) == 0) {
                    List[i * (MD5LEN + 1) + MD5LEN] = 0;
                    break;
                }
            }
            if(i == Len) {
                memcpy(List + Len * (MD5LEN + 1), Cur, MD5LEN);
                List[Len * (MD5LEN + 1) + MD5LEN] = 1;
                ++Len;
            }
        }
        for(i = 0; i < Len; ++i) {
            if(!List[i * (MD5LEN + 1) + MD5LEN]) {
                sprintf(Cmd, "grep -w \"%s\" %s", List + i * (MD5LEN + 1), TEMP_FILE);
                system(Cmd);
                puts("");
            }
        }
        fclose(fr);
        free(List);
    }
    sprintf(Cmd, "rm -f %s", TEMP_FILE);
    system(Cmd);
    free(Cmd);
    free(Path);
    return 0;
}

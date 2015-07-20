#ifndef _Generate_h_
#define _Generate_h_

#include <string.h>
#include "CFResource.h"

void SetName(int *div, char *name, char *Str)
{
    int i, j;
    for(i = 0, *div = 0; Str[i] != '\0'; ++i) {
        if(Str[i] >= '0' && Str[i] <= '9') {
            break;
        }
    }
    for(; Str[i] != '\0'; ++i) {
        if(Str[i] == '/') {
            break;
        }
        (*div) *= 10;
        (*div) += Str[i] - '0';
    }
    for(++i, j = 0; Str[i] != '"'; ++i, ++j) {
        name[j] = Str[i];
    }
    name[j] = '\0';
}

void GetHtml(int i, char *str)
{
    printf("Try to get %d%s.........\n", i, str);
    int l, p, Len, pn;
    char *Cmd, *Line, *Png, *SrcPng;
    Cmd = (char *)malloc(sizeof(char) * MAXLINELEN);
    Line = (char *)malloc(sizeof(char) * MAXLINELEN * MAXLINELEN);
    Png = (char *)malloc(sizeof(char) * MAXLINELEN);
    SrcPng = (char *)malloc(sizeof(char) * MAXLINELEN);
    FILE *fWrite, *fr;
    sprintf(Cmd, "wget -q http://codeforces.com/problemset/problem/%d/%s -O temp", i, str);
    system(Cmd);
    sprintf(Cmd, "problemset/%d%s", i, str);
    fWrite = fopen(Cmd, "w");
    fprintf(fWrite, "<html>\n\t<body>\n<LINK charset=\"utf-8\" href=\"src/style.css\" rel=\"stylesheet\" type=\"text/css\"/>\n\n");
    fr = popen("grep  \"\\\"ttypography\" temp", "r");
    fgets(Line, MAXLINELEN * MAXLINELEN, fr);
    Len = strlen(Line);
    for(l = 0; l < Len; ++l) {
        if(l < Len - 10) {
            if(Line[l] == 's' && Line[l + 1] == 'r' && Line[l + 2] == 'c') {
                fprintf(fWrite, "src=\"");
                l += 5;
                for(p = 0; Line[l + p] != '"'; ++p) {
                    SrcPng[p] = Line[l + p];
                }
                SrcPng[p] = '\0';
                while(p--) {
                    if(Line[l + p] == '/') {
                        break;
                    }
                }
                ++p;
                for(pn = 0; Line[l + p] != '"'; ++p, ++pn) {
                    Png[pn] = Line[l + p];
                }
                Png[pn] = '\0';
                fprintf(fWrite, "renderer/%s\"", Png);
                sprintf(Cmd, "wget -q %s -O problemset/renderer/%s", SrcPng, Png);
                system(Cmd);
                l += strlen(SrcPng) + 1;
            } else {
                fprintf(fWrite, "%c", Line[l]);
            }
        } else {
            fprintf(fWrite, "%c", Line[l]);
        }
    }
    fprintf(fWrite, "\n\t</body>\n</html>");
    fclose(fWrite);
    pclose(fr);
    free(SrcPng);
    free(Png);
    free(Line);
    free(Cmd);
}

int ProcessName(int i, char *str, int *over)
{
    char *Cmd = (char *)malloc(sizeof(char) * MAXLINELEN);
    sprintf(Cmd, "problemset/%d%s", i, str);
    FILE *fExist = fopen(Cmd, "r");
    if(fExist == NULL) {
        GetHtml(i, str);
    } else {
        *over = 1;
    }
    free(Cmd);
    return 0;
}

void MakeProblemSet()
{
    int i, div, over = 0;
    char *Str = (char *)malloc(sizeof(char) * MAXLINELEN);
    char name[5];
    FILE *fr;
    for(i = 1; i <= Pages; ++i) {
        printf("Add Page : %d===================================\n", i);
        sprintf(Str, "wget -q http://codeforces.com/problemset/page/%d -O page", i);
        system(Str);
        fr = popen("grep \"problemset/problem\" page", "r");
        while(fgets(Str, MAXLINELEN, fr) != NULL) {
            fgets(Str, MAXLINELEN, fr);
            SetName(&div, name, Str);
            ProcessName(div, name, &over);
            if(over) {
                pclose(fr);
                goto Finished;
            }
        }
        pclose(fr);
    }
Finished:
    system("rm page");
    free(Str);
}

void AddToIndex(FILE *file, int i, char *str)
{
    static int IndexCnt = 0;
    fprintf(file, "\t\t<td><a href=problemset/%d%s>%d%s</a></td>\n", i, str, i, str);
    printf("Add %d%s OK.\n", i, str);
    ++IndexCnt;
    if((IndexCnt & 7) == 0) {
        fprintf(file, "\t</tr><tr>\n");
    }
}

void MakeForMe()
{
    int i, div;
    char name[5];
    FILE *fIndex, *fr, *fExist;
    char *Line = (char *)malloc(sizeof(char) * MAXLINELEN);
    system("touch ForMe && rm ForMe");
    fIndex = fopen("ForMe", "w");
    fprintf(fIndex, "<html>\n\t<body>\n\n<h1 align=center>CodeForces For Me</h1>\n");
    fprintf(fIndex, "<table align=center width=\"90%%\">\n\t<tr>\n");
    for(i = 1; i <= Pages; ++i) {
        printf("Add Page : %d===================================\n", i);
        sprintf(Line, "wget -q http://codeforces.com/problemset/page/%d?order=BY_SOLVED_DESC -O temp", i);
        system(Line);
        fr = popen("grep \"problemset/problem\" temp", "r");
        while(fgets(Line, MAXLINELEN, fr) != NULL) {
            fgets(Line, MAXLINELEN, fr);
            SetName(&div, name, Line);
            sprintf(Line, "/home/nosoul/NoSoul.Love/MyProjects/MyCode/CodeForces/%d%s.c", div, name);
            fExist = fopen(Line, "r");
            if(fExist == NULL) {
                AddToIndex(fIndex, div, name);
            } else {
                fclose(fExist);
            }
        }
        pclose(fr);
    }
    fprintf(fIndex, "\t</tr>\n</table>\n\n\t</body>\n</html>");
    fclose(fIndex);
}
#endif

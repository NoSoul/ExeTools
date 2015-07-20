#ifndef _CFResource_h_
#define _CFResource_h_

#include <stdio.h>
#include <stdlib.h>
#define MAXLINELEN	1000

int Pages;

void GetCSS()
{
    printf("Get CSS File...\n");
    FILE *fExist = fopen("problemset/src/style.css", "r");
    if(fExist == NULL) {
        system("mkdir problemset && mkdir problemset/src problemset/renderer");
        system("wget -q http://worker.codeforces.ru/static/combine/ec1325e741305c9a1a85151d0f3e0565_8a4397705a15b7078eeb3cc514684ad0.css -O problemset/src/style.css");
    } else {
        fclose(fExist);
    }
}

void GetPages()
{
    printf("Get Pages...\n");
    int i;
    FILE *fr;
    char *Line = (char *)malloc(sizeof(char) * MAXLINELEN);
    system("wget -q http://codeforces.com/problemset -O temp");
    fr = popen("grep \"page-index\" temp", "r");
    while(fgets(Line, MAXLINELEN, fr) != NULL);
    for(i = 0; Line[i] != '\0'; ++i) {
        if(Line[i] >= '0' && Line[i] <= '9') {
            break;
        }
    }
    for(; Line[i] != '\0'; ++i) {
        if(Line[i] < '0' || Line[i] > '9') {
            break;
        }
        Pages *= 10;
        Pages += Line[i] - '0';
    }
    pclose(fr);
    free(Line);
}
#endif

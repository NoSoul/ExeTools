#include "Generate.h"

int main(int argc, char *argv[])
{
    GetCSS();
    GetPages();
    int i;
    for(i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "Set") == 0) {
            printf("Start Make ProblemSet...\n");
            MakeProblemSet();
            system("rm temp");
            printf("End Make ProblemSet.\n");
        }
        if(strcmp(argv[i], "Me") == 0) {
            printf("Start Make For Me...\n");
            MakeForMe();
            system("rm temp");
            printf("End Make For Me.\n");
        }
    }
    return 0;
}

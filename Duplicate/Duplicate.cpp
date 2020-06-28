/*******************************************************************************
Copyright: 2020, Innovem Tech. Co., Ltd.
FileName: Duplicate.cpp
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc != 5) {
        printf("Usage: %s init delta times ratio\n", argv[0]);
        return 1;
    }
    double sum = atof(argv[1]);
    for(int i = 0; i < atoi(argv[3]); ++i) {
        sum *= 1 + atof(argv[4]);
        sum += atof(argv[2]);
    }
    printf("%f\n", sum);
    return 0;
}

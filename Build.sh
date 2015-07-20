# !/bin/sh
CCFLAG="-O3 -W -Wall -Werror"
SUFFIX="-suse-linux"
gcc -o FindSameFile$SUFFIX $CCFLAG FindSameFile/FindSameFile.c
g++ -o GetGoogleIP$SUFFIX $CCFLAG GetGoogleIP/GetGoogleIP.cpp -lpthread
gcc -o LocalCodeForces$SUFFIX $CCFLAG LocalCodeForces/LocalCodeForces.c

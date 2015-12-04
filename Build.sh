# !/bin/sh
CCFLAG="-O2 -W -Wall -Werror"
LDFLAG="-lpthread"
SUFFIX="-suse-linux"
g++ -o CodeForcesAide$SUFFIX $CCFLAG CodeForcesAide/CodeForcesAide.cpp $LDFLAG
gcc -o CtrlInput$SUFFIX $CCFLAG CtrlInput/CtrlInput.c $LDFLAG
gcc -o FindSameFile$SUFFIX $CCFLAG FindSameFile/FindSameFile.c $LDFLAG
g++ -o GetGoogleIP$SUFFIX $CCFLAG GetGoogleIP/GetGoogleIP.cpp $LDFLAG

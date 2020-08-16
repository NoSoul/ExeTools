# !/bin/sh
CFLAG="-O3 -W -Wall -Werror"
CPPFLAG="-O3 -W -Wall -Werror -std=c++11"
LDFLAG="-lpthread"
SUFFIX="-suse-linux"
g++ -o ArchivingCodeForces$SUFFIX $CPPFLAG ArchivingCodeForces/ArchivingCodeForces.cpp $LDFLAG
g++ -o CodeForcesAide$SUFFIX $CPPFLAG CodeForcesAide/CodeForcesAide.cpp $LDFLAG
gcc -o CtrlInput$SUFFIX $CFLAG CtrlInput/CtrlInput.c $LDFLAG
gcc -o FindSameFile$SUFFIX $CFLAG FindSameFile/FindSameFile.c $LDFLAG
g++ -o GetGoogleIP$SUFFIX $CPPFLAG GetGoogleIP/GetGoogleIP.cpp $LDFLAG
g++ -o AnalysisFund$SUFFIX $CPPFLAG AnalysisFund/AnalysisFund.cpp $LDFLAG 
mv *$SUFFIX ~/bin/

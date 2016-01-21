# !/bin/sh
if [ $# -ne 1 ]; then
    echo "Usage: $0 \"<C expression>\""
    exit 1;
fi
exename=__ccal
echo "#include <stdio.h>" > $exename.c
echo "#include <math.h>" >> $exename.c
echo "" >> $exename.c
echo "int main()" >> $exename.c
echo "{" >> $exename.c
echo "    printf(\"%lf\\n\", 1.0*($1));" >> $exename.c
echo "    return 0;" >> $exename.c
echo "}" >> $exename.c
gcc -o $exename $exename.c -lm && ./$exename
rm -f $exename*
exit 0;

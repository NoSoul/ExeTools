#! /bin/sh
rm -f testmd*.html
browers=chromium
if [ $# -eq 0 ];then
    echo "<meta http-equiv="Content-Type" content="text/html\; charset=utf-8">" > testmd0.html
    markdown *.md -o checkmdtemp.html && cat checkmdtemp.html >> testmd0.html && rm checkmdtemp.html && $browers testmd0.html 
else
    i=0
    for arg in $@;do
        echo "<meta http-equiv="Content-Type" content="text/html\; charset=utf-8">" > testmd$i.html
        markdown $arg -o checkmdtemp.html && cat checkmdtemp.html >> testmd$i.html && rm checkmdtemp.html && $browers testmd$i.html
        i=`expr $i + 1`
    done
fi
exit 0;

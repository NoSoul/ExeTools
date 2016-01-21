# !/bin/sh
if [ $# -ne 1 ];then
    echo "Usage: $0 yyyy-mm-dd"
    exit 1;
fi
echo "    'after $1'"
username=NoSoul
for filename in $(grep -wn "Innovem" *.cpp *.h PHY/* PHYAlgorithm/* 2>/dev/null | cut -d : -f 1);do
    ret="`git blame $filename | awk -v TimeBoud=$1 -v UserName="($username" '
    {
        for (i = 1; i <= NF; i++) {
            if($i == UserName && $(i+1) > TimeBoud && !/#include/) {
                print $0
            }
        }
    }'`"
    if [ -n "$ret" ];then
        echo ""
        echo $filename
        awk '/#include/ { print NR ") " $0}' $filename
        git blame $filename | awk -v TimeBoud=$1 -v UserName="($username" '
        {
            for (i = 1; i <= NF; i++) {
                if($i == UserName && $(i+1) > TimeBoud && !/#include/) {
                    print $0
                }
            }
        }'
    fi
done
exit 0

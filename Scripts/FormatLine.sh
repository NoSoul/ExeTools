# !/bin/sh
if [ $# -eq 0 ];then
    echo "Usage: $0 filename"
    exit 1;
fi
for file in $@;do
    awk 'length($0)>1 {print $0 "\n"}' $file  > __tempfile
    mv __tempfile $file
done
exit 0;

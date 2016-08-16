# !/bin/sh
if [ $# -eq 0 ];then
    echo "Usage: $0 filename"
    exit 1;
fi
function Format
{
    while [ $(awk 'END {print NR}' AfterChange/$fileName) -ne 1 ];do
        sed -i 's/ //g' AfterChange/$fileName
        cat AfterChange/$fileName | tr "\n" " " > AfterChange/_$fileName
        mv AfterChange/_$fileName AfterChange/$fileName
        cat AfterChange/$fileName | tr "\r" " " > AfterChange/_$fileName
        mv AfterChange/_$fileName AfterChange/$fileName
    done
    sed -i 's/^[ \t]*//g' AfterChange/$fileName
    sed -i 's/[[:space:]]*//g' AfterChange/$fileName
    sed -i 's/\t//g' AfterChange/$fileName
    sed -i 's///g' AfterChange/$fileName
    sed -i 's/「/“/g' AfterChange/$fileName
    sed -i 's/」/”/g' AfterChange/$fileName
    sed -i 's/『/“/g' AfterChange/$fileName
    sed -i 's/』/”/g' AfterChange/$fileName
    sed -i 's/。”/”/g' AfterChange/$fileName
    sed -i 's/。/。\n/g' AfterChange/$fileName
    sed -i 's/”“/”\n“/g' AfterChange/$fileName
    awk 'length($0)>1 {printf("%s\n\n", $0)}' AfterChange/$fileName  > AfterChange/$safeFileName
    rm -f AfterChange/$fileName
}
SAVEIFS=$IFS
IFS=$(echo -en "\n\b")
rm -rf AfterChange
mkdir -p AfterChange
for fileName in $@;do
{
    safeFileName="$(echo $fileName | sed 's/ //g')"
    if [ $(file $fileName | grep "ISO-8859 text" | wc -l) -ne 0 ];then
        iconv -f GB18030 -t UTF-8 $fileName -o AfterChange/_$safeFileName 2>/dev/null
    else
        cp $fileName AfterChange/_$safeFileName
    fi
}&
done
wait
for fileName in $(ls AfterChange/);do
{
    safeFileName="$(echo $fileName | cut -d '_' -f 2)"
    while [ $(awk 'END {print NR}' AfterChange/$fileName) -ne 1 ];do
        sed -i 's/ //g' AfterChange/$fileName
        cat AfterChange/$fileName | tr "\n" " " > AfterChange/_$fileName
        mv AfterChange/_$fileName AfterChange/$fileName
        cat AfterChange/$fileName | tr "\r" " " > AfterChange/_$fileName
        mv AfterChange/_$fileName AfterChange/$fileName
    done
    sed -i 's/^[ \t]*//g' AfterChange/$fileName
    sed -i 's/[[:space:]]*//g' AfterChange/$fileName
    sed -i 's/\t//g' AfterChange/$fileName
    sed -i 's///g' AfterChange/$fileName
    sed -i 's/「/“/g' AfterChange/$fileName
    sed -i 's/」/”/g' AfterChange/$fileName
    sed -i 's/『/“/g' AfterChange/$fileName
    sed -i 's/』/”/g' AfterChange/$fileName
    sed -i 's/。”/”/g' AfterChange/$fileName
    sed -i 's/。/。\n/g' AfterChange/$fileName
    sed -i 's/”“/”\n“/g' AfterChange/$fileName
    awk 'length($0)>1 {printf("%s\n\n", $0)}' AfterChange/$fileName  > AfterChange/$safeFileName
    rm -f AfterChange/$fileName
} &
done
wait
mv AfterChange/* .
rm -rf AfterChange
IFS=$SAVEIFS
exit 0;

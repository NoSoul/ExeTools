# /bin/sh
for filename in $(ls *.sh);do
    diff $0 $filename > /dev/null
    if [ $? -ne 0 ];then
        echo "rm -f $HOME/bin/$filename && ln -sf $PWD/$filename $HOME/bin/$filename"
        rm -f $HOME/bin/$filename && ln -sf $PWD/$filename $HOME/bin/$filename
    fi
done
if [ -f .vimrc ];then
    echo "rm -f $HOME/.vimrc && ln -sf $PWD/.vimrc $HOME/.vimrc"
    rm -f $HOME/.vimrc && ln -sf $PWD/.vimrc $HOME/.vimrc
fi
exit 0;

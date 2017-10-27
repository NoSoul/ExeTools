set hlsearch "选中高亮
colorscheme desert "颜色主题
set noendofline "禁用文件尾新加一行
set guifont=Source\ Code\ Pro\ 14 "设置字体
"set number "显示行号
set tabstop=4 "制表符
set expandtab "tab使用空格
set shiftwidth=4 "缩进空格
set autoindent "自动缩进
set cindent "C/C++语言缩进
set encoding=utf-8 "设置内部编码方式
set fileencodings=utf-8,gbk "编码识别列表
set foldmethod=indent "代码折叠
set mousemodel=popup "右键弹出菜单
set viminfo= "关闭历史记录
set noswapfile  "不生成swap文件
set guioptions-=m "隐藏菜单栏
set guioptions-=T "隐藏工具栏
set guioptions-=r "隐藏右侧滚动条
set guioptions-=L "隐藏左侧滚动条
set makeprg=./Build.sh "设置:make命令
"set autochdir "自动切换工作目录
set autoread "自动读取文件
filetype plugin indent on "打开文件类型检测、插件、缩进
"C/C++文件使用80字符提示
"autocmd BufRead,BufNewFile *.{cpp,c,h} set tw=80
"autocmd BufRead,BufNewFile *.{cpp,c,h} set cc=81
"三种模式下的键映射---<F3>AStyle C/C++文件
func! AStyle()
    if &filetype == 'c' || &filetype == 'cpp'
        exec "wall"
        exec "!astyle --style=kr -p --suffix=none %"
    endif
    exec "e! %"
endfunc
map <F3> :call AStyle()<CR>
imap <F3> <ESC>:call AStyle()<CR>
vmap <F3> <ESC>:call AStyle()<CR>
"三种模式下的键映射---<F4>切换源文件/头文件
map <F4> :AV<CR>
imap <F4> <ESC>:AV<CR>
vmap <F4> <ESC>:AV<CR>
"三种模式下的键映射---<F5>一键Build
func! Build()
    if filereadable("Build.sh")
    	exec "wall"
	    exec "make"
	    exec "cw"
    else
        exec "wall"
        if expand("%:e") == 'c'
            exec "!gcc-7 -o suse-linux-a -Wall -W -Werror -mavx -g -lm -lpthread % && ./suse-linux-a"
        elseif expand("%:e") == 'cpp'
            exec "!g++-7 -o suse-linux-a -Wall -W -Werror -mavx -g -std=c++11 -lm -lpthread % && ./suse-linux-a"
        endif
    endif
endfunc
map <F5> :call Build()<CR>
imap <F5> <ESC>:call Build()<CR>
vmap <F5> <ESC>:call Build()<CR>
"非编辑模式下的键映射---Ctrl+a全选
map <C-A> ggVG
vmap <C-A> ggVG
"查找定义Ctrl+g
if filereadable("cscope.out")
    cs add cscope.out
endif
map <C-g> :cs find g <C-R>=expand("<cword>")<CR><CR>
imap <C-g> <ESC>:cs find g <C-R>=expand("<cword>")<CR><CR>
vmap <C-g> <ESC>:cs find g <C-R>=expand("<cword>")<CR><CR>
map <C-d> :cs find d <C-R>=expand("<cword>")<CR><CR>
imap <C-d> <ESC>:cs find d <C-R>=expand("<cword>")<CR><CR>
vmap <C-d> <ESC>:cs find d <C-R>=expand("<cword>")<CR><CR>
map <C-c> :cs find c <C-R>=expand("<cword>")<CR><CR>
imap <C-c> <ESC>:cs find c <C-R>=expand("<cword>")<CR><CR>
vmap <C-c> <ESC>:cs find c <C-R>=expand("<cword>")<CR><CR>
"新建C/C++文件插入文件头及宏定义
autocmd BufNewFile *.{cpp,c,h} exec ":call AddSourceFileHeader()"
func! AddSourceFileHeader()
    call setline(1, "/*******************************************************************************")
    call setline(2, "Copyright: 2017, Innovem Tech. Co., Ltd.")
    call setline(3, "FileName: ".expand("%:t"))
    call setline(4, "*******************************************************************************/")
    if expand("%:e") == 'h'
        call setline(5, "#ifndef _".expand("%:t:r").expand("_H_"))
        call setline(6, "#define _".expand("%:t:r").expand("_H_"))
        call setline(7, "#endif")
    endif
endfunc
"新建文件自动定位文件末尾
autocmd BufNewFile * normal G

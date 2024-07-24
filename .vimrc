set nocompatible
set t_Co=256
syntax enable
set cursorline
hi CursorLine cterm=none ctermbg=DarkMagenta ctermfg=White 
set nu
set tabstop=4
set bg=dark
set hlsearch
hi Search cterm=reverse ctermbg=none ctermfg=none

set mouse=a






set nocompatible
filetype off

set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

Plugin 'VundleVim/Vundle.vim'

call vundle#end()
filetype plugin indent on

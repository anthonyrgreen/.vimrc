set ts=2
set expandtab
set autoindent
set background=dark
set ruler
set number
syntax on
nnoremap <silent> <A-Up> :wincmd k<CR>
nnoremap <silent> <A-Down> :wincmd j<CR>
nnoremap <silent> <A-Left> :wincmd h<CR>
nnoremap <silent> <A-Right> :wincmd l<CR>
autocmd Filetype html,xml,xsl source ~/.vim/scripts/closetag.vim 
noremap <C-l> :!pdflatex %
inoremap jk <Esc> 
nnoremap <CR> o<Esc>k

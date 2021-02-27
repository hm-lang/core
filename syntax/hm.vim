" Vim syntax file
" Language:	Hm-lang
" Maintainer:	Lucas Wagner <lowagner@gmail.com>
" Last Change:	2020-08-21
" Credits:	Zvezdan Petkovic, Neil Schemenauer, and Dmitry Vasiliev (python.vim) 
"		Bram Moolenaar (c.vim)
" Info:	Can copy to ~/.vim/syntax/hm.vim, then edit ~/.vim/filetype.vim
"	to include a "*.hm" line:
"		"au BufNewFile,BufRead *.hm		setf hm"
if exists("b:current_syntax")
  finish
endif

" We need nocompatible mode in order to continue lines with backslashes.
" Original setting will be restored.
let s:cpo_save = &cpo
set cpo&vim

syn keyword hmSpecial	debug This this abstract

syn keyword hmStatement	goto return and or not from from_ from__ _from __from _from_ to
syn keyword hmStatement	define settable gettable
syn keyword hmStatement	enum try catch throw as in assert
syn keyword hmStatement	class
syn keyword hmStatement	print printing warn warning
syn keyword hmConditional	if then elif else consider case default also
syn keyword hmRepeat	for while break continue on
syn match hmLambdaVariable	/\$\v<[A-Z]\S*>/
syn match hmLambdaFunction	/\$\v<[a-z]\S*>/
syn match	hmType	"\v<[a-z][a-zA-Z_0-9]*\(@!"
syn match	hmFunction		"\v<[a-z][a-zA-Z_0-9]*\ze\("
syn match	hmSpecifier		"\v<[A-Z][A-Za-z0-9]*>"
syn region hmCcBlock start="cc{" end="}cc"
syn match   hmComment	"#.*$" contains=hmTodo
syn region hmComment	start="#/" end="/#"
syn keyword hmTodo		FIXME NOTE NOTES TODO XXX contained
syn match hmFile		"\v\@<[A-Za-z_]*>"

syn match	hmFormat		display "%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlLjzt]\|ll\|hh\)\=\([aAbdiuoxXDOUfFeEgGcCsSpn]\|\[\^\=.[^]]*\]\)" contained
syn match   hmEscape	display contained "\\." 

syn region  hmString matchgroup=hmQuotes
      \ start=+"+ end=+"+ skip=+\\\\\|\\"+
      \ contains=hmEscape,hmFormat,@Spell extend
syn region  hmString matchgroup=hmQuotes
      \ start=+`+ end=+`+ skip=+\\\\\|\\"+
      \ contains=hmEscape,hmFormat,@Spell extend
"syn region  hmTranspilerDirective matchgroup=hmTranspilerDirective
"      \ start="hm\"\"\"" end="\"\"\"" keepend
"      \ contains=hmEscape
syn region  hmComment matchgroup=hmLongComment
      \ start="===" end="===" keepend
      \ contains=hmEscape,hmSpaceError,hmTodo

syn match	hmCharError	"'\\[^abfnrtv'\\]*'"
syn match	hmCharError	"''"
syn match	hmCharError	"'''"
syn match	hmCharError	"'[^\\].*'"
syn match	hmChar	"'[^'\\]'" 
syn match	hmEscapeChar	"'\\[abfnrtv'\\]'" 

if exists("hm_highlight_all")
  if exists("hm_no_builtin_highlight")
    unlet hm_no_builtin_highlight
  endif
  if exists("hm_no_exception_highlight")
    unlet hm_no_exception_highlight
  endif
  if exists("hm_no_number_highlight")
    unlet hm_no_number_highlight
  endif
  let hm_space_error_highlight = 1
endif

" It is very important to understand all details before changing the
" regular expressions below or their order.
" The word boundaries are *not* the floating-point number boundaries
" because of a possible leading or trailing decimal point.
" The expressions below ensure that all valid number literals are
" highlighted, and invalid number literals are not.  For example,
"
" - a decimal point in '4.' at the end of a line is highlighted,
" - a second dot in 1.0.0 is not highlighted,
" - 08 is not highlighted,
" - 08e0 or 08j are highlighted,

if !exists("hm_no_number_highlight")
  " numbers (including longs and complex)
  syn match   hmNumber	"\<0[oO]\=\o\+[Ll]\=[Uu]\=\>"
  syn match   hmNumber	"\<0[xX]\x\+[Ll]\=[Uu]\=\>"
  syn match   hmNumber	"\<0[bB][01]\+[Ll]\=[Uu]\=\>"
  syn match   hmNumber	"\<\%([1-9]\d*\|0\)[Ll]\=[Uu]\=\>"
  syn match   hmNumber	"\<0[oO]\=\o\+[Uu][Ll]\=\>"
  syn match   hmNumber	"\<0[xX]\x\+[Uu][Ll]\=\>"
  syn match   hmNumber	"\<0[bB][01]\+[Uu][Ll]\=\>"
  syn match   hmNumber	"\<\%([1-9]\d*\|0\)[Uu][Ll]\=\>"
  syn match   hmNumber	"\<\d*\>"
  syn match   hmNumber	"\<\d\+[jJ]\>"
  syn match   hmNumber	"\<\d\+[eE][+-]\=\d\+[jJ]\=\>"
  syn match   hmNumber
	\ "\<\d\+\.\%([eE][+-]\=\d\+\)\=[jJ]\=\%(\W\|$\)\@="
  syn match   hmNumber
	\ "\%(^\|\W\)\zs\d*\.\d\+\%([eE][+-]\=\d\+\)\=[jJ]\=\>"
endif

if !exists("hm_no_builtin_highlight")
  syn keyword hmBuiltin	main test
  syn keyword hmBuiltin NEW new old any
  syn keyword hmBuiltin	yield extends
  syn keyword hmBuiltin	False Null True Unspecified
  syn keyword hmBuiltin	unpack complete pass descope
  "syn keyword hmBuiltin	create delete reallocate allocate zero bits bytes attach absolve free copy
  syn keyword hmBuiltin	abs floor ceil min max
  syn keyword hmBuiltin	seed random
  syn keyword hmBuiltin	now retype
endif

" trailing whitespace
syn match  hmSpaceError	display excludenl "\s\+$"
" mixed tabs and spaces
"syn match   hmSpaceError	display " \+\t"
"syn match   hmSpaceError	display "\t\+ "
" tabs in general:
syn match  hmSpaceError	display "\t"

" Sync at the beginning of class, function, or method definition.
syn sync match hmSync grouphere NONE "^\s*\%(debug\|class\)\s\+\h\w*\s*("

if version >= 508 || !exists("did_hm_syn_inits")
  if version <= 508
    let did_hm_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  " The default highlight links.  Can be overridden later.
  HiLink hmEscape		Special
  HiLink hmStatement	Statement
  HiLink hmConditional	Conditional
  HiLink hmRepeat		Repeat
  HiLink hmLambdaVariable		Statement
  HiLink hmSpecifier		Normal
  HiLink hmLambdaFunction		Function
  HiLink hmFunction		Function
  HiLink hmType	Function
  HiLink hmComment		Comment
  HiLink hmSpecial		Special
  HiLink hmFile		Statement
  HiLink hmSingleQuotes		Special
  HiLink hmChar		Statement
  HiLink hmEscapeChar		Special
  HiLink hmCharError		Error
  HiLink hmLongComment		String
  HiLink hmCcBlock		Special
  HiLink hmTodo		Todo
  HiLink hmFormat		Special
  HiLink hmString		String
  HiLink hmQuotes		String
  HiLink hmEscape		Special
  "HiLink hmTranspilerDirective	Special
  if !exists("hm_no_number_highlight")
    HiLink hmNumber		Number
  endif
  if !exists("hm_no_builtin_highlight")
    HiLink hmBuiltin	Special
  endif
  HiLink hmSpaceError	Error
  delcommand HiLink
endif

let b:current_syntax = "hm"

let &cpo = s:cpo_save
unlet s:cpo_save

" vim:set sw=2 sts=2 ts=8 noet:

default: compile

compile:
	bison -d ratlang.y
	flex ratlang.l
	gcc -o ratlang lex.yy.c ratlang.tab.c env.c

default: compile

compile:
	bison --report all -d ratlang.y
	flex ratlang.l
	gcc -g -lm -lreadline -o ratlang lex.yy.c parser.c env.c value.c node.c

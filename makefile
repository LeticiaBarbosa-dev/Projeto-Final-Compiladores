all : regras.l regras.y
	clear
	flex -i regras.l
	bison regras.y
	gcc regras.tab.c -o compilador -lfl -lm
	./compilador

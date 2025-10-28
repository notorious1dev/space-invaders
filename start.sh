OBJ_DIRECTORY="obj"


if [ ! -d "$OBJ_DIRECTORY" ]; then
	echo "'$OBJ_DIRECTORY' does not exist. Creating..."
	mkdir "$OBJ_DIRECTORY"
else
	echo "'$OBJ_DIRECTORY' already exists"
fi

gcc -c ./src/utilities.c -I./include -lm -o ./obj/utilities.o
ar rcs ./lib/libutilities.a ./obj/utilities.o

gcc ./src/main.c -I./include/ -L./lib/ -lraylib -lutilities -lm -o main
./main

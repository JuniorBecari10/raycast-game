all:
	gcc main.c -Iraylib/include lib/libraylib.a -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXcursor -lGL -o game

make run:
	(make && ./game) > /dev/null

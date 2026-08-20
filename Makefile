# If you use Void, use these libs to compile:
# libX11-devel libXrandr-devel libXinerama-devel libXi-devel libXcursor-devel MesaLib-devel

all:
	gcc main.c -Iraylib/include lib/libraylib.a -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXcursor -lGL -o game

run:
	(make && ./game) > /dev/null

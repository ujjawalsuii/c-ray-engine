all: MS1_assg MS2_assg FS_assg

MS1_assg: src/assg.c src/vector.c src/spheres.c

	gcc -Wall -Werror -std=c99 -DMS1 -o MS1_assg src/spheres.c src/assg.c src/vector.c src/color.c -lm

MS2_assg: src/spheres.c src/assg.c src/vector.c src/color.c

	gcc -Wall -Werror -std=c99 -DMS2 -o MS2_assg src/spheres.c src/assg.c src/vector.c src/color.c -lm

FS_assg: src/vector.c src/spheres.c src/assg.c src/color.c

	gcc -Wall -Werror -std=c99 -DFS -o FS_assg src/spheres.c src/assg.c src/vector.c src/color.c -lm

clean:

	rm -f MS1_assg MS2_assg FS_assg

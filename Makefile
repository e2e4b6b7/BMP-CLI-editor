.PHONY: all clean compile_subfiles debug debug_compile_subfiles

common_flags := -Wall -Wextra -Werror -I include

all: compile_subfiles hw_01

hw_01: obj/main.o obj/bmp.o
	gcc $(common_flags) -o hw_01 obj/*.o

compile_subfiles: obj/bmp.o | obj

obj/main.o: src/main.c include/bmp.h | obj
	gcc $(common_flags) -o obj/main.o -c src/main.c

obj/bmp.o: src/bmp.c include/bmp.h | obj
	gcc $(common_flags) -o obj/bmp.o -c src/bmp.c

obj:
	mkdir -p obj

clean: 
	rm -rf obj debug_obj hw_01 debug_hw_01



debug_flags := -Wall -Wextra -Werror -I include -g -fsanitize=address -fsanitize=undefined -fsanitize=leak


debug: clean debug_compile_subfiles debug_hw_01

debug_hw_01: debug_obj/main.o debug_obj/bmp.o
	gcc $(debug_flags) -o debug_hw_01 debug_obj/*.o

debug_compile_subfiles: debug_obj/bmp.o | debug_obj

debug_obj/main.o: src/main.c include/bmp.h | debug_obj
	gcc $(debug_flags) -o debug_obj/main.o -c src/main.c

debug_obj/bmp.o: src/bmp.c include/bmp.h | debug_obj
	gcc $(debug_flags) -o debug_obj/bmp.o  -c src/bmp.c

debug_obj:
	mkdir -p debug_obj

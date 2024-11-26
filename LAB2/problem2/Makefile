all: sum_serial sum_multi_thread

sum_serial: sum_serial.c
	gcc sum_serial.c -o sum_serial

sum_multi_thread: sum_multi_thread.c
	gcc -pthread sum_multi_thread.c -o sum_multi_thread
clean:
	rm sum_serial sum_multi_thread


all:
	cc -c queue.c
	cc -c set.c
	cc -c monitor.c
	cc -c process_sync.c
	cc -c reader_writer.c
	cc queue.o set.o monitor.o process_sync.o reader_writer.o -o readWrite
clean:
	rm readWrite

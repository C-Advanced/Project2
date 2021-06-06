CC=gcc

all: libfdr.a mini_project

.PHONY: libfdr.a

libfdr.a:
	$(MAKE) -C libfdr
mini_project:
	${CC} -o $@ -w mini_project.c libfdr/libfdr.a
clean:
	rm -f mini_project
	$(MAKE) clean -C libfdr
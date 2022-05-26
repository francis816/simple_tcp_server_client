src = $(wildcard *.c)
obj = $(patsubst %.c, %, $(src))


ALL:$(obj)


%:%.c	
	gcc $< -o $@



clean :
	-rm -rf $(obj)
	

all:
	$(MAKE) -C ./lib
	$(MAKE) -C ./demo

clean:
	$(MAKE) clean -C ./lib
	$(MAKE) clean -C ./demo

MAKE= make -C
all: smelt
smelt:
	core extensions
core:
	$(MAKE) ./smelt/sdl/
	$(MAKE) ./smelt/sdl/ dumb
extensions:
	$(MAKE) ./extensions/
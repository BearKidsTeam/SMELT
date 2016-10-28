MAKE= make -C
all: smelt
smelt:
	core extensions
core:
	$(MAKE) ./smelt/sdl/
	$(MAKE) ./smelt/sdl/ dumb
	$(MAKE) ./smelt/glfw/
	$(MAKE) ./smelt/glfw/ dumb
extensions:
	$(MAKE) ./extensions/

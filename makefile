MAKE= make -B -C
.PHONY: smelt core extensions
all: smelt
smelt: core extensions
core:
	$(MAKE) ./smelt/sdl/CxImage/
	$(MAKE) ./smelt/sdl/
	$(MAKE) ./smelt/sdl/ dumb
	$(MAKE) ./smelt/glfw/CxImage/
	$(MAKE) ./smelt/glfw/
	$(MAKE) ./smelt/glfw/ dumb
extensions:
	$(MAKE) ./extensions/

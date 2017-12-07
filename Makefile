.PHONY : build
build :
	gcc src/splay.c -o splay \
		-Wall\
		-I/usr/include/vlc/plugins \
		-lncurses \
		-ltinfo \
		-lvlc \
		-lvlccore \
		-pthread

	chmod +x splay

.PHONY : build-mpris
build-mpris :
	gcc src/splay-mpris.c -o splay \
		-Wall\
		-I/usr/include/dbus-1.0 \
		-I/usr/lib/dbus-1.0/include  \
		-I/usr/include/vlc/plugins \
		-ldbus-1 \
		-lncurses \
		-ltinfo \
		-lvlc \
		-lvlccore \
		-pthread

	chmod +x splay

.PHONY : debug
debug :
	gcc -g -O0 -D DEBUG=1 src/splay.c -o splay \
		-Wall\
		-I/usr/include/vlc/plugins \
		-lncurses \
		-ltinfo \
		-lvlc \
		-lvlccore \
		-pthread

	chmod +x splay

.PHONY : debug-mpris
debug-mpris :
	gcc -g -O0 -D DEBUG=1 src/splay-mpris.c -o splay \
		-Wall\
		-I/usr/include/dbus-1.0 \
		-I/usr/lib/dbus-1.0/include  \
		-I/usr/include/vlc/plugins \
		-ldbus-1 \
		-lncurses \
		-ltinfo \
		-lvlc \
		-lvlccore \
		-pthread

	chmod +x splay

.PHONY : clean
clean :
	-rm splay

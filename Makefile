.PHONY : build
build :
	gcc src/splay.c -o splay -lncurses -ltinfo -lvlc -pthread

	chmod +x splay
	
.PHONY : build-mpris
build-mpris :
	gcc src/splay-mpris.c -o splay \
		    -I/usr/include/dbus-1.0 \
		    -I/usr/lib/dbus-1.0/include  \
		    -ldbus-1 \
		    -lncurses \
		    -ltinfo \
		    -lvlc \
		    -pthread

	chmod +x splay
	
.PHONY : debug
debug :
	gcc src/splay.c -o splay -lncurses -ltinfo -lvlc -pthread

	chmod +x splay
	
.PHONY : debug-mpris
dbug-mpris :
	gcc -D DEBUG=1 src/splay-mpris.c -o splay \
		-I/usr/include/dbus-1.0 \
		-I/usr/lib/dbus-1.0/include  \
		-ldbus-1 \
		-lncurses \
		-ltinfo \
		-pthread

	chmod +x splay

.PHONY : clean
clean :
	-rm splay

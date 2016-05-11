DESTDIR?=
PREFIX ?= /usr/local
BIN=xlview

CXX = g++
CXXFLAGS =`fltk-config --cxxflags`
LDFLAGS =`fltk-config --use-images --ldflags`

CXXFLAGS += -march=i486 -mtune=i686 -Os -pipe -ffunction-sections -fdata-sections 
CXXFLAGS += -fomit-frame-pointer -fno-rtti -fno-exceptions -fvisibility-inlines-hidden
LDFLAGS += -Wl,--gc-sections,--strip-all,--build-id=none,--as-needed

all: $(BIN)

clean:
	rm -f $(BIN)
	
install: $(BIN)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	mkdir -p $(DESTDIR)$(PREFIX)/share/pixmaps
	cp $^ $(DESTDIR)$(PREFIX)/bin  
	cp $^.desktop $(DESTDIR)$(PREFIX)/share/applications 
	cp $^.png $(DESTDIR)$(PREFIX)/share/pixmaps 
	
uninstall: $(BIN)
	rm -f $(DESTDIR)$(PREFIX)/bin/$^  
	rm -f $(DESTDIR)$(PREFIX)/share/applications/$^.desktop 
	rm -f $(DESTDIR)$(PREFIX)/share/pixmaps/$^.png 
	
strip:
	strip --strip-all -R .note -R .comment $(BIN)
# if you have sstrip
sstrip:
	sstrip -z $(BIN)
	
# for tiny core linux	
tcz:
	make install DESTDIR=/tmp/$(BIN)1
	mksquashfs /tmp/$(BIN)1 $(BIN).tcz
	#rm -rf /tmp/$(BIN)1
	

CXX = g++
CXXFLAGS = 	-I/usr/include/libxml2\
			$(shell pkg-config --cflags alsa sndfile samplerate jack liblo)\
			$(shell fltk-config --cxxflags)\
			-g -O0 -Wall
LDFLAGS =   -lxml2\
			$(shell pkg-config --libs alsa sndfile samplerate jack liblo)\
			$(shell fltk-config --libs --ldflags)
SOURCES = 	$(wildcard src/*/*.cpp) src/main.cpp
OBJ = $(SOURCES:.cpp=.o)
DEPENDS := $(SOURCES:.cpp=.d)
BIN = tap192
PREFIX = /usr/local

.PHONY: all clean install uninstall

all: $(BIN)

bold := $(shell tput bold)
sgr0 := $(shell tput sgr0)

$(BIN): $(OBJ)
	@printf '\n$(bold)Linking$(sgr0)\n'
	$(CXX) -o $@ $^ $(LDFLAGS)
	@printf '\n'

%.o: %.cpp Makefile
	@printf '\n$(bold)Compilation from $< to $@ $(sgr0)\n'
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPENDS)

clean:
	@rm -f $(OBJ) $(DEPENDS) $(BIN)

install: $(BIN)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/pixmaps
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	cp $< $(DESTDIR)$(PREFIX)/bin/$(BIN)
	cp data/tap192.png $(DESTDIR)$(PREFIX)/share/pixmaps/tap192.png
	cp data/tap192.desktop $(DESTDIR)$(PREFIX)/share/applications/tap192.desktop

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(PREFIX)/share/pixmaps/tap192.png
	rm -f $(DESTDIR)$(PREFIX)/share/applications/tap192.desktop

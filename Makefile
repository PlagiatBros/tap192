CXX = g++
CXXFLAGS = 	-I/usr/include/libxml2\
			$(shell pkg-config --cflags alsa sndfile samplerate jack liblo)\
			$(shell fltk-config --cxxflags)\
			-g -O0 #-Wall
LDFLAGS =   -lxml2\
			$(shell pkg-config --libs alsa sndfile samplerate jack liblo)\
			$(shell fltk-config --libs --ldflags)
SOURCES = 	$(wildcard src/*/*.cpp) src/main.cpp
OBJ = $(SOURCES:.cpp=.o)
DEPENDS := $(SOURCES:.cpp=.d)
BIN = tapeutape
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

install: src/$(BIN)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/pixmaps
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	cp $< $(DESTDIR)$(PREFIX)/bin/$(BIN)
	cp data/tapeutape.png $(DESTDIR)$(PREFIX)/share/pixmaps/tapeutape.png
	cp data/tapeutape.desktop $(DESTDIR)$(PREFIX)/share/applications/tapeutape.desktop

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(PREFIX)/share/pixmaps/tapeutape.png
	rm -f $(DESTDIR)$(PREFIX)/share/applications/tapeutape.desktop

# Copyright (C) 2021 Eduardo Ferreira
# 
# This file is part of synthpp.
# 
# synthpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# synthpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with synthpp.  If not, see <http://www.gnu.org/licenses/>.

CC = g++
MAKE = make

PKGCONF_INCLUDES = $(shell pkg-config gtk4 libadwaita-1 sigc++-3.0 -cflags)
PKGCONF_LIBS = $(shell pkg-config gtk4 libadwaita-1 sigc++-3.0 -libs)

PORTAUDIO_LIBS = -lportaudio -lportmidi -lporttime -ldl

ELF_LIBS = -lbfd


FILE_PARSING_DIR = file_parsing/

SUBDIRS = $(FILE_PARSING_DIR)/file_parsing.o

MAINDIRS = main.o mainwindow.o audio_widget.o debug_widget.o port.o signal_path.o utils.o audio_engine.o counter_widget.o probe_widget.o source_widget.o sink_widget.o delay_widget.o feedback_delay_widget.o gain_widget.o midi_options_dialog.o settings_manager.o widget_manager.o context.o plugin_manager.o

OBJECTS = $(MAINDIRS)

TARGET = output/synthpp

CFLAGS = -O2 -g -std=c++17 -fpermissive $(PKGCONF_INCLUDES)
LDFLAGS = $(PKGCONF_LIBS) $(PORTAUDIO_LIBS) $(ELF_LIBS) -fno-stack-protector -pthread -rdynamic

all: $(OBJECTS) $(TARGET)
	mkdir -p output/plugins
	cp *.sppp output/plugins || :

plugins: $(PLUGINS)

file_parsing.o:
	$(MAKE) -C $(FILE_PARSING_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

main.o: main.cpp mainwindow.h
	$(CC) $(CFLAGS) -c main.cpp -o main.o

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)

debug: $(TARGET)
	G_DEBUG=fatal-criticals $(TARGET)

run: $(TARGET)
	G_DEBUG=fatal-criticals $(TARGET)
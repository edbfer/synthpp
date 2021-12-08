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

PKGCONF_INCLUDES = $(shell pkg-config gtkmm-4.0 -cflags)
PKGCONF_LIBS = $(shell pkg-config gtkmm-4.0 -libs)

OBJECTS = main.o mainwindow.o
TARGET = synthpp

CFLAGS = -O3 -g -std=c++17 $(PKGCONF_INCLUDES)
LDFLAGS = $(PKGCONF_LIBS)

all: $(OBJECTS) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)

run:
	$(TARGET)
# Copyright (C) 2022 Eduardo Ferreira
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
LD = g++
RM = rm

PKGCONF_INCLUDES = $(shell pkg-config gtk4 libadwaita-1 sigc++-3.0 -cflags)
PKGCONF_LIBS = $(shell pkg-config gtk4 libadwaita-1 sigc++-3.0 -libs)

CFLAGS = -g -O0 $(PKGCONF_INCLUDES) -fPIC
LDFLAGS = $(PKGCONF_LIBS)

PLUGIN_TEST_OBJECTS = plugin_test.o

PLUGIN_TEST_TARGET = ../plugin_test.sppp

all: $(PLUGIN_TEST_TARGET)

$(PLUGIN_TEST_TARGET): $(PLUGIN_TEST_OBJECTS)
	$(LD) -shared $(LDFLAGS) -o $(PLUGIN_TEST_TARGET) $(PLUGIN_TEST_OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(PLUGIN_TEST_TARGET) $(PLUGIN_TEST_OBJECTS)
	$(RM) $(PLUGIN_TEST_TARGET) $(PLUGIN_TEST_OBJECTS)
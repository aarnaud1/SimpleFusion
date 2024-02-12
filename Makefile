# Copyright (C) 2024 Adrien ARNAUD
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

CC := g++ -std=c++17 -pedantic -W -Wall
OMP_FLAGS := -fopenmp -DUSE_TIMING
CXX_FLAGS := -O3 -g -march=native -mavx2 -mfma -ffast-math $(OMP_FLAGS)
IFLAGS := -I./ \
	-I./include/ \
	-I./main/include \
	-I./spf/include \
	-I./GL/include \
	-I./thirdparty/ \
	-I./shader \
	-I/usr/include/opencv4
LFLAGS := -L./lib/ -Wl,-rpath,./lib -l3dl-fusion \
	-L./GL/lib -Wl,-rpath,./GL/lib -lOpenGL \
	-lglfw -lopencv_imgcodecs -lopencv_core -lopencv_imgproc \
	-lopencv_highgui -lgflags -lz

OBJ_FILES := $(patsubst spf/src/%.cpp,obj/%.o,$(wildcard spf/src/**/*.cpp))
DIS_FILES := $(patsubst obj/%.o,dis/%.dis,$(wildcard obj/**/*.o))

MAIN_SRC_FILES := \
	main/DataStreamer.cpp \
	main/RenderWindow.cpp \
	main/PointCloudFrameRenderer.cpp \
	main/OPCFrameRenderer.cpp \
	main/MeshRenderer.cpp \
	main/DepthMapRenderer.cpp \
	shader/shader.c

EXEC :=  bin/main bin/syntheticDataset

## -----------------------------------------------------------------------------

all: $(EXEC)

deps: out_files shaders 

dis: lib/lib3dl-fusion.so $(DIS_FILES)
	objdump -dC lib/lib3dl-fusion.so > dis/lib3dl-fusion.dis

out_files:
	$(shell mkdir -p obj/fusion obj/marching_cubes obj/data_types \
		obj/registration obj/math)
	$(shell mkdir -p bin/fusion)
	$(shell mkdir -p lib/fusion)
	$(shell mkdir -p dis/fusion dis/marching_cubes dis/data_types \
		dis/registration dis/math)

bin/%: main/%.cpp deps lib/lib3dl-fusion.so
	$(CC) $(CXX_FLAGS) -o $@ $(IFLAGS) $< $(MAIN_SRC_FILES) \
	$(LFLAGS)

lib/lib3dl-fusion.so: $(OBJ_FILES)
	$(CC) $(CXX_FLAGS) -shared -o $@ $^

obj/%.o: spf/src/%.cpp
	$(CC) $(CXX_FLAGS) -fPIC -c -o $@ $(IFLAGS) $^

dis/%.dis: obj/%.o
	objdump -dC $< > $@

shaders:
	python3 generator.py -i main/shaders/ -o main/include/

clean:
	$(shell rm -rfd bin)
	$(shell rm -rfd obj)
	$(shell rm -rfd dis)
	$(shell rm -rfd lib)

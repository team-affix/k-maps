INCLUDE += -I"./include/"
INCLUDE += -I"./factor-dag/include/"
INCLUDE += -I"./factor-dag/digital-logic/include/"

SOURCE += ./main.cpp
SOURCE += ./factor-dag/factor.cpp

all:
	g++ -std=c++20 -g $(SOURCE) $(INCLUDE) -o main

clean:
	rm -rf ./main

CC=g++
Src:=$(wildcard *.cpp)
Obj:=$(patsubst %.cpp,%,$(Src))
SubObj:=$(patsubst %.cpp,%.o,$(Src))


all:$(Obj)
.PHONY:all


$(Obj):$(SubObj)
	$(CC) -o $@ $@.o


.PHONY:clean
clean:
	rm $(Obj) $(SubObj)
CXX=g++
CXXFLAGS=-std=c++20 -Wall -g -MMD -Ofast
EXEC=chess
CCFILES=$(wildcard src/*.cc)
CCFILES += $(wildcard src/Evaluation/*.cc)
OBJECTS=${CCFILES:.cc=.o}
DEPENDS=${CCFILES:.cc=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC} ${CXXFLAGS}

-include ${DEPENDS}

# Extra recipe that allows us to delete temporary files by saying
# make clean

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}

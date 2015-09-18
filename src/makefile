# Makefile for Graph Generator tool.
# all to make everythin$(CC)g, clean to remove.

CC = g++
CFLAGS = -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -I"C:\tinyxml"
TINYXML = "\tinyXML\tinyXML2.o"


all: GraphGenerator Testing
	
Testing: Testing.o BoolComp.o dotFileMaker.o GraphConfiguration.o NoCollapseConfig.o spdxParser.o Node.o 
	$(CC) Testing.o BoolComp.o dotFileMaker.o GraphConfiguration.o spdxParser.o Node.o NoCollapseConfig.o $(TINYXML) -o Testing

GraphGenerator: Main.o BoolComp.o dotFileMaker.o GraphConfiguration.o spdxParser.o Node.o 
	$(CC) Main.o BoolComp.o dotFileMaker.o GraphConfiguration.o spdxParser.o Node.o $(TINYXML) -o GraphGenerator

BoolComp.o: BoolComp.cpp BoolComp.h
	$(CC) $(CFLAGS) BoolComp.cpp

dotFileMaker.o: dotFileMaker.cpp dotFileMaker.h GraphConfiguration.h Node.h BoolComp.h
	$(CC) $(CFLAGS) dotFileMaker.cpp

GraphConfiguration.o: GraphConfiguration.cpp BoolComp.h Node.h
	$(CC) $(CFLAGS) GraphConfiguration.cpp

Main.o: Main.cpp Node.h BoolComp.h dotFileMaker.h GraphConfiguration.h spdxParser.h spdxNomenclature.h
	$(CC) $(CFLAGS) Main.cpp

Node.o: Node.cpp Node.h BoolComp.h
	$(CC) $(CFLAGS) Node.cpp

spdxParser.o: spdxParser.cpp spdxParser.h spdxNomenclature.h Node.h
	$(CC) $(CFLAGS) spdxParser.cpp

NoCollapseConfig.o: noCollapseConfig.cpp BoolComp.h Node.h GraphConfiguration.h
	$(CC) $(CFLAGS) NoCollapseConfig.cpp

Testing.o: Testing.cpp Node.h BoolComp.h dotFileMaker.h GraphConfiguration.h spdxParser.h spdxNomenclature.h NoCollapseConfig.h
	$(CC) $(CFLAGS) Testing.cpp

clean:
	rm -f *.o GraphGenerator Testing


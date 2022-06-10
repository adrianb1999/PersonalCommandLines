CCFLAGS = -g

MOCA : MOCA.o Parser.o Command.o Help.o Quit.o Utils.o Cd.o Chm.o Cpy.o Del.o Dir.o File.o Move.o Pwd.o Read.o	Write.o \
CreateProc.o KillProc.o ListProcs.o SuspendProc.o Server.o
	g++ -o MOCA MOCA.o Parser.o Command.o Help.o Quit.o Utils.o Cd.o Chm.o Cpy.o Del.o Dir.o File.o Move.o Pwd.o Read.o Write.o \
CreateProc.o KillProc.o ListProcs.o SuspendProc.o Server.o -lpthread

Parser.o: Parser.cpp Parser.h Commands/Command.h Commands/Commons/Help.h Commands/Commons/Quit.h Utils/Utils.h \
Commands/Files/Cd.h Commands/Files/Chm.h Commands/Files/Cpy.h Commands/Files/Del.h  \
Commands/Files/Dir.h Commands/Files/File.h Commands/Files/Move.h Commands/Files/Pwd.h Commands/Files/Read.h \
Commands/Files/Write.h Commands/Processes/CreateProc.h Commands/Processes/KillProc.h Commands/Processes/ListProcs.h Commands/Processes/SuspendProc.h
	g++ -c Parser.cpp $(CCFLAGS)

MOCA.o : MOCA.cpp Commands/Commons/Quit.h Parser.h 
	g++ -c MOCA.cpp $(CCFLAGS)

Command.o : Commands/Command.cpp Commands/Command.h
	g++ -c Commands/Command.cpp $(CCFLAGS) 

Help.o : Commands/Commons/Help.cpp Commands/Commons/Help.h
	g++ -c Commands/Commons/Help.cpp $(CCFLAGS)

Quit.o : Commands/Commons/Quit.cpp Commands/Commons/Quit.h
	g++ -c Commands/Commons/Quit.cpp $(CCFLAGS)

Utils.o : Utils/Utils.cpp Utils/Utils.h
	g++ -c Utils/Utils.cpp $(CCFLAGS)

Cd.o : Commands/Files/Cd.cpp Commands/Files/Cd.h
	g++ -c Commands/Files/Cd.cpp $(CCFLAGS)

Chm.o : Commands/Files/Chm.cpp Commands/Files/Chm.h
	g++ -c Commands/Files/Chm.cpp $(CCFLAGS)

Cpy.o : Commands/Files/Cpy.cpp Commands/Files/Cpy.h
	g++ -c Commands/Files/Cpy.cpp $(CCFLAGS)

Del.o : Commands/Files/Del.cpp Commands/Files/Del.h
	g++ -c Commands/Files/Del.cpp $(CCFLAGS)

Dir.o : Commands/Files/Dir.cpp Commands/Files/Dir.h
	g++ -c Commands/Files/Dir.cpp $(CCFLAGS)

File.o : Commands/Files/File.cpp Commands/Files/File.h
	g++ -c Commands/Files/File.cpp $(CCFLAGS)

Move.o : Commands/Files/Move.cpp Commands/Files/Move.h
	g++ -c Commands/Files/Move.cpp $(CCFLAGS)

Pwd.o : Commands/Files/Pwd.cpp Commands/Files/Pwd.h
	g++ -c Commands/Files/Pwd.cpp $(CCFLAGS)

Read.o : Commands/Files/Read.cpp Commands/Files/Read.h
	g++ -c Commands/Files/Read.cpp $(CCFLAGS)

Write.o : Commands/Files/Write.cpp Commands/Files/Write.h
	g++ -c Commands/Files/Write.cpp $(CCFLAGS)

KillProc.o : Commands/Processes/KillProc.cpp Commands/Processes/KillProc.h 
	g++ -c Commands/Processes/KillProc.cpp $(CCFLAGS)

ListProcs.o : Commands/Processes/ListProcs.cpp Commands/Processes/ListProcs.h 
	g++ -c Commands/Processes/ListProcs.cpp $(CCFLAGS)

SuspendProc.o : Commands/Processes/SuspendProc.cpp Commands/Processes/SuspendProc.h 
	g++ -c Commands/Processes/SuspendProc.cpp $(CCFLAGS)

CreateProc.o : Commands/Processes/CreateProc.cpp Commands/Processes/CreateProc.h
	g++ -c Commands/Processes/CreateProc.cpp $(CCFLAGS)

Server.o : Network/Server.cpp Network/Server.h
	g++ -c Network/Server.cpp $(CCFLAGS)

clean : 
	rm MOCA *.o
BOOSTINCLUDE=-pthread -lmongoclient -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system

main:main.cpp
	g++ -o $@ $^ -lsegmentor -lpostagger -lparser -lboost_regex $(BOOSTINCLUDE)  -lNLPIR -DOS_LINUX

.PHONY:clean

clean:
	-rm -rf main

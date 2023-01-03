convertRC.exe: convertRC.o
	g++ -g $^ -o $@
%.o:%.cpp
	g++ -c -DUNICODE -D_UNICODE $^

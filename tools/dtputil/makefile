CC= g++
CXXFLAGS= -Wall
LINK= -lz -lsmeltext
LINK_FOLDER= -L../../extensions/

all: anmutil dtputil
anmutil:
	$(CC) anmutil.cpp -o anmutil $(CXXFLAGS) $(LINK_FOLDER) $(LINK)
dtputil:
	$(CC) dtputil.cpp -o dtputil $(CXXFLAGS) $(LINK_FOLDER) $(LINK)
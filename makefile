all: Spider splitString.o main TextFilter.o deamonize
	@echo "making spider"
	g++ main.o Spider.o splitString.o BloomFilter.o Saver.o TextFilter.o deamonize.o -o spider -std=c++11

deamonize:deamonize.cpp
	g++ -c deamonize.cpp -std=c++11

Spider:Spider.cpp Spider.h BloomFilter.o TextFilter.o Saver.o 
	g++ -c Spider.cpp -std=c++11

main:main.cpp
	g++ -c main.cpp  -std=c++11

#spider:spider.cpp Spider
#	g++ -c spider.cpp  -std=c++11

splitString.o:splitString.cpp
	g++ -c splitString.cpp  -std=c++11

BloomFilter.o:BloomFilter.cpp BloomFilter.h
	g++ -c BloomFilter.cpp  -std=c++11

TextFilter.o:TextFilter.cpp TextFilter.h
	g++ -c TextFilter.cpp  -std=c++11

Saver.o:Saver.cpp Saver.h
	g++ -c Saver.cpp  -std=c++11

clear:
	rm *.o spider

CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl 

EODMarketData: EODMarketData.o
	$(CC) $(CFLAGS) -o EODMarketData EODMarketData.o

EODMarketData.o: EODMarketData.cpp
	$(CC) $(CFLAGS) -c EODMarketData.cpp

clean:
	rm -rf  EODMarketData  *.o

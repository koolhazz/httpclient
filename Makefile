# File Makefile

CXX := g++
CXXFLAGS := -g -Wall -O
LDFLAGS := -lz

main : url.o tcp.o httprequest.o httpresponse.o httpclient.o main.o
	$(CXX) $(CXXFLAGS) url.o tcp.o httprequest.o httpresponse.o httpclient.o main.o -o main $(LDFLAGS)

main.o : main.cc url.h tcp.h httprequest.h
url.o : url.cc url.h
tcp.o : tcp.cc tcp.h
httprequest.o : httprequest.cc httprequest.h
httpresponse.o : httpresponse.cc httpresponse.h
httpclient.o : httpclient.cc httpclient.h

clean :
	rm  main *.o 
	  

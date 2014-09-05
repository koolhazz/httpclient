#include <iostream>
#include <string>
#include "tcp.h"

using namespace std;

char buffer[1024];

void test_tcp()
{
	TcpClient tc;

	if (tc.connect_server("localhost", "80") != SUCCESS) {
		cout << "connect server failed!" << endl;
		return ;
	}
	
	string get("GET / HTTP/1.1\r\nHost: localhost\r\nAccept: text/*\r\nConnection: close\r\n\r\n");
	
	if (tc.writen(get.c_str(), get.size()) < 0) {
		cout << "send data failed!" << endl;
		return ;
	}
	if (tc.readn(buffer, 1024) < 0) {
		cout << "read data failed" << endl;
		return ;
	}
	
	cout << "revieve data:----------------->" << endl;
	cout << buffer << endl;
}

int main()
{
	test_tcp();
	return 0;
}

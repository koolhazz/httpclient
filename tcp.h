#ifndef _TCP_H_
#define _TCP_H_

#include <netdb.h>                    /* gethostbyname(), gethostbyaddr(), getaddrinfo() */
#include <sys/socket.h>
#include <cstring>

using namespace std;

const unsigned int TIMEOUT_SECONDS = 10;
const unsigned int SLEEP_SECONDS   = 20;

const unsigned int INFATAL_ERROR   = 1;
const unsigned int FATAL_ERROR     = 2;
const unsigned int SUCCESS         = 0;

class TcpClient {
public:
	TcpClient(unsigned int time = TIMEOUT_SECONDS):
	m_sockfd(-1), m_connected(false), m_timewait(time) {
		bzero(&m_hints, sizeof(struct addrinfo));
		m_hints.ai_family = AF_UNSPEC;
		m_hints.ai_socktype = SOCK_STREAM;
	}
	
	unsigned int        readn(char *buffer, size_t n);
	unsigned int        writen(const char *buffer, size_t n);
	unsigned int        connect_server(const char *host, const char *serv);	
	
	bool                close_sock();
	
private:
	int                 m_sockfd;
	
	struct addrinfo     m_hints;
	
	bool                m_connected;
	
	unsigned int        m_timewait;
	unsigned int        do_connect(const struct sockaddr *servaddr, socklen_t addrlen);
	void                set_timeout();
};

#endif /* _TCP_H_ */

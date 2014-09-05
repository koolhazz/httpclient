#include "tcp.h"
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

using namespace std;



unsigned int TcpClient::connect_server(const char *host, const char *serv)
{
	if (m_connected)
		return SUCCESS;
	
	unsigned int ret = FATAL_ERROR;
	bool         infatal = false;		
	struct addrinfo*      result = NULL;
	struct addrinfo*      result_save = NULL;	
		
	int retval = getaddrinfo(host, serv, &m_hints, &result);
	if (retval != 0) {
		cout << "error code: " << retval << endl;
		if (retval == EAI_AGAIN)
			return INFATAL_ERROR;
		return FATAL_ERROR;
	}		
	result_save  = result;
	
	do {
		m_sockfd = socket(result->ai_family, result->ai_socktype,
		                result->ai_protocol);
		if (m_sockfd < 0) {
			continue;
		}
		
		/* once connect success, break */
		ret = do_connect(result->ai_addr, result->ai_addrlen);
		if (ret == SUCCESS)
			break;
		if (ret == INFATAL_ERROR)
			infatal = true;
		
		/* if this addrinfo can't be connected, ignore it */
		close(m_sockfd);
		m_sockfd = -1;	
	
	} while ( (result = result->ai_next) != NULL );
	
	/* free the addrinfo list */
	freeaddrinfo(result_save);
	
	/* try all addr, if no one could be connected, set flag false, */
	/* or got one success, then set it true                        */
	if (m_sockfd < 0 && infatal) {
		m_connected = false;
		ret = INFATAL_ERROR;
	}
	else {
		m_connected = true;
		set_timeout();
	}
	return ret;	
}

unsigned int TcpClient::do_connect(const struct sockaddr *servaddr, socklen_t addrlen)
{
	unsigned int   ret;
	int            flag, error, retval;
	socklen_t      len;
	fd_set         read_set, write_set;
	struct timeval time_val;
	
	flag = fcntl(m_sockfd, F_GETFL, 0);
	if (flag == -1)
		return FATAL_ERROR;
	if (fcntl(m_sockfd, F_SETFL, flag | O_NONBLOCK) == -1)
		return FATAL_ERROR;
	
	error = 0;				
	retval = connect(m_sockfd, servaddr, addrlen);
	if (retval == 0) {     /* connect completed immediately */
		fcntl(m_sockfd, F_SETFL, flag);
		return SUCCESS;
	}
	if (errno != EINPROGRESS) {
		if (errno == EADDRINUSE)
			return INFATAL_ERROR;
		return FATAL_ERROR;
	}
	
	/* errno is EINPROGRESS, then we can do anything else, waiting for connection completed */
	FD_ZERO(&read_set);
	FD_SET(m_sockfd, &read_set);
	write_set = read_set;
	time_val.tv_sec  = m_timewait;
	time_val.tv_usec = 0;
	
	retval = select(m_sockfd+1, &read_set, &write_set, NULL, &time_val);
	/* time out, we can try later */
	if (retval == 0) 
		return INFATAL_ERROR;	
	
	/* check sockfd, there may be some error */
	if (FD_ISSET(m_sockfd, &read_set) || FD_ISSET(m_sockfd, &write_set)) {
		len = sizeof(error);
		if (getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			if (errno == EADDRINUSE || errno == ENETUNREACH || errno == EINTR || errno == EISCONN)
				return INFATAL_ERROR;
			return FATAL_ERROR;
		}
	} 
	else 
		return FATAL_ERROR;
	
	fcntl(m_sockfd, F_SETFL, flag);
	return SUCCESS;
}

void TcpClient::set_timeout()
{
	struct timeval tv;
	tv.tv_sec  = m_timewait;
	tv.tv_usec = 0;
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0 )
		; // do sth. like loging it...
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0 )
		;
}

unsigned int TcpClient::readn(char *buffer, size_t n)
{
	char     *current = buffer;
	size_t   left_bytes = n;
	size_t   read_bytes = 0;
	   
	while (left_bytes > 0) {
		if ( (read_bytes = read(m_sockfd, current, n)) < 0) {
			if (errno == EINTR)  /* interrupt by signal, call read() again */
				read_bytes = 0;
			else 
				return -1;      /* some error */
		}
		else if (read_bytes == 0) /* EOF */ 
			break;
		left_bytes = n - read_bytes;
		current += read_bytes;
	}
	
	return (n - left_bytes);   /* return >= 0 */
}

unsigned int TcpClient::writen(const char *buffer, size_t n)
{
	const char *current = buffer;
	int left_bytes = n;
	int write_bytes = 0;
	
	while (left_bytes > 0) {
		if ((write_bytes = write(m_sockfd, current, n)) <= 0) {
			if (errno == EINTR && write_bytes < 0)
				write_bytes = 0;       /* interrupt by signal, call read() again */
			else 
				return -1;            /* error */
		}
		left_bytes = n - write_bytes;
		current += write_bytes;
	}
	
	return n;
}












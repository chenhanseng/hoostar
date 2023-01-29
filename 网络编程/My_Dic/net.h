#ifndef _NET_H_
#define _NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>		
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sqlite3.h>
#include <time.h>
#include <netdb.h>
#include "debug.h"

#define PORT 5003
#define IP "192.168.20.152"
#define BACKLOG 5
#define QUIT "quit"


#endif 

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#include <unistd.h>

#define   N  32

#define  R  1   // user - register
#define  L  2   // user - login
#define  Q  3   // user - query
#define  H  4   // user - history

#define KU  "my.db"
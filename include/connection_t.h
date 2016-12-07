
#ifndef __CONNECTION_T_H__
	#define __CONNECTION_T_H__

	#include <sys/socket.h>

	typedef struct
  {
    int sock ;
    struct sockaddr address ;
    int addr_len ;
  } connection_t ;

#endif
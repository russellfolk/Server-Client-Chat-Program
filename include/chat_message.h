/**-----------------------------------------------------------------------------
	*
	* Header file for BOOL
	* Defines the boolean data types for convenience.
	*
	* Created: 2013-10-18
	* Revised:
	*
	*---------------------------------------------------------------------------*/

#ifndef __CHAT_MESSAGE_H__
	#define __CHAT_MESSAGE_H__

	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>

	typedef struct
	{
		int msg_type ;
		char payload[ 512 ] ;
	} message ;

#endif

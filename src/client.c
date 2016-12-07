/**-----------------------------------------------------------------------------
	*
	* Chat Client - End user agent
	* Sets up a client for the chat program using pthreads to separate
	* reading from the server, and writing to the server.
	*
	* Created: 2013-10-23
	*
	*---------------------------------------------------------------------------*/

#include "client.h"

int sock ;
message * msg ;
char str[ 512 ] ;
FILE * fp ;
int in_chat = FALSE;

/**
*	main ( int argc , char ** argv )
*	Sets up initial connection with server
*	@parameter - argc, **argv - host address , port number
*	@returns - 0
**/
int
main ( int argc , char ** argv )
{
	//delcare variables	
	int port ;
	sock = -1 ;
	struct sockaddr_in address ;
	struct hostent * host ;
	int user = FALSE ;
	msg = malloc ( sizeof ( message ) ) ;
	pthread_t thread_read ;
	pthread_t thread_write ;
	fp = fopen ( "log.txt" , "w" ) ;

	//initialize ctrl c signal handling
	signal ( SIGINT, catch_ctrlc ) ;

	/* checking commandline parameter */
	if ( argc != 3 )
	{
		printf ( "Run using: %s hostname port\n" , argv[ 0 ] ) ;
		return -1 ;
	}

	/* obtain port number */
	if ( sscanf ( argv[ 2 ] , "%d" , &port ) <= 0 )
	{
		fprintf ( stderr , "%s -- ERROR -- Wrong parameter: port #\n" , argv[ 0 ] ) ;
		return -2 ;
	}

	/* create socket */
	sock = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;

	if ( sock <= 0 )
	{
		fprintf ( stderr , "%s -- ERROR -- Cannot create socket\n" , argv[ 0 ] ) ;
		return -3 ;
	}

	/* connect to server */
	address.sin_family = AF_INET ;
	address.sin_port = htons ( port ) ;
	host = gethostbyname( argv[ 1 ] ) ;
	if ( !host )
	{
		fprintf ( stderr , "%s -- ERROR -- Unknown host: %s\n" , argv[ 0 ] , argv[ 1 ] ) ;
		return -4 ;
	}
	memcpy ( &address.sin_addr , host->h_addr_list[ 0 ] , host->h_length ) ;

	if ( connect ( sock , ( struct sockaddr * ) &address , sizeof ( address ) ) )
	{
		fprintf ( stderr , "%s -- ERROR -- Cannot connect to host: %s\n" , argv[ 0 ] , argv[ 1 ] ) ;
		return -5 ;
	}

	//loop until an acceptable username is entered or -1 for exit
	while ( user == FALSE )
	{
		msg->msg_type = 0 ;
		printf ( "Choose an user name: " ) ;
		scanf ( "%s" , str ) ;
		clear () ;
		if ( strcmp( str , "-1" ) == 0 )
		{
			msg->msg_type = -1 ;
			msg->payload[ 0 ] = '\0' ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
			//close ( sock ) ;
			exit ( 0 ) ;
		}
		strcpy ( msg->payload , str ) ;
		//printf ( "%s\n" , str ) ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		recv ( sock , msg , sizeof ( message ) , 0 ) ;
		if ( msg->msg_type == 1 )
			user = TRUE ;
		else if ( msg->msg_type == -1 )
			printf ( "User already in use.\n" ) ;
	}

	//create read and write threads
	pthread_create ( &thread_read , NULL , client_read , &sock ) ;
	pthread_create ( &thread_write , NULL , client_write , &sock ) ;

	pthread_join ( thread_read , NULL ) ;
	pthread_join ( thread_write , NULL ) ;

	fclose ( fp ) ;

	return 0 ;
}


/**
*	void client_read ( void * s )
*	Displays Menu to user. Then displays commands and prompts based on messages from the server.
*	Also receives and prints output from chat partners message.
*	@returns - void
*	@parameter -  void * s - a pointer to a socket
**/
void *
client_read (void * s )
{
	//initial menu print
	print_menu () ;
	//loop forever to always accept input	
	while ( TRUE )
	{
		recv ( sock , msg , sizeof ( message ) , 0 ) ;

		//output to user based on message from server
		switch ( msg->msg_type )
		{
			//reprint menu
			case  0 :
				print_menu () ;
				break ;
			//print user
			case -1 :
				printf ( "%s\n" , msg->payload ) ;
				break ;
			//Ask to enter a user for chatting
			case -2 :
				printf ( "Choose a user to chat with:\n" ) ;
				break ;
			//Display bad input response
			case -3 :
				printf ( "User does not exist.\n" ) ;
				break ;
			//Display chat request from another user
			case -4 :
				printf ( "Would you like to chat with: " ) ;
				printf ( "%s. " , msg->payload ) ;
				printf ( "y/n\n" ) ;
				break ;
			//Display beginning of chat message
			case -5 :
				in_chat = TRUE ;
				printf ( "Now chatting with: " ) ;
				printf ( "%s\n" , msg->payload ) ;
				break ;
			//Display chat denial
			case -6 :
				printf ( "Chat denied.\n" ) ;
				break ;
			//Display chat ended message
			case -7 :
				in_chat = FALSE ;
				printf ( "Chat ended.\n" ) ;
				break ;
			//Display message from chat partner
			case -8 :
				printf ( "%s" , msg->payload ) ;
				break ;
		}
	}
}

/**
*	void client_write ( void * s )
*	Sends menu choices, and answers to prompts to server.
*	Then sends messages to chat partner 
*	@returns - void
*	@parameter - void * s - a pointer to a socket
**/
void *
client_write ( void * s )
{
	char buffer[512];
	//loop forever to always be able to send to server
	while ( TRUE )
	{
		fgets ( buffer , 512 , stdin ) ;
		
		//while inside a chat, send messages to chat partner
		if ( in_chat == TRUE )
		{
			msg->msg_type = 8 ;
			strcpy ( msg->payload , buffer ) ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
		}
		//while in menu/waiting for chat request
		else
		{
			buffer[ strlen ( buffer ) - 1 ] = '\0' ;

			//if menu response or chat request response			
			if ( strlen ( buffer ) == 1 )
			{
				//ask to list users
				if ( buffer[ 0 ] == '1' )
				{
					msg->msg_type = 1 ;
					msg->payload[ 0 ] = '\0' ;
					send ( sock , msg , sizeof ( message ) , 0 ) ;
				}
				//ask to chat
				else if ( buffer[ 0 ] == '2' )
				{
					msg->msg_type = 2 ;
					msg->payload[ 0 ] = '\0' ;
					send ( sock , msg , sizeof ( message ) , 0 ) ;
				}
				//exit program
				else if ( buffer[ 0 ] == '3' )
				{
					msg->msg_type = 3 ;
					msg->payload[ 0 ] = '\0' ;
					send ( sock , msg , sizeof ( message ) , 0 ) ;
					exit ( 0 ) ;
				}
				//accept chat request
				else if ( buffer[ 0 ] == 'y' || buffer[ 0 ] == 'Y' )
				{
					msg->msg_type = 5 ;
					send ( sock , msg , sizeof ( message ) , 0 ) ;
				}
				//deny chat request
				else if ( buffer[ 0 ] == 'n' || buffer[ 0 ] == 'N' )
				{
					msg->msg_type = 6 ;
					send ( sock , msg , sizeof ( message ) , 0 ) ;
				}
				//default, asks server to send back 0
				else
				{
					msg->msg_type = 0 ;
					msg->payload[ 0 ] = '\0' ;
					send ( sock , msg , sizeof ( message ) , 0 ) ;
				}
			}
			//Send user name to server for chat request
			else
			{
				msg->msg_type = 4 ;
				strcpy ( msg->payload , buffer ) ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
			}
		}
		printf ( "\n" ) ;
	}
}

/**
*	void catch_ctrlc ( int sig )
*	Catches control c signal and then overrides it.
*	While chatting, it breaks the chat.
*	Anywhere else, it notifies the server and exits the program.
*	@parameter - sig - SIGINT signal generated by ^C
*	@returns - void
**/
void
catch_ctrlc ( int sig )
{
	//ctrl_c ends chat session
	if ( in_chat == TRUE )
	{
		msg->msg_type = 7 ;
		msg->payload[ 0 ] = '\0' ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		signal ( SIGINT, catch_ctrlc ) ;
	}
	//ctrl_c ends the program gracefully
	else
	{
		msg->msg_type = 3 ;
		msg->payload[ 0 ] = '\0' ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		//close ( sock ) ;
		exit ( 0 ) ;
	}
}

/**
*	void print_menu ( void )
*	Simplys prints out the standard menu options.
*	@returns - void
**/
void
print_menu ( void )
{
	printf ( "\n\n" ) ;
	printf ( "1. List users\n" ) ;
	printf ( "2. Chat\n" ) ;
	printf ( "3. Exit\n" ) ;
	//printf ( ">  " ) ;
}


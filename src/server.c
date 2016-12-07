/**-----------------------------------------------------------------------------
	*
	* Chat Server
	* Sets up a multithreaded server to handle one on one chats between end users.
	* rusty
	*
	* Created: 2013-10-23
	*
	*---------------------------------------------------------------------------*/

#include "server.h"

/**	void * process ( void * ptr )
*	This is the main process thread for each client connection. It will handle the
* chats that happen for all users.
*	@parameter - * ptr - Connection information
*	@return - void
**/
void
* process ( void * ptr )
{
	connection_t * conn ;
	conn = ( connection_t * ) ptr ;
	message * msg = malloc ( sizeof ( message ) ) ;
	message * tmp_msg = malloc ( sizeof ( message ) ) ;
	user * this_user ;
	int has_user = FALSE ;
	int i = 0 ;
	if ( !ptr )
		pthread_exit ( 0 ) ;
	//	Setup a user name so that the user is registered on the server.
	while ( !has_user )
	{
		recv ( conn->sock , msg , sizeof ( message ) , 0 ) ;
		// a cancel connect is received
		if ( msg->msg_type == -1 )
		{
			close ( conn->sock ) ;
			pthread_exit ( 0 ) ;
		}
		// We're good to go
		else if ( msg->msg_type == 0 && check_user ( msg->payload ) )
		{
			this_user = new_user ( ptr , msg->payload ) ;
			has_user = TRUE ;
			msg->msg_type = 1 ;
			msg->payload[ 0 ] = '\0' ;
			send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
			free ( conn ) ;
		}
		// Try again... :/
		else
		{
			msg->msg_type = -1 ;
			msg->payload[ 0 ] = '\0' ;
			connection_t * conn = (connection_t *) ptr ;
			send ( conn->sock , msg , sizeof ( message ) , 0 ) ;
		}
	}
	// Main loop that will run for this connection
	while ( TRUE )
	{
		recv ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
		switch ( msg->msg_type )
		{
			case 0 ://PRINT MENU
				msg->payload[ 0 ] = '\0' ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 1 ://LIST USER
				list_user ( this_user->socket , msg ) ;
				msg->msg_type = 0 ;
				msg->payload[ 0 ] = '\0' ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 2 ://OPTION FOR CHAT
				list_user ( this_user->socket , msg ) ;
				msg->msg_type = -2 ;
				msg->payload[ 0 ] = '\0' ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 3 ://EXIT PROGRAM
				close ( this_user->socket ) ;
				remove_user ( this_user ) ;
				pthread_exit ( 0 ) ;
				break ;
			case 4 ://USER CHAT REQUEST
				remove_from_file ( this_user->name ) ;
				remove_from_file ( msg->payload ) ;
				this_user->peer = add_peer ( msg->payload ) ;
				// Peer does not exist
				if ( !this_user->peer )
				{
					msg->msg_type = -3 ;
					msg->payload[ 0 ] = '\0' ;
					add_user_to_file ( this_user ) ;
					send ( this_user->socket, msg , sizeof ( message ) , 0 ) ;
					msg->msg_type = 0 ;
					send ( this_user->socket, msg , sizeof ( message ) , 0 ) ;
				}
				// We're gold, baby!
				else
				{
					msg->msg_type = -4 ;
					this_user->peer->peer = this_user ;
					strcpy ( msg->payload , this_user->name ) ;
					send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				}
				break ;
			case 5 : // CHAT ACCEPTED
				msg->msg_type = -5 ;
				strcpy ( msg->payload , this_user->peer->name ) ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				strcpy ( msg->payload , this_user->name ) ;
				send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 6 : // CHAT DENIED
				// add user to file
				add_user_to_file ( this_user ) ;
				add_user_to_file ( this_user->peer ) ;
				msg->msg_type = -6 ;
				msg->payload[ 0 ] = '\0' ;
				send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				msg->msg_type = 0 ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				// remove peer
				this_user->peer->peer = NULL ;
			 	this_user->peer = NULL ;
				break ;
			case 7 :	//	CHAT ENDED
				// add user to file
				add_user_to_file ( this_user ) ;
				add_user_to_file ( this_user->peer ) ;
				msg->msg_type = -7 ;
				msg->payload[ 0 ] = '\0' ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				msg->msg_type = 0 ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				// remove peer
				this_user->peer->peer = NULL ;
			 	this_user->peer = NULL ;
				break ;
			case 8 :	//	Message to peer
				// write "user: "
				tmp_msg->msg_type = -8 ;
				strcpy ( tmp_msg->payload , this_user->name ) ;
				send ( this_user->peer->socket , tmp_msg , sizeof ( message ) , 0 ) ;
				strcpy ( tmp_msg->payload , ": " ) ;
				send ( this_user->peer->socket , tmp_msg , sizeof ( message ) , 0 ) ;
				// write message
				msg->msg_type = -8 ;
				send ( this_user->peer->socket , msg , sizeof ( message ) , 0 ) ;
				break ;

			default :
				break ;
		}

	}

	/* close socket and clean up */
	close ( this_user->socket ) ;
	free ( this_user ) ;
	pthread_exit ( 0 ) ;
}

/**
*	main ( int argc , char ** argv )
*	Keep listening for connections
*	@parameter - argc, **argv - port number for listening
*	@returns - 0
**/
int
main ( int argc , char ** argv )
{
	/* clear out the client list at every run */
	FILE * fp = fopen ( CLIENT_LIST , "w" );
	fclose ( fp ) ;
	head = NULL ;

	int sock = -1 ;
	struct sockaddr_in address ;
	int port ;
	connection_t * connection ;
	pthread_t thread ;

	/* check for command line arguments */
	if ( argc != 2 )
	{
		fprintf ( stderr , "usage: %s port\n" , argv[ 0 ] ) ;
		return -1 ;
	}

	/* obtain port number */
	if ( sscanf ( argv[ 1 ] , "%d" , &port ) <= 0 )
	{
		fprintf ( stderr , "%s: error: wrong parameter: port\n" , argv[ 0 ] ) ;
		return -2 ;
	}

	/* create socket */
	sock = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
	if ( sock <= 0 )
	{
		fprintf ( stderr , "%s: error: cannot create socket\n" , argv[ 0 ] ) ;
		return -3 ;
	}

	/* bind socket to port */
	address.sin_family = AF_INET ;
	address.sin_addr.s_addr = INADDR_ANY ;
	address.sin_port = htons ( port ) ;
	if ( bind ( sock , ( struct sockaddr * )&address , sizeof ( struct sockaddr_in ) ) < 0 )
	{
		fprintf ( stderr , "%s: error: cannot bind socket to port %d\n" , argv[ 0 ] , port ) ;
		return -4 ;
	}

	/* listen on port */
	if ( listen ( sock , 5 ) < 0 )
	{
		fprintf ( stderr , "%s: error: cannot listen on port\n" , argv[ 0 ] ) ;
		return -5 ;
	}

	printf ( "%s: ready and listening\n" , argv[ 0 ] ) ;

	while ( TRUE )
	{
		/* accept incoming connections */
		connection = ( connection_t * ) malloc ( sizeof ( connection_t ) ) ;
		connection->sock = accept ( sock , &connection->address , ( socklen_t * ) &connection->addr_len ) ;
		if ( connection->sock <= 0 )
		{
			free ( connection ) ;
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create ( &thread , 0 , process , ( void * )connection ) ;
			pthread_detach ( thread ) ;
		}
	}

	while ( TRUE ) ;

	return 0 ;
}

/**
*	int check_user ( char * str )
*	Checks if user input is already taken
*	@parameter - * str - user name
*	@returns - int - if taken then false, otherwise true
**/
int
check_user ( char * str )
{
	FILE * fp ;
	fp = fopen( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		if ( ( pos % 4 == 0 ) && strcmp ( str , buffer ) == 0 )
		{
			fclose ( fp ) ;
			return FALSE ;
		}
		pos ++ ;
	}
	fclose ( fp ) ;
	return TRUE ;
}

/**
*	void list_user ( int sock , message * msg )
*	Sends a list of the chat elligable users' names
*	@parameter - sock - socket for sending messages
*	@parameter - * msg - message pointer for sending messages
*	@returns - void
**/
void
list_user ( int sock , message * msg )
{
	FILE * fp ;
	fp = fopen ( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		pos ++ ;
		if ( pos % 4 == 1 )
		{
			msg->msg_type = -1 ;
			strcpy ( msg->payload , buffer ) ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
		}
	}
	fclose ( fp ) ;
}

/**
*	user * add_peer ( char * str )
*	Add user to the servers list
*	@parameter - * str - user
*	@returns - user pointer
**/
user *
add_peer ( char * str )
{
	user * u = head ;
	while ( u )
	{
		if ( strcmp ( u->name , str ) != 0 )
			u = u->next ;
		else
			return u ;
	}
	return FALSE ;
}

/**
*	void * remove_user ( user * u )
*	Remove user from the servers list
*	@parameter - * u -
*	@returns - void
**/
void
remove_user ( user * u )
{
	if ( u != head )
	{
		user * cursor = head ;
		while ( cursor->next != u )
			cursor = cursor->next ;
		cursor->next = u->next ;
	}
	else
		head = head->next ;
	remove_from_file ( u->name ) ;
	free ( u ) ;
}

/**
*	void remove_from_file ( char * str )
*	Remove from registered users
*	@parameter - * str
*	@returns - void
**/
void
remove_from_file ( char * str )
{
	FILE * fp ;
	FILE * tmp ;
	fp = fopen ( CLIENT_LIST , "r" ) ;
	tmp = fopen ( "temp" , "w" ) ;
	char buffer[ 1024 ] ;
	int length = strlen ( str ) ;
	while ( fgets ( buffer , 1024 , fp ) != NULL )
	{
		if ( strncmp ( buffer , str , length ) != 0 )
			fprintf ( tmp , "%s" , buffer ) ;
	}
	fclose ( fp ) ;
	fclose ( tmp ) ;
	remove ( CLIENT_LIST ) ;
	rename ( "temp" , CLIENT_LIST ) ;
}

/**
*	user * new_user ( void * ptr , char * str )
*	Register a new user
*	@parameter - void * ptr -
*	@parameter - * str -
*	@returns - user pointer
**/
user *
new_user ( void * ptr , char * str )
{
	connection_t * conn ;
	conn = ( connection_t * ) ptr ;
	user * this_user ;
	if ( !head )
		head = malloc ( sizeof ( user ) ) ;

	this_user = head ;
	while ( this_user->next )
		this_user = this_user->next ;
	this_user->next = malloc ( sizeof ( user ) ) ;
	this_user = this_user->next ;

	strcpy ( this_user->name , str ) ;
	this_user->socket = conn->sock ;
	this_user->port = ((struct sockaddr_in *)&conn->address)->sin_port ;

	long addr = 0 ;
	char ip_str[ 16 ] ;

	addr = ( long ) ( ( struct sockaddr_in * )&conn->address )->sin_addr.s_addr ;
	sprintf ( ip_str , "%d.%d.%d.%d" ,
						( int ) ( ( addr       ) & 0xff ) ,
						( int ) ( ( addr >>  8 ) & 0xff ) ,
						( int ) ( ( addr >> 16 ) & 0xff ) ,
						( int ) ( ( addr >> 24 ) & 0xff ) ) ;
	strcpy ( this_user->ip , ip_str ) ;

	this_user->next = 0 ;
	this_user->peer = 0 ;

	add_user_to_file ( this_user ) ;
	return this_user ;
}

/**
*	void add_user_to_file ( user * u )
*	Adds the user details to the client list
*	@parameter - * u - pointer the the user
*	@returns void
**/
void
add_user_to_file ( user * u )
{
	FILE * fp = fopen ( CLIENT_LIST , "a" ) ;
	/* print name of user */
	fprintf ( fp , "%s " , u->name ) ;
	/* print ip address of user */
	fprintf ( fp , "%s ", u->ip ) ;
	/* print port # of user */
	fprintf ( fp , "%d " , u->port ) ;
	/* print socket # of user */
	fprintf ( fp , "%d\n", u->socket ) ;
	fclose ( fp ) ;
}

//Only used in testing
//Prints user details
void
print_user ( user * u )
{
	printf ( "Name:   %s\n" , u->name ) ;
	printf ( "IP:     %s\n" , u->ip ) ;
	printf ( "Port:   %d\n" , u->port ) ;
	printf ( "Socket: %d\n" , u->socket ) ;
}

//Print out message to server's terminal
void
print_message ( message * m )
{
	printf ( "Message: %d\n" , m->msg_type ) ;
	printf ( "\"%s\"\n" , m->payload ) ;
}

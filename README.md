Client-Server Chat Application
==============================

This program was written collaboratively by Russell Folk and Nicholas Nelson for
C S 484 – Computer Networks I @ NMSU Fall 2013

This is a console-based, multi-threaded, client-server chat application with a
with a single server that multiple clients connect to and are able to chat
privately with each other.

This program is programmed using the C programming language and is for the Linux
environment.

Configuration
-------------
Build programs using the make command from the top directory.

To run the server, start it with the command ./bin/server $port
$port is a variable for a local port number the server should listen on.
For the sake of geekiness we recommend port 1337


Installation
------------
N/A.

Operation
---------
To connect a client do the following: ./bin/client $host $port
$host is the url of the server, if run on the same machine localhost works
$port is the port the server is listening on.

File Manifest
-------------
<dl>
  <dt>bin</dt>
  <dd>Directory where the compiled files are outputted.</dd>
  <dt>docs</dt>
  <dd>Directory for documentation.</dd>
  <dt>LICENSE.md</dt>
  <dd>BSD license information for this project.</dd>
  <dt>include</dt>
  <dd>Directory for header files.</dd>
  <dt>include/bool.h</dt>
  <dd>Definition for TRUE / FALSE values</dd>
  <dt>include/chat_message.h</dt>
  <dd>Definition of the message passed between client and server.</dd>
  <dt>include/clear.h</dt>
  <dd>Definition of the clear input function.</dd>
  <dt>include/client.h</dt>
  <dd>Definition for the chat client program.</dd>
  <dt>include/connection_t.h</dt>
  <dd>Definition of a struct that holds connection information.</dd>
  <dt>include/server.h</dt>
  <dd>Definition for the chat server program.</dd>
  <dt>include/user.h</dt>
  <dd>Definition of the user information used by the server.</dd>
  <dt>obj</dt>
  <dd>Directory where the compiled object files are outputted.</dd>
  <dt>src</dt>
  <dd>Directory for source files.</dd>
  <dt>src/clear.c</dt>
  <dd>Source code file for stdin buffer clear.</dd>
  <dt>src/client.c</dt>
  <dd>Source code file for the chat client.</dd>
  <dt>src/clear.c</dt>
  <dd>Source code file for the chat server.</dd>
  <dt>Makefile</dt>
  <dd>Compiles the project completely.</dd>
  <dt>README.md</dt>
  <dd>Basic documentation file.</dd>
  <dt>connected_clients.txt</dt>
  <dd>List of connected clients.</dd>
</dl>

Copyright & License
-------------------
Refer to LICENSE.md for all pertinent licensing information.

Contact Information
-------------------
Russell Folk  
rfolk@nmsu.edu  
C S 484 – Fall 2013

Nicholas Nelson
nnelson@nmsu.edu
C S 484 – Fall 2013

Known Bugs
----------
None.

Troubleshooting
---------------


Credits & Acknowledgments
-------------------------
Dr. Satyajayant ‘Jay’ Misra for the challenge of building the program.

Changelog
---------
<dl>
  <dt>v1.0</dt>
  <dd>Things are finally working behind schedule...</dd>
  <dt>v0.0.2</dt>
  <dd>Added all the files for the menu to be displayed.</dd>
  <dd>Added a Makefile to compile the project.</dd>
  <dt>v0.0.1</dt>
  <dd>Initial README file.</dd>
  <dd>Initial LICENSE file. Completed and finalized.</dd>
  <dd>Initial .gitignore file added.</dd>
</dl>

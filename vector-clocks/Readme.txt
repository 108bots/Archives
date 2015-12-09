/**************************************************************************
Vector Clocks Program
Author : Hemanth Srinivasan
**************************************************************************/


This folder contains the following files:
-----------------------------------------

Config file: input.config

Source code: vectorClocks.c

Program file: vectorClocks

Log files: 0_vectorClocks
           1_vectorClocks
           2_vectorClocks
           3_vectorClocks
           4_vectorClocks

Make file: Makefile

Compiling the Program: Type "make" to run the Makefile
----------------------

Running the Program               
-------------------

  Usage: vectorClocks <Process ID> <Config File>


Other Details:  
--------------

Process ID should start from 0 and monotonically increase by 1.

Once the connections are established, the program goes in standby mode waiting for a keypress,
'S' - Starts the Vector Clock Simulation
'T' - Sends a Termination notification to all processes and also resets the clocks & log files.

After “E” number of events are generated, the program again goes into standby, and the above
process can be repeated.

NOTE on connection establishment:
----------------------------------

    Initially, when a server (on a differnt process) is not running, the client trying to connect would 
throw the following error message,

"Error in connecting to the  server
 connect: Connection refused"

Also,differnt processes can be run in any order, however they should have valid process IDs.
Processes maintain only ONE TCP connection between each other. When a duplicate connection is detected,
the redundant connection is closed.
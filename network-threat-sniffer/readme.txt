THREAT SNIFFER V.1.0 (a threat sniffer for telnet)

      

        The threat sniffer is very versatile tool for the system administrator. It allows the admin to monitor all file transfers that take place in his network. If any suspicious files (with predefined words) are being transferred, then the tool sends a mail to admin and also makes a copy of the file. The admin can then use the gui portion of the tool, to view the details like source ip, destination ip, time, logged in user etc....

How to use this tool...

First,run the sniffer program in the console. It should be running as long as the network is being used for continuous monitoring. Then to view the details,every now and then just 
invoke the GUI based version.

enjoy...


BACKEND

f550.c is the main sniffer program. This should be run on one m/c by telnetting into the same m/c
i.e run f550.c from the telnet terminal of that m/c and not from its console.

fcli.c is the client program which is receiving the file.(may be suspicious)
fser.c is the server program which is sending the file.

Run fser.c and fcli.c on different m/c's. There can be any no:of clients.

f550.c requires the pcap library. If not installed install it.

To compile:
     gcc f550.c -lpcap -lpthread.
Run: 
     ./a.out



FRONTEND

The folder gui contains 3 files
  project1.glade
  callbacks.c
  main.c

Open project1.glade using glade. Run the autogen shell script then build the source code, 
after which u overwrite tbe auto-generated callbacks.c and main.c with the ones here.

Leave the folder sniff as it is. Place this folder directly under the root directory. ie "/sniff".
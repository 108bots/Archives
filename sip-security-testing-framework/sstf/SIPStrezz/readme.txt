SIP SECURITY TESTING FRAMEWORK version 0.1
==========================================

Here are the basic steps to create and run a test case:

Step 1: We first determine the set of events that will constitute our test case. 
	The event could either be to send a SIP	message, or receive one or pause. 
	The test case can have any number of these events and in any order.

Step 2: Once we build our events list, we need to configure each of them. Receive 
	and Pause events require minimal configuration where the important part is 
	to specify the type of SIP message we expect to receive and the duration
	of pause event respectively. The send event is the most important one in our 
	test case. Here we decide on the type of message to send, its various header 
	fields, whether it will be well formed or malformed and so on. The next 2 steps
	describe the send event configuration.

Step 3: Once we decide on the type of the message to send, we can design the entire message
	in a template file or use existing templates (see ./template/ directory). We can also 
	define some rules on how the templates should look like and then auto generate them 
	based on those rules. (See ./template/rules/ directory)
	

Step 4: Once the base message template is ready, we can alter the various header fields and
	specify whether some of them will be fuzzed or spoofed. We can do this by replacing 
	a header field by one of various fuzz or spoof keywords. (Ex:SIP_int, SIP_src_ip_spoof
	and so on). The entire list of keywords will be found in ./fuzz/keword.lst. Each of
	the keywords is associated to a particular set of fuzz data values (see ./fuzz/ directory). 
        We can alter this mapping if required or even generate new fuzz data values.

Step 5: Now that all events are setup we are ready to include other test settings. We can specify
	some performance metrics like call rate, call duration, and so on. We can set some SIP 
	protocol parameters like local address/port, target address/port and so on. We can also 
	specify some methods of logging and other external module settings, if included.

Step 6: At this stage, the test case is completely setup and is ready to be launched. Once launched, 
	we can monitor the progress, and analyze the results once the test terminates or we stop it.


README FILES
============

Each directory has a README file, Please read them.

./testcases/README.txt
./fuzz/README.txt
./fuzz/fitness/README.txt
./fuzz/offspring/README.txt
./spoof/README.txt
./extract/README.txt
./template/README.txt
./template/rules_tpl/README.txt
./template/rules/README.txt


SIP, Performance and Logging Settings Help
==========================================


NOTE:   When sending REGISTER messages, Use your local username as the remote 
=====   user name. And, IP of proxy as the Remote IP address. 


Call Duration       : Controls the length of calls. More precisely, this
                      controls the duration of 'pause' instructions in the
                      scenario, if they do not have a 'milliseconds' section.
                      Default value is 0 and default unit is milliseconds.


Local IP address    : Set the local IP address for 'Contact:','Via:', and
                      'From:' headers. Default is primary host IP address.
                      

Simultaneous Calls  : Set the maximum number of simultaneous calls. Once this
                      limit is reached, traffic is decreased until the number
                      of open calls goes down. Default:
                        (3 * call_duration (s) * rate).

Lost Packets        : Set the number of packets to lose by default (scenario
                      specifications override this value).

Maximu Calls        : Stop the test and exit when 'calls' calls are processed

Local Port          : Set the local port number.  Default is a random free port
                      chosen by the system.


Call Rate           : Set the call rate (in calls per seconds).  
                      Default is 10.
                      If the -rp option is used, the call rate is calculated
                      with the period in ms given by the user.

Rate Period         : Specify the rate period for the call rate.  Default is 1
                      second and default unit is milliseconds.  This allows
                      you to have n calls every m milliseconds (by using -r n
                      -rp m).
                      Example: -r 7 -rp 2000 ==> 7 calls every 2 seconds.
                               -r 10 -rp 5s => 10 calls every 5 seconds.

Remote Sending Addr : Set the remote sending address to host:port for sending
                      the messages.

Local User Name     : Set the username part of the resquest URI. Default is
                      'service'.

Transport           : Set the transport mode:
                      - u1: UDP with one socket (default),
                      - un: UDP with one socket per call,
                      - ui: UDP with one socket per IP address The IP
                        addresses must be defined in the injection file.
                      - t1: TCP with one socket,
                      - tn: TCP with one socket per call,
                      

Trace_msg           : Displays sent and received SIP messages in <scenario file
                      name>_<pid>_messages.log

Trace_screen        : Dump statistic screens in the
                      <scenario_name>_<pid>_      s.log file when quitting
                      SIPp. Useful to get a final status report in background
                      mode (-bg option).

Trace_err           : Trace all unexpected messages in <scenario file
                      name>_<pid>_errors.log.

Trace_stat          : Dumps all statistics in <scenario_name>_<pid>.csv file.





# Anything not within the tags will be ignored
# The file defines the default format for all the headers. This will be referred to
# when processing the rules file to generate the new *.tpl file.
# Currently not all, but standard headers are included here.
# The header tags here should match with the header type values used in the 
# rules file.
# For any text extending beyond one line, a line delimiter (###) must be used. See SDP tag for eg:

<INVITE>
INVITE sip:[service]@[remote_ip]:[remote_port] SIP/2.0
</INVITE>

<REGISTER>
REGISTER sip:[service]@[remote_ip]:[remote_port] SIP/2.0
</REGISTER>

<CANCEL>
CANCEL sip:[service]@[remote_ip]:[remote_port] SIP/2.0
</CANCEL>

<OPTIONS>
OPTIONS sip:[service]@[remote_ip]:[remote_port] SIP/2.0
</OPTIONS>

<BYE>
BYE sip:[service]@[remote_ip]:[remote_port] SIP/2.0
</BYE>

<SDP>
v=0 ###
o=user1 53655765 2353687637 IN IP[local_ip_type] [local_ip] ###
s=- ###
c=IN IP[local_ip_type] [local_ip] ###
t=0 0 ###
m=audio 6490 RTP/AVP 8 ###
a=rtpmap:8 PCMA/8000 ###
a=rtpmap:101 telephone-event/8000 ###
a=fmtp:101 0-11,16 
</SDP>

<Via>
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
</Via>

<From>
From: strezzer <sip:strezzer@[local_ip]:[local_port]>;tag=[call_number]
</From>

<To>
To: sut <sip:[service]@[remote_ip]:[remote_port]>
</To>

<ToReg>
To: strezzer <sip:strezzer@[local_ip]:[local_port]>
</ToReg>


<Call-ID>
Call-ID: [call_id]
</Call-ID>

<CSeq>
CSeq: 1 [value] 
</CSeq>

<Contact>
Contact: sip:strezzer@[local_ip]:[local_port]
</Contact>

<Max-Forwards>
Max-Forwards: [value]
</Max-Forwards>

<Subject>
Subject: [value]
</Subject>

<Content-Length>
Content-Length: [value]
</Content-Length>

<180>
SIP/2.0 180 Ringing
</180>

<100>
SIP/2.0 100 Trying
</100>

<200>
SIP/2.0 200 OK
</200>

<301>
SIP/2.0 301 Moved Permanently
</301>

<401>
SIP/2.0 401 Unauthorized
</401>

<407>
SIP/2.0 407 Proxy Authentication Required
</407>

<503>
SIP/2.0 503 Service Unavailable
</503>

<600>
SIP/2.0 600 Busy Everywhere
</600>

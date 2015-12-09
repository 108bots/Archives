Rule #0
register sip:[service]@[remote_ip]:[remote_port] sip/2.0
via: sip/2.0/[transport] [local_ip]:[local_port];branch=[branch]
from[SIP_colon] strezzer <sip[SIP_colon]strezzer[SIP_at][SIP_ip][SIP_colon][local_port]>;tag=[call_number]
to: sut <sip:[service]@[remote_ip][SIP_colon][remote_port]>
call-id: [call_id]
cseq: 1 REGISTER 
contact: sip:strezzer@[local_ip]:[local_port]
content-length: 0

Rule #1
invite sip:[service]@[remote_ip]:[remote_port] sip/2.0
via: sip/2.0/[transport] [local_ip]:[local_port];branch=[branch]
from: strezzer <sip:strezzer@[local_ip]:[local_port][SIP_rAngBrckt];tag=[call_number]
to: sut <sip:[service]@[remote_ip]:[remote_port]>
call-id: [call_id]
cseq: 1 [value] 
contact: sip:strezzer@[local_ip]:[local_port]
content-length: [SIP_int]

v=0 
o=user1 53655765 2353687637 in ip[local_ip_type] [local_ip] 
s=- 
c=in ip[local_ip_type] [local_ip] 
t[SIP_equal]0 0 
m=[SIP_string] 6490 rtp/avp 8 
a=rtpmap:8 pcma/8000 
a=rtpmap:101 telephone-event/8000 
a=fmtp:101 0-11,16 


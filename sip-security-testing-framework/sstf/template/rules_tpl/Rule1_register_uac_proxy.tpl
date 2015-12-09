register sip:[service]@[remote_ip]:[remote_port] sip/2.0
via: sip/2.0/[transport] [local_ip]:[local_port];branch=[branch]
from[SIP_colon] strezzer <sip[SIP_colon]strezzer[SIP_at][SIP_ip][SIP_colon][local_port]>;tag=[call_number]
to: sut <sip:[service]@[remote_ip][SIP_colon][remote_port]>
call-id: [call_id]
cseq: 1 REGISTER 
contact: sip:strezzer@[local_ip]:[local_port]
content-length: 0


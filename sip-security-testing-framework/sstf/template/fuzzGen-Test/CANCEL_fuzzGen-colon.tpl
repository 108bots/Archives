CANCEL sip:[service]@[remote_ip]:[remote_port] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: SSTF <sip:strezzer@[local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[service]@[remote_ip]:[remote_port]>
Call-ID[SIP_colon] [call_id]
CSeq: 1 CANCEL
Contact: sip:strezzer@[local_ip]:[local_port]
Content-Length[SIP_colon]0


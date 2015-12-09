CANCEL sip:[service]@[remote_ip]:[remote_port] SIP[SIP_slash]2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: SSTF <sip:strezzer@[local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[service]@[remote_ip]:[remote_port]>
Call-ID: [call_id]
CSeq: 1 CANCEL
Contact: sip:strezzer@[local_ip]:[local_port]
Content-Length: [SIP_slash]


CANCEL sip:[service]@[remote_ip]:[remote_port] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: [SIP_string] <sip:[SIP_string]@[local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[service]@[remote_ip]:[remote_port]>
Call-ID: [call_id]
CSeq: 1 [SIP_string]
Contact: sip:6000@[local_ip]:[local_port]
Content-Length: 0


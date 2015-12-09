BYE sip:[service]@[remote_ip]:[remote_port] [SIP_version]
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: strezzer [SIP_lAngBrckt]sip:6000@[local_ip]:[local_port]>;tag=[call_number]
To: sut <[SIP_uri]:[remote_port]>
Call-ID: [call_id]
CSeq: 1 [SIP_string] 
Contact: sip:6000@[SIP_ip]:[local_port]
Max-Forwards: 70
Subject: Strezz Test
Content-Length: 0


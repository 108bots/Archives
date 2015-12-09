BYE sip:[service]@[remote_ip]:[remote_port] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: strezzer <sip:6000@[local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[service]@[remote_ip]:[remote_port]>
Call-ID: [call_id]
CSeq: 1 BYE 
Contact: sip:6000@[local_ip]:[local_port]
Max-Forwards: 70
Subject: Strezz Test
Content-Length: 0


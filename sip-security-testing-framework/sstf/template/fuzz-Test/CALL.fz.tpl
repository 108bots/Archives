INVITE sip:[service]@[remote_ip]:[remote_port] SIP/2.0
Via: SIP/2.0/[transport] [SIP_ip]:[local_port];branch=[SIP_string]
From: strezzer <sip:6000@[local_ip][SIP_colon][local_port]>;tag=[call_number]
To: [SIP_string] <sip:[service]@[remote_ip]:[remote_port]>
Call-ID: [call_id]
CSeq: 1 [SIP_int]
Contact[SIP_colon] sip:6000@[local_ip]:[local_port]
Max-Forwards: [SIP_int]
Subject: Strezz Test
Content-Type: application[SIP_slash]sdp
Content-Length: [len]

v=0
o=user1 53655765 2353687637 IN IP[local_ip_type] [local_ip]
s=-
c[SIP_equal]IN IP[local_ip_type] [local_ip]
t=0 0
m=audio 1669 RTP/AVP 8
a=rtpmap:8 PCMA/8000
a=rtpmap:101 telephone-event/8000
a=fmtp:101 0-11,16

ACK sip:[service]@[remote_ip]:[SIP_int] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: [SIP_string] <sip:6000[SIP_at][local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[service]@[remote_ip]:[remote_port][SIP_rAngBrckt]]
Call-ID: [call_id]
CSeq: 1 ACK 
Contact: sip:6000@[SIP_ip]:[local_port]
Max-Forwards: [SIP_int]
Subject: Strezz Test
Content-Length[SIP_colon] 0

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


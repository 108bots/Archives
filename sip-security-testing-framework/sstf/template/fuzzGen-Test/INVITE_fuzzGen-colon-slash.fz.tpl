INVITE sip:[service]@[remote_ip]:[remote_port] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: SSTF <sip:[SIP_slash]@[local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[service]@[remote_ip]:[remote_port]>
Call-ID: [SIP_colon]
CSeq: 1 INVITE
Contact: sip:6000@[local_ip]:[local_port]
Max-Forwards: 70
Subject: SSTF Test
Content-Type: application/sdp
Content-Length: [len]

v=0
o=user1 53655765 2353687637 IN IP[local_ip_type] [SIP_slash]
s=-
c=IN IP[SIP_colon] [local_ip]
t=0 0
m=audio 6490 RTP/AVP 8
a=rtpmap:8 PCMA/8000
a=rtpmap:101 telephone-event/8000
a=fmtp:101 0-11,16

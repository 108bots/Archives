REGISTER sip[remote_ip] SIP/2.0
Via: SIP/2.0/[transport] [SIP_ip]:[local_port];branch=[branch]
From: [SIP_string] <sip:6000@[local_ip]:[local_port]>;tag[SIP_equal][call_number]
To: [SIP_string] <sip:6000@[local_ip]:[local_port]>
Call-ID: [SIP_at]
CSeq: 1 REGISTER
Contact: sip:strezzer@[local_ip][SIP_colon][local_port]
Content-Length: [SIP_int]


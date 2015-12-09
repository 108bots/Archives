CANCEL sip:[SIP_dst_user_spoof]@[remote_ip]:[remote_port] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: strezzer <sip:[SIP_src_user_spoof]@[local_ip]:[local_port]>;tag=[call_number]
To: sut <sip:[SIP_dst_user_spoof]@[remote_ip]:[remote_port]>
Call-ID: [call_id]
CSeq: 1 CANCEL
Contact: sip:[SIP_src_user_spoof]@[local_ip]:[local_port]
Content-Length: 0


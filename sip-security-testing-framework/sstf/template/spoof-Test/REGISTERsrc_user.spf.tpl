REGISTER sip:[remote_ip] SIP/2.0
Via: SIP/2.0/[transport] [local_ip]:[local_port];branch=[branch]
From: SSTF <sip:[SIP_src_user_spoof]@[local_ip]:[local_port]>;tag=[call_number]
To: SSTF <sip:[SIP_src_user_spoof]@[local_ip]:[local_port]>
Call-ID: [call_id]
CSeq: 1 REGISTER
Contact: sip:[SIP_src_user_spoof]@[local_ip]:[local_port]
Expires: 60
Content-Length: 0

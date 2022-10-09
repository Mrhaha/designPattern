#!/bin/sh

echo "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" > msg_name.xml
echo "<root>" >> msg_name.xml
cat ../../include/protocol/server_cmd_extend_define.h | grep '=' | grep ',' | grep "/" | sed  "s/=/ /g" | sed "s/,/ /g" | sed "s/\// /g" | awk '{print "    <msg_name id=\"" $2 "\" name=\"" $3 "\" cmd=\"" $1 "\"/>"}' >> msg_name.xml
cat ../../include/protocol/server_cmd_logic_define.h | grep '=' | grep ',' | grep "/" | sed  "s/=/ /g" | sed "s/,/ /g" | sed "s/\// /g" | awk '{print "    <msg_name id=\"" $2 "\" name=\"" $3 "\" cmd=\"" $1 "\"/>"}' >> msg_name.xml
cat ../../include/protocol/server_cmd_cross_define.h | grep '=' | grep ',' | grep "/" | sed  "s/=/ /g" | sed "s/,/ /g" | sed "s/\// /g" | awk '{print "    <msg_name id=\"" $2 "\" name=\"" $3 "\" cmd=\"" $1 "\"/>"}' >> msg_name.xml
cat ../../include/protocol/server_cmd_admin_define.h | grep -a '=' | grep -a ',' | grep -a "/" | sed  "s/=/ /g" | sed "s/,/ /g" | sed "s/\// /g" | awk '{print "    <msg_name id=\"" $2 "\" name=\"" $3 "\" cmd=\"" $1 "\"/>"}' >> msg_name.xml
echo "</root>" >> msg_name.xml

mv msg_name.xml ../../../msg_name.xml


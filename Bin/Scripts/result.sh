#!/bin/sh

cat ../../include/protocol/server_errcode.h | grep '=' | grep ","| grep "/"| sed  "s/=/ /g" | sed "s/,/ /g" | sed "s/\// /g"  | awk '{print $1 " = " $2}' > rsp_code.lua

echo "ErrorMsg = " > ErrorMsg.lua
echo "{" >> ErrorMsg.lua
cat ../../include/protocol/server_errcode.h | grep '=' | grep ","| grep "/"| sed  "s/=/ /g" | sed "s/,/ /g" | sed "s/\// /g"  | awk '{print "    [" $1 "] = \"" $3 "\","}' >> ErrorMsg.lua
echo "}" >> ErrorMsg.lua


mv rsp_code.lua ../../../naruto_client/src/enum/rsp_code.lua

mv ErrorMsg.lua ../../../naruto_client/src/helper/ErrorMsg.lua

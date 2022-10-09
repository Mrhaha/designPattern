#！/bin/sh

set -e

ulimit -c unlimited
ulimit -n 65535

SHELL_PATH=$(cd `dirname $0`; pwd)

LOGIC=${SHELL_PATH}/LogicServer/bin/logic_svr
CROSS=${SHELL_PATH}/CrossServer/bin/cross_svr
CONN=${SHELL_PATH}/Connector/bin/connector_svr
# QUEUE=${SHELL_PATH}/QueueServer/bin/queue_svr

echo "Shutting down ${SHELL_PATH} server "
for d in `ps -ef|grep ${SHELL_PATH} |grep -v grep |awk '{print $2}'`; do
    kill -USR2 $d
done

sleep 10


echo "Start cross server"
chmod a+x ${CROSS}
cd `dirname ${CROSS}`
${CROSS} -d

sleep 3
echo "Start logic server"
chmod a+x ${LOGIC}
for d in `find -P ${SHELL_PATH}/LogicServer -maxdepth 1 -type d -iregex ".*[0-9]+$"`; do
	cp -f ${LOGIC} $d/
	echo "Copy Logic To $d"
	chmod a+x $d/logic_svr
	cd $d
	$d/logic_svr -d
done

sleep 3
echo "Start conn server"
chmod a+x ${CONN}
cd `dirname ${CONN}`
${CONN} -d

# sleep 3
# echo "Start queue server"
# chmod a+x ${QUEUE}
# cd `dirname ${QUEUE}`
# ${QUEUE} -d

sleep 5
echo "done"
ps -ef | grep ${SHELL_PATH} | grep -v grep


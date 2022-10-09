#！/bin/sh

set -e

SHELL_PATH=$(cd `dirname $0`; pwd)
SQL_BIN=${SHELL_PATH}/../Tools/sql_alchemy

chmod a+x ${SQL_BIN}

${SQL_BIN} code ${SHELL_PATH}/../LogicServer/config ${SHELL_PATH}/../LogicServer/sql/xml ${SHELL_PATH}/../../server/logic_server/record
${SQL_BIN} code ${SHELL_PATH}/../LogicServer/config ${SHELL_PATH}/../LogicServer/sql/cross ${SHELL_PATH}/../../server/cross_server

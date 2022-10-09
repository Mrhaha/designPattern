#！/bin/sh

set -e

SHELL_PATH=$(cd `dirname $0`; pwd)
GEN_BIN=${SHELL_PATH}/../Tools/sql_alchemy

chmod a+x ${GEN_BIN}

${GEN_BIN} update ${SHELL_PATH}/../LogicServer/config ${SHELL_PATH}/../LogicServer/sql/xml
${GEN_BIN} update ${SHELL_PATH}/../LogicServer/config ${SHELL_PATH}/../LogicServer/sql/cross

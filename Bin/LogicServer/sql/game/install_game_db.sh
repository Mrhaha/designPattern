#!/bin/sh

SHELL_PATH=$(cd `dirname $0`; pwd)

MYSQL_BIN=mysql
SQL_ALCHEMY=$1
DB_HOST=$2
DB_PORT=$3
DB_USER=$4
DB_PWD=$5
DB_NAME=$6

cd ${SHELL_PATH}

if [ `$MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -e "show databases"|grep -w $DB_NAME|wc -l` == 0 ];then
    $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -e "CREATE DATABASE $DB_NAME DEFAULT CHARSET=utf8;"
    
    cat game_table.sql | $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -D$DB_NAME
    
    sh update_sql.sh $SQL_ALCHEMY $DB_HOST $DB_PORT $DB_USER $DB_PWD $DB_NAME

else
	echo "the $DB_NAME exist"
	exit 1
fi
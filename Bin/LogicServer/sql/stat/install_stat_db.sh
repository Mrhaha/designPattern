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
    cat stat_db.sql | sed -e "s/STAT_DATABASE/$DB_NAME/g"| $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -D$DB_NAME
    cat stat_db_partition.sql | sed -e "s/STAT_DATABASE/$DB_NAME/g"| $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -D$DB_NAME
else
	echo "the $DB_NAME exist"
	exit 1
fi

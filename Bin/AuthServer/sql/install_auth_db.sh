#!/bin/sh

SHELL_PATH=$(cd `dirname $0`; pwd)

MYSQL_BIN=mysql
SQL_ALCHEMY=$1
DB_HOST=$2
DB_PORT=$3
DB_USER=$4
DB_PWD=$5
DB_NAME=$6
MAX_TABLE_NUM=32

cd ${SHELL_PATH}

if [ `$MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -e "show databases"|grep -w $DB_NAME|wc -l` == 0 ];then
    $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -e "CREATE DATABASE $DB_NAME DEFAULT CHARSET=utf8;"
    
    cat auth_uin.sql | $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -D$DB_NAME
    
    i=0
    while [ $i -lt $MAX_TABLE_NUM ]
    do
        cat auth_db.sql | sed "s/TABLENO/$i/g" | $MYSQL_BIN -P${DB_PORT} -u$DB_USER -p$DB_PWD -h$DB_HOST -D$DB_NAME
        i=`expr $i + 1`
    done

else
	echo "the $DB_NAME exist"
	exit 1
fi
#!/bin/sh

SQL_ALCHEMY=$1
MYSQL_HOST=$2
MYSQL_PORT=$3
MYSQL_USER=$4
MYSQL_PWD=$5
MYSQL_NAME=$6

if [ `mysql -P${MYSQL_PORT} -u$MYSQL_USER -p$MYSQL_PWD -h$MYSQL_HOST -e "show databases"|grep -w $MYSQL_NAME|wc -l` == 0 ];then
    echo "no databases" 1>&2
    exit 1
fi

SQLROOT=$(cd `dirname $0`; cd ..; pwd)

cd $SQLROOT

mkdir -p $SQLROOT/xml
mkdir -p $SQLROOT/cross
mkdir -p $SQLROOT/create

cp -r $SQLROOT/game/sql_global_info.xml $SQLROOT/create/sql_global_info.xml
sed -i -e "s/MYSQL_HOST/$MYSQL_HOST/g" -e "s/MYSQL_PORT/$MYSQL_PORT/g"  -e "s/MYSQL_USER/$MYSQL_USER/g"  -e "s/MYSQL_PASSWORD/$MYSQL_PWD/g"  -e "s/MYSQL_NAME/$MYSQL_NAME/g" $SQLROOT/create/sql_global_info.xml

echo "begin update ... "
chmod a+x $SQL_ALCHEMY
$SQL_ALCHEMY update $SQLROOT/create $SQLROOT/xml
$SQL_ALCHEMY update $SQLROOT/create $SQLROOT/cross

echo "done" 


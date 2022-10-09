#!/bin/bash

#0 3 * * * /bin/bash /opt/Script/package.sh

last_day=7
last_day_ymd=`date -d "${last_day} days ago" +%Y%m%d`
last_day_f=`date -d "${last_day} days ago" +%F`

logic_log_dir=/data/log/LogicServer
conn_log_dir=/data/log/Connector
api_log_dir=/data/www/serviceapi/storage/logs
cross_log_dir=/data/log/CrossServer
cross_bill_dir=/data/bill/CrossServer
auth_bill_dir=/data/bill/AuthServer
auth_log_dir=/data/log/AuthServer

file_package()
{
    if [ -n "$dir" ] && [ -n "$package_day" ]
    then
        if [ -d "$dir" ]
        then
            cd $dir
			echo $dir
			tar --remove-files -czf $package_day.tar.gz *$package_day* --exclude=*.tar.gz          
        fi
    fi
}

if [ -d $logic_log_dir ]
then
    for d in `find -P $logic_log_dir -maxdepth 1 -type d -iregex ".*[0-9]+$"`
    do
        for f in trace mysql slow
        do
            dir=$d/$f
            package_day=$last_day_ymd

            file_package        
        done
    done
fi

for log in $conn_log_dir $api_log_dir $cross_log_dir $cross_bill_dir $auth_bill_dir $auth_log_dir
do
    if [ -d $log ]
    then
        dir=$log
        package_day=$last_day_f

        file_package
    fi
done

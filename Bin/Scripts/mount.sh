#!/bin/bash

mkfs.ext3 /dev/vdb
mkdir /data
chmod a+w /data
mount /dev/vdb /data

#添加到开机自动挂载
echo "/dev/vdb             /data                ext3       noatime,acl,user_xattr 1 0" >> /etc/fstab

#!/bin/bash

dd if=/dev/zero of=/swapfile bs=4096 count=1572864
mkswap /swapfile
swapon /swapfile
echo "/swapfile swap swap defaults 0 0" >> /etc/fstab


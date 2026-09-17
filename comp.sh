#!/bin/bash

gcc list.c device.c config.c main.c
chown root:root a.out
chmod +s a.out 

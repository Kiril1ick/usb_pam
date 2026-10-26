#!/bin/bash

gcc ./crypto/stribog/gost_3411_2012_const.h ./crypto/stribog/gost_3411_2012_calc.h ./crypto/stribog/gost_3411_2012_calc.c ./crypto/crypt.h ./crypto/crypt.c list.c device.c config.c main.c -g
chown root:root a.out
chmod +s a.out 

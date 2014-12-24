#! /bin/bash

sudo dd if=./barebox-flash-image of=$1 bs=512 seek=1
#!/bin/bash

boostlib=/usr/local/lib
echo $boostlib
export LD_LIBRARY_PATH=`pwd`/lib:`pwd`/ToolDAQ/zeromq-4.0.7/lib:$boostlib:$LD_LIBRARY_PATH


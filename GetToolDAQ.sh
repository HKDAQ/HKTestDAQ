#!/bin/bash

mkdir ToolDAQ
cd ToolDAQ

git clone https://github.com/ToolDAQ/ToolDAQFramework.git
git clone https://github.com/ToolDAQ/zeromq-4.0.7.git

cd zeromq-4.0.7

./configure --prefix=`pwd`
make
make install

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/lib


cd ../ToolDAQFramework

more Makefile | sed s:'BoostLib= -L ../boost_1_60_0/install/lib -lboost_date_time':'BoostLib= -L /usr/lib64/boost148 -lboost_date_time': | sed s:'BoostInclude= -I ../boost_1_60_0/install/include/':'BoostInclude= -I  /usr/include/boost148/': >Makefile.tmp
mv Makefile.tmp Makefile 
make clean
make

cd ../../
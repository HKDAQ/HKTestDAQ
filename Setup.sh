#!/bin/bash

boostlib=/usr/local/lib

cd ToolDAQ/zeromq-4.0.7/lib

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`:$booslib
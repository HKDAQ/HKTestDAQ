CUDA_HOME=/usr/local/cuda-7.5/targets/x86_64-linux
CUDAINC = -I$(CUDA_HOME)/include -I.
CUDALIB = -L$(CUDA_HOME)/lib -lcudart

NVCCFLAGS       := -lineinfo -arch=sm_20 --ptxas-options=-v --use_fast_math



ToolDAQFrameworkPath=ToolDAQ/ToolDAQFramework

ZMQLib= -L ToolDAQ/zeromq-4.0.7/lib -lzmq 
ZMQInclude= -I ToolDAQ/zeromq-4.0.7/include/ 

#Change to in the future
#BoostLib= -L ToolDAQ/boost_1_60_0/install/lib -lboost_date_time -lboost_serialization
#BoostInclude= -I ToolDAQ/boost_1_60_0/install/include

BoostLib= -L /usr/lib64/boost148 -lboost_date_time
BoostInclude= -I  /usr/include/boost148/

#BoostLib= -L /usr/local/lib -lboost_date_time
#BoostInclude= -I /usr/local/include/

DataModelInclude =
DataModelLib =

MyToolsInclude = $(CUDAINC)
MyToolsLib = $(CUDALIB)

all: lib/libMyTools.so lib/libToolChain.so lib/libStore.so include/Tool.h  lib/libDataModel.so RemoteControl NodeDaemon

	g++ src/main.cpp -o main -I include -L lib -lStore -lMyTools -lToolChain -lDataModel -lLogging -lServiceDiscovery -lpthread $(DataModelInclude) $(MyToolsInclude) $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)


lib/libStore.so:

	cp $(ToolDAQFrameworkPath)/src/Store/Store.h include/
	g++ -fPIC -shared -I inlcude $(ToolDAQFrameworkPath)/src/Store/Store.cpp -o lib/libStore.so


include/Tool.h:

	cp $(ToolDAQFrameworkPath)/src/Tool/Tool.h include/


lib/libToolChain.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libMyTools.so lib/libServiceDiscovery.so lib/libLogging.so


	cp $(ToolDAQFrameworkPath)/src/ToolChain/*.h include/
	g++ -fPIC -shared $(ToolDAQFrameworkPath)/src/ToolChain/ToolChain.cpp -I include -lpthread -L lib -lStore -lDataModel -lMyTools -lServiceDiscovery -lLogging -o lib/libToolChain.so $(DataModelInclude) $(MyToolsInclude) $(ZMQLib) $(ZMQInclude)  $(BoostLib) $(BoostInclude)



clean: 
	rm -f include/*.h
	rm -f lib/*.so
	rm -f main
	rm -f RemoteControl
	rm -f NodeDaemon
	rm -f UserTools/CUDA/daq_code.o

lib/libDataModel.so: lib/libStore.so lib/libLogging.so

	cp DataModel/DataModel.h include/
	g++ -fPIC -shared DataModel/DataModel.cpp -I include -L lib -lStore -lLogging -o lib/libDataModel.so $(DataModelInclude) $(DataModelLib) $(ZMQLib) $(ZMQInclude) $(BoostLib) $(BoostInclude)



lib/libMyTools.so: lib/libStore.so include/Tool.h lib/libDataModel.so lib/libLogging.so UserTools/CUDA/daq_code.o

	cp UserTools/*.h include/
	cp UserTools/Factory/*.h include/
	g++  -shared -fPIC UserTools/Factory/Factory.cpp UserTools/CUDA/daq_code.o -I include -L lib -lStore -lDataModel -o lib/libMyTools.so $(MyToolsInclude) $(MyToolsLib) $(ZMQLib) $(ZMQInclude) $(BoostLib) $(BoostInclude)


RemoteControl:
	cp $(ToolDAQFrameworkPath)/RemoteControl ./

NodeDaemon:
	cp $(ToolDAQFrameworkPath)/NodeDaemon ./

lib/libServiceDiscovery.so: lib/libStore.so
	cp $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.h include/
	g++ -shared -fPIC -I include $(ToolDAQFrameworkPath)/src/ServiceDiscovery/ServiceDiscovery.cpp -o lib/libServiceDiscovery.so -L lib/ -lStore  $(ZMQInclude) $(ZMQLib) $(BoostLib) $(BoostInclude)

lib/libLogging.so: lib/libStore.so
	cp $(ToolDAQFrameworkPath)/src/Logging/Logging.h include/
	g++ -shared -fPIC -I include $(ToolDAQFrameworkPath)/src/Logging/Logging.cpp -o lib/libLogging.so -L lib/ -lStore $(ZMQInclude) $(ZMQLib) $(BoostLib) $(BoostInclude)

UserTools/CUDA/daq_code.o:

	cp UserTools/CUDA/*.h include/
	nvcc -c --shared -Xcompiler -fPIC UserTools/CUDA/daq_code.cu -o UserTools/CUDA/daq_code.o -I include $(CUDAINC) $(NVCCFLAGS) $(CUDALIB)
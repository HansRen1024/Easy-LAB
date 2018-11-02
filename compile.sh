#!/usr/bin/env bash
export LD_LIBRARY_PATH=/home/hans/caffe_LAB/build/lib/:$LD_LIBRARY_PATH
g++ -I/home/hans/LAB-master/include -I/home/hans/LAB-master/build/src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -O3 -D CPU_ONLY -MMD -MP -MF"alignment_tools.d" -MT"alignment_tools.o" -o "alignment_tools.o" "./alignment_tools.cpp"
g++ -L/home/hans/LAB-master/build/lib -o "caffe_LAB"  ./alignment_tools.o   -lcaffe -lglog -lopencv_core -lopencv_contrib -lprotobuf -lboost_system -lopencv_imgproc -lopencv_highgui

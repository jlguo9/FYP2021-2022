# Version

openCV 3.4.16

# Installation

1. ```tar -zxvf opencv-3.4.16.tar.gz```
2. run ```download.sh``` to install dependencies
3. make a new folder (named "build" for example) under ```opencv-3.4.16/```
4. move ```cmake_build.sh``` to ```build/``` and run it under ```build/```
5. ```make -jn``` where ```n``` is the number of cores you want to use & ```make install```
6. create a file named ```opencv.conf``` under ```/etc/ld.so.conf.d/```
7. write the following into opencv.conf:```/usr/local/lib```
8. ```sudo ldconfig```
9. ```sudo vim /etc/bash.bashrc```
10. write ```export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig``` at the end of ```bash.bashrc```
11. ```source /etc/bash.bashrc``` & ```sudo updatedb```

# FYP2021-2022 Design of a Prototype to Measure the Perceptual Quality of Online Games
This is the repository for our FYP2021-2022

# Structure:
    |- WAH2102_code.zip
        |-- src
            |-- Makefile
            |-- heatmap.py
            |-- interpolate.py
            |-- optimize.py
            |-- main.cpp
            |-- offline_generation.cpp
            |-- offline_generation.h
            |-- online_generation.cpp
            |-- online_generation.h
            |-- merge_surface.cpp
            |-- merge_surface.h
            |-- util.cpp
            |-- util.h
        |-- pingpong_udp
            |-- Makefile
            |-- crc32.h
            |-- PacketHeader.h
            |-- wReceiver.cpp
            |-- wSender.cpp
        |-- pingpong_tcp
            |-- data
                |-- ball.txt
            |-- Makefile
            |-- game.c
            |-- game.h
            |-- gameclient.c
            |-- gameserver.c

# Program in Folder "src"
This is the implementation of Dr. Xu's algorithm for measuring perceptual quality using JND Surfaces. 
Sections included: offline generation, online transformation, merging two surfaces, finding optimal points

## Compile the C++ program

    $ make

## To generate a JND Surface offline:

    $ ./main.exe -g <output path> <threshold> <smallest reference> <largest reference> <smallest modification> <largest modification>

Then the program will repeatedly ask for subjective test results at certain points, until an error smaller than threshold can be achieved.
A csv file storing awareness at each point and a heatmap will be generated.

## To transform an offline-generated JND Surface into online version:

    $ ./main.exe -o <input filepath> <output folder> <smallest reference> <largest reference> <smallest modification> \
    <largest modification> <original loss rate> <thread_number(optional)>

Input filepath is the path of the csv-format JND Surface generated offline. output folder is the folder where the resultant JND Surface will be put.
The last parameter "thread number" is optional; if a number is given, the online transformation will run in multi-threads using openMP.

Note that f1 and f2 should be user-defined before running according to the error concealment strategy used. The program will repeatedly ask for current network loss rate, and give the resultant JND Surfaces. Enter -1 to quit.

## To merge two JND Surfaces regarding one same control input:

    $ ./main.exe -m <input filepath 1> <input filepath 2> <output path> <smallest reference> <largest reference> \
    <smallest modification> <largest modification>

The two input filepaths are the paths of input csv-format JND Surfaces. Output path is where the resultant JND Surface will be put.

## To find the optimal points:

    $ python optimize.py <threshold> 

Please place the csv file of the combined JND Surfaces that need to find the optimal value in the source folder and name it "combined.csv". To run the program, enter the upper limit of x for which you need to find the local optimal value, i.e. threshold. If you need to find the global optimal value, just enter the maximum value of y. The program will output the optimal value.

Generation of correlation images between Normalized Control Input and Perceptual Quality (Normalized MOS) is being refined.

## heatmap.py can be used seperately, to visualize a csv-format JND Surface

    $ python heatmap.py <input filepath> <smallest reference> <largest reference> <smallest modification> <largest modification>

# UDP Ping-pong Game

This is a simple ping-pong game using UDP and socket programming. To compile the game:

    $ make

# Start the receiver in a terminal

    $ ./receiver <port number> <window size> <output path> <log file>

# Start the sender in another terminal

    $ ./sender <receiver IP address> <receiver port number> <window size> <payload file> <log file>

# TCP Ping-pong Game

This is a simple ping-pong game using TCP and socket programming. To compile the game:

    $ make

# Start the server in a terminal

    $ ./gameserver <port number>

# Start the client in another terminal

    $ ./gameclient <server ip addr> <server port>

Then it will show the following:

    $ Press enter to send a request; or press "q" to quit:

If the user press "q", the client will disconnect and close. If the user press enter, a request will be sent to the server.
The server will return the "ball": a certain-byte file to the client, and "SUCCESSFULLY Received a file from the server" 
will be displayed on the client's screen if the ball is received by the client correctly.

## References
[1] Optimizing the Perceptual Quality of Real-Time Multimedia Applications, J. X. Xu and B. W. Wah, IEEE Multimedia, vol. 22, no. 4, Oct.-Dec. 2015, pp. 14-28.

[2] Optimality of Greedy Algorithm for Generating Just-Noticeable Difference Surfaces, J. X. Xu and B. W. Wah, IEEE Trans. on Multimedia, vol. 18, no. 7, July 2016, pp. 1330-1337.

[3] Optimizing Perceptual Quality for Online Multimedia Systems with Fast-Paced Interactions, J. X. Xu, Ph.D. Thesis, Dept. of Computer Science and Engineering, Chinese Univ. of Hong Kong, Hong Kong, Aug. 2017.

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "crc32.h"
#include "PacketHeader.h"

#define MAX_CHUNK_LEN 1456
#define MAX_PACKET_LEN 1472

// assemble datagram with type, seqNum, length and chunk
void assemble_datagram(char *datagram, int type, int seqNum, int length, char *chunk) {
    size_t packet_header_len = sizeof(struct PacketHeader);
    struct PacketHeader packet_header = {type, seqNum, length, crc32(chunk, length)};
    memcpy(datagram, &packet_header, packet_header_len);
    memcpy(datagram + packet_header_len, chunk, length);
}

// extract header from a datagram
struct PacketHeader extract_packet_header(char *datagram) {
    struct PacketHeader packet_header;
    memcpy(&packet_header, datagram, sizeof(struct PacketHeader));
    return packet_header;
}

// extract chunk from a datagram
size_t extract_chunk(char *datagram, char *chunk) {
    struct PacketHeader packet_header = extract_packet_header(datagram);
    size_t packet_len = packet_header.length;
    memcpy(chunk, datagram + sizeof(struct PacketHeader), packet_len);
    return packet_len;
}

// write nth chunk to output file
size_t write_nth_chunk(char *chunk, int n, size_t chunk_len, FILE *fileptr) {
    long offset = MAX_CHUNK_LEN * n;
    fseek(fileptr, offset, SEEK_SET);
    fwrite(chunk, chunk_len, 1, fileptr);
    return chunk_len;
}

// move window forward
void move_forward(int *window, int window_size, int move_forward_by) {
    memmove(&window[0], &window[move_forward_by], (window_size - move_forward_by) * sizeof(int));
    for (int i = window_size - move_forward_by; i < window_size; i++) {
        window[i] = 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cout << "Error: Usage is ./wReceiver <port-num> <window-size> <output-dir> <log>\n";
        return 1;
    }
    
    // handle input
    int port = atoi(argv[1]);
    int window_size = atoi(argv[2]);
    char *file_dir = argv[3];
    char *log = argv[4];
    FILE *log_fileptr = fopen(log, "w");
    
    // set socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in recv_addr;
    struct sockaddr_in send_addr;
    int addr_len = sizeof(struct sockaddr);
    
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(port);
    recv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *) &recv_addr, addr_len);
    
    // initialize buffer
    char dg[MAX_PACKET_LEN];
    bzero(dg, MAX_PACKET_LEN);
    char ack[MAX_PACKET_LEN];
    bzero(ack, MAX_PACKET_LEN);
    char chunk[MAX_CHUNK_LEN];
    bzero(chunk, MAX_CHUNK_LEN);
    
    // initialize length
    size_t header_len = sizeof(PacketHeader);
    size_t chunk_len;
    size_t packet_len;
    
    int file_num = -1;
    int window_start;
    int window[window_size];
    
    // keep server alive
    while (true) {
        // set file path
        file_num++;
        char file[strlen(file_dir) + 10];
        sprintf(file, "%s/FILE-%d.out", file_dir, file_num);
        
        // initialize for each file
        window_start = 0;
        int ver = -1;
        FILE *fileptr = nullptr;
        for (int i = 0; i < window_size; i++) {
            window[i] = 0;
        }
        
        // receive file
        bool flag = 0;
        while (!flag) {
            recvfrom(sockfd, dg, MAX_PACKET_LEN, 0, (struct sockaddr *) &send_addr, (socklen_t *) &addr_len);
            struct PacketHeader packet_header = extract_packet_header(dg);
            fprintf(log_fileptr, "%u %u %u %u\n", packet_header.type, packet_header.seqNum, packet_header.length, packet_header.checksum);
            fflush(log_fileptr);
            char *send_ip = inet_ntoa(send_addr.sin_addr);
            int send_port = ntohs(send_addr.sin_port);
            
            // check chunk
            chunk_len = extract_chunk(dg, chunk);
            if (crc32(chunk, chunk_len) != packet_header.checksum) {
                printf("Checksum wrong\n");
                continue;
            }
            
            int seqNum = -1;
            bool send_ack = 1;
            // receive START
            if (packet_header.type == 0) {
                if (ver != -1) {
                    printf("Redundant START\n");
                    send_ack = 0;
                } else {
                    // open file
                    ver = packet_header.seqNum;
                    seqNum = packet_header.seqNum;
                    if (fileptr == nullptr) {
                        fileptr = fopen(file, "wb+");
                        fclose(fileptr);
                        fileptr = fopen(file, "rb+");
                    }
                }
            // receive END
            } else if (packet_header.type == 1) {
                if (ver != packet_header.seqNum) {
                    printf("END seqNum not identical to START\n");
                    send_ack = 0;
                } else {
                    // finish receiving
                    ver = -1;
                    seqNum = packet_header.seqNum;
                    flag = 1;
                }
            // receive chunk
            } else if (packet_header.type == 2) {
                if (ver == -1) {
                    printf("Not receive START\n");
                    send_ack = 0;
                } else {
                    if (packet_header.seqNum < window_start) {
                        seqNum = window_start;
                    } else if (packet_header.seqNum > window_start) {
                        seqNum = window_start;
                        
                        // write chunk
                        if (packet_header.seqNum < window_start + window_size && window[packet_header.seqNum - window_start] == 0) {
                            window[packet_header.seqNum - window_start] = 1;
                            write_nth_chunk(chunk, packet_header.seqNum, chunk_len, fileptr);
                        }
                    } else {
                        // write chunk
                        if (window[0] == 0) {
                            window[0] = 1;
                            write_nth_chunk(chunk, packet_header.seqNum, chunk_len, fileptr);
                        }
                        
                        // move window forward
                        int move_forward_by = 0;
                        for (int i = 0; i < window_size; i++) {
                            if (window[i] == 1) {
                                move_forward_by++;
                            } else {
                                break;
                            }
                        }
                        move_forward(window, window_size, move_forward_by);
                        window_start += move_forward_by;
                        seqNum = window_start;
                    }
                }
            } else {
                send_ack = 0;
            }
            
            if (!send_ack) {
                continue;
            }
            
            // send ACK
            struct sockaddr_in ack_addr;
            struct hostent *host = gethostbyname(send_ip);
            ack_addr.sin_family = AF_INET;
            ack_addr.sin_port = htons(send_port);
            ack_addr.sin_addr = *((struct in_addr *) host->h_addr);

            bzero(chunk, MAX_CHUNK_LEN);
            assemble_datagram(ack, 3, seqNum, 0, chunk);
            size_t ack_packet_len = header_len;
            
            sendto(sockfd, ack, ack_packet_len, 0, (struct sockaddr *) &ack_addr, addr_len);
            struct PacketHeader ack_packet_header = extract_packet_header(ack);
            fprintf(log_fileptr, "%u %u %u %u\n", ack_packet_header.type, ack_packet_header.seqNum, ack_packet_header.length, ack_packet_header.checksum);
            fflush(log_fileptr);
        }

        fclose(fileptr);
    }

    close(sockfd);
    fclose(log_fileptr);

    return 0;
}

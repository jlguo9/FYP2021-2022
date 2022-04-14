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
    struct PacketHeader packet_header = {type, seqNum, length, crc32(chunk, length)};
    size_t header_len = sizeof(struct PacketHeader);
    memcpy(datagram, &packet_header, header_len);
    memcpy(datagram + header_len, chunk, length);
}

// extract header from a datagram
struct PacketHeader extract_packet_header(char *datagram) {
    struct PacketHeader packet_header;
    memcpy(&packet_header, datagram, sizeof(struct PacketHeader));
    return packet_header;
}

// read nth chunk from input file
size_t read_nth_chunk(char *chunk, int n, long file_len, FILE *fileptr) {
    long offset = MAX_CHUNK_LEN * n;
    size_t chunk_len;
    if (file_len - offset < MAX_CHUNK_LEN) {
        chunk_len = file_len - offset;
    } else {
        chunk_len = MAX_CHUNK_LEN;
    }
    fseek(fileptr, offset, SEEK_SET);
    fread(chunk, chunk_len, 1, fileptr);
    return chunk_len;
}

// return smaller value
int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

// move window forward
void move_forward(int *window, int window_size, int move_forward_by) {
    memmove(&window[0], &window[move_forward_by], (window_size - move_forward_by) * sizeof(int));
    for (int i = window_size - move_forward_by; i < window_size; i++) {
        window[i] = -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cout << "Error: Usage is ./wSender <receiver-IP> <receiver-port> <window-size> <input-file> <log>\n";
        return 1;
    }
    
    // handle input
    char *recv_ip = argv[1];
    int recv_port = atoi(argv[2]);
    int window_size = atoi(argv[3]);
    char *file = argv[4];
    char *log = argv[5];
    FILE *log_fileptr = fopen(log, "w");
    
    // set socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in recv_addr;
    struct sockaddr_in ack_addr;
    int addr_len = sizeof(struct sockaddr);
    
    struct hostent *host = gethostbyname(recv_ip);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(recv_port);
    recv_addr.sin_addr = *((struct in_addr *) host->h_addr);
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // initialize buffer
    char dg[MAX_PACKET_LEN];
    bzero(dg, MAX_PACKET_LEN);
    char ack[MAX_PACKET_LEN];
    bzero(ack, MAX_PACKET_LEN);
    char chunk[MAX_CHUNK_LEN];
    bzero(chunk, MAX_CHUNK_LEN);
    
    // initialize length
    size_t header_len = sizeof(struct PacketHeader);
    size_t chunk_len;
    size_t packet_len;
    
    // initialize reader
    FILE *fileptr;
    fileptr = fopen(file, "rb");
    fseek(fileptr, 0, SEEK_END);
    long file_len = ftell(fileptr);
    int num_chunks = file_len / (MAX_PACKET_LEN - sizeof(struct PacketHeader)) + 1;
    fseek(fileptr, 0, SEEK_SET);
    
    // assemble START
    srand(time(NULL));
    unsigned int ver = rand();
    assemble_datagram(dg, 0, ver, 0, chunk);
    packet_len = header_len;
    
    // send START
    while (true) {
        // send START
        sendto(sockfd, dg, packet_len, 0, (struct sockaddr *) &recv_addr, addr_len);
        struct PacketHeader packet_header = extract_packet_header(dg);
        fprintf(log_fileptr, "%u %u %u %u\n", packet_header.type, packet_header.seqNum, packet_header.length, packet_header.checksum);
        fflush(log_fileptr);
        
        // receive ACK
        if (recvfrom(sockfd, ack, MAX_PACKET_LEN, 0, (struct sockaddr *) &ack_addr, (socklen_t *) &addr_len) == -1) {
            continue;
        }
        struct PacketHeader ack_header = extract_packet_header(ack);
        fprintf(log_fileptr, "%u %u %u %u\n", ack_header.type, ack_header.seqNum, ack_header.length, ack_header.checksum);
        fflush(log_fileptr);
        
        // verify
        if (ack_header.type == 3 && ack_header.seqNum == ver) {
            break;
        }
    }
    
    // initialize window
    int window_start = 0;
    int window[window_size];
    for (int i = 0; i < window_size; i++) {
        window[i] = -1;     // -1: not sent; 0: sent not ack'd; 1: ack'd
    }
    
    bool resend_all = 1;
    
    // send file
    while (window_start != num_chunks) {
        // send chunks in window
        for (int i = 0; i < min(window_size, num_chunks - window_start); i++) {
            if (window[i] == -1 || (resend_all && (window[i] == 0))) {
                // read chunk and assemble datagram
                chunk_len = read_nth_chunk(chunk, window_start + i, file_len, fileptr);
                assemble_datagram(dg, 2, window_start + i, chunk_len, chunk);
                packet_len = chunk_len + header_len;
                
                // send datagram
                sendto(sockfd, dg, packet_len, 0, (struct sockaddr *) &recv_addr, addr_len);
                struct PacketHeader packet_header = extract_packet_header(dg);
                fprintf(log_fileptr, "%u %u %u %u\n", packet_header.type, packet_header.seqNum, packet_header.length, packet_header.checksum);
                fflush(log_fileptr);
                window[i] = 0;
            }
        }
        
        // receive ACK
        if (recvfrom(sockfd, ack, MAX_PACKET_LEN, 0, (struct sockaddr *) &ack_addr, (socklen_t *) &addr_len) == -1) {
            resend_all = 1;
            continue;
        } else {
            resend_all = 0;
        }
        struct PacketHeader ack_header = extract_packet_header(ack);
        fprintf(log_fileptr, "%u %u %u %u\n", ack_header.type, ack_header.seqNum, ack_header.length, ack_header.checksum);
        fflush(log_fileptr);
        
        // move window forward
        if (ack_header.type == 3 && ack_header.seqNum > window_start) {
            move_forward(window, window_size, ack_header.seqNum - window_start);
            window_start = ack_header.seqNum;
            continue;
        }
    }
    
    // assemble END
    bzero(dg, MAX_PACKET_LEN);
    bzero(chunk, MAX_CHUNK_LEN);
    assemble_datagram(dg, 1, ver, 0, chunk);
    packet_len = header_len;
    
    // send END
    while (true) {
        // send END
        sendto(sockfd, dg, packet_len, 0, (struct sockaddr *) &recv_addr, addr_len);
        struct PacketHeader packet_header = extract_packet_header(dg);
        fprintf(log_fileptr, "%u %u %u %u\n", packet_header.type, packet_header.seqNum, packet_header.length, packet_header.checksum);
        fflush(log_fileptr);
        
        // receive ACK
        if (recvfrom(sockfd, ack, MAX_PACKET_LEN, 0, (struct sockaddr *) &ack_addr, (socklen_t *) &addr_len) == -1) {
            continue;
        }
        struct PacketHeader ack_header = extract_packet_header(ack);
        fprintf(log_fileptr, "%u %u %u %u\n", ack_header.type, ack_header.seqNum, ack_header.length, ack_header.checksum);
        fflush(log_fileptr);
        
        // verify
        if (ack_header.type == 3 && ack_header.seqNum == ver) {
            break;
        }
    }
    
    close(sockfd);
    fclose(fileptr);
    fclose(log_fileptr);

    return 0;
}

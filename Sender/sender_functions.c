/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (312497084)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <math.h>
#include "sender_functions.h"
#include "HardCodedData.h"

#pragma comment(lib, "Ws2_32.lib")

static int g_port;
static char* g_ip;
static unsigned int total_bytes_read = 0;
static unsigned int total_bytes_sent = 0;

/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <returns>zero if successful, one otherwise</returns>
int boot_client(char* address, int port){
    g_ip = address;
    g_port = port;
    SOCKET main_socket = INVALID_SOCKET;
    char f_name[MAX_FN] = "";

    // Initialize Winsock
    WSADATA wsa_data;
    int result;
    result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        printf("Error: WSAStartup failed: %d\n", result);
        return 1;
    }

    SOCKADDR_IN client_service;
    client_service.sin_family = AF_INET;
    client_service.sin_port = htons(port); //The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
    client_service.sin_addr.s_addr = inet_addr(address);

    // start server communications
    while (TRUE) {

        total_bytes_read = 0;
        total_bytes_sent = 0;

        main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (main_socket == INVALID_SOCKET) {
            printf("ERROR: Failed to create a socket\n");
            return 1;
        }

        if (connect(main_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR) {
            printf("Failed connecting to server on %s:%lu\n", address, port);
            return 1;
        }

        // Client connected succefully, get file name
        printf("enter file name:\n");

        if (fgets(f_name, MAX_FN, stdin) == NULL) {
            printf("Error: could not read file name\n");
            return 1;
        }

        strtok(f_name, "\n");

        if (strcmp(f_name, "quit") == 0) {
            return 0;
        }

        if (send_file(f_name, &main_socket)) {
            return 1;
        }

        printf("file length: %u bytes\n", total_bytes_read);
        printf("sent: %u bytes\n", total_bytes_sent);

    }
}


/// <summary>
/// send file
/// </summary>
/// <param name="file_name">file name</param>
/// <param name="p_socket">pointer to socket</param>
/// <returns>zero if successful, one otherwise</returns>
int send_file(char* file_name, SOCKET* p_socket) {
    FILE* fp = NULL;
    if (fopen_s(&fp, file_name, "rb")) {
        fprintf(stderr, "Error: failed to read file");
        return 1;
    }

    // taken from stack overflow https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);  /* same as rewind(f); */

    char* file_buffer = (char*)malloc(sizeof(char) * FILE_BUFFER_SIZE_BYTES + 1);

    int size_read = fread(file_buffer, sizeof(char), fsize, fp);
    if (size_read < fsize) {
        printf("Error: failed to read file\n");
        free(file_buffer);
        return 1;
    }

    total_bytes_read = size_read;
    //file_buffer[size_read + 1] = '\0';

    if (fclose(fp)) {
        fprintf(stderr, "Error: failed to close file");
        return 1;
    }

    if (fsize % DATA_BITS_IN_BLOCK != 0) {
        printf("Warning: file size is not a multiplication of %d", DATA_BITS_IN_BLOCK);
    }

    int packet_size = (fsize / DATA_BITS_IN_BLOCK) * BITS_IN_BLOCK_WITH_HAMMING; // packet size in bytes
    char* packet = (char*)malloc(sizeof(char) * PACKET_BUFFER_SIZE_BYTES + 1);

    int pakcet_offset = 0;

    int number_of_hamming_frames = (fsize / DATA_BITS_IN_BLOCK); // each frame is 26 bytes long

    char frame_buffer[DATA_BITS_IN_BLOCK] = { 0 }; // frame contains 26 bytes

    int offset = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < number_of_hamming_frames; i++) {
        memset(frame_buffer, 0, sizeof(frame_buffer));
        offset = i * DATA_BITS_IN_BLOCK;
        for (j = 0; j < DATA_BITS_IN_BLOCK; j++) {
            frame_buffer[j] = file_buffer[offset + j];
        }
        write_frame_to_packet(fp, frame_buffer, &(packet[pakcet_offset]));
        pakcet_offset += BITS_IN_BLOCK_WITH_HAMMING;
    }

    send_packet(packet, packet_size, p_socket);

    free(packet);
    free(file_buffer);

    return 0;
}


void write_frame_to_packet(FILE* fp, char* frame_buffer, char* packet) {
    char frame_in_bits[DATA_BITS_IN_BLOCK * BITS_IN_BYTE] = { 0 }; // get bits from 26 bytes of frames
    char hamming_frame_in_bits[BITS_IN_BLOCK_WITH_HAMMING * BITS_IN_BYTE] = { 0 }; // frame with hamming check bits in bit representation

    int offset = 0;
    int read_mask = 0;
    int temp = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < DATA_BITS_IN_BLOCK; i++) {
        offset = i * BITS_IN_BYTE;
        read_mask = 1 << (BITS_IN_BYTE - 1);
        for (j = 0; j < BITS_IN_BYTE; j++) {
            temp = read_mask & (int)frame_buffer[i];
            if (temp > 0) {
                frame_in_bits[offset + j] = 1;
            }
            else
                frame_in_bits[offset + j] = 0;
            read_mask >>= 1;
        }
    }

    int frame_in_bits_offset = 0;
    int hamming_frame_in_bits_offset = 0;
    for (int i = 0; i < BITS_IN_BYTE; i++) {
        frame_in_bits_offset = i * DATA_BITS_IN_BLOCK;
        hamming_frame_in_bits_offset = i * BITS_IN_BLOCK_WITH_HAMMING;
        add_hamming(&(frame_in_bits[frame_in_bits_offset]), &(hamming_frame_in_bits[hamming_frame_in_bits_offset]));
    }
    bin_to_dec(hamming_frame_in_bits, packet, BITS_IN_BLOCK_WITH_HAMMING);
}


/// <summary>
/// Create Hamming block by adding the hamming check bits to the data bits
/// </summary>
/// <param name="data_buffer">Hamming block data bits</param>
/// <param name="frame_buffer">hamming block</param>
void add_hamming(char* data_buffer, char* frame_buffer) {
    int q1, q2, q4, q8, q16;

    frame_buffer[2] = data_buffer[0];

    frame_buffer[4] = data_buffer[1];
    frame_buffer[5] = data_buffer[2];
    frame_buffer[6] = data_buffer[3];

    frame_buffer[8] = data_buffer[4];
    frame_buffer[9] = data_buffer[5];
    frame_buffer[10] = data_buffer[6];
    frame_buffer[11] = data_buffer[7];
    frame_buffer[12] = data_buffer[8];
    frame_buffer[13] = data_buffer[9];
    frame_buffer[14] = data_buffer[10];

    frame_buffer[16] = data_buffer[11];
    frame_buffer[17] = data_buffer[12];
    frame_buffer[18] = data_buffer[13];
    frame_buffer[19] = data_buffer[14];
    frame_buffer[20] = data_buffer[15];
    frame_buffer[21] = data_buffer[16];
    frame_buffer[22] = data_buffer[17];
    frame_buffer[23] = data_buffer[18];
    frame_buffer[24] = data_buffer[19];
    frame_buffer[25] = data_buffer[20];
    frame_buffer[26] = data_buffer[21];
    frame_buffer[27] = data_buffer[22];
    frame_buffer[28] = data_buffer[23];
    frame_buffer[29] = data_buffer[24];
    frame_buffer[30] = data_buffer[25];


    //using indexing that starts in 0, not in 1 
    q1 = frame_buffer[2] ^ frame_buffer[4] ^ frame_buffer[6] ^ frame_buffer[8] ^ frame_buffer[10] ^ frame_buffer[12] ^ frame_buffer[14] ^ frame_buffer[16] ^ frame_buffer[18] ^ frame_buffer[20] ^ frame_buffer[22] ^ frame_buffer[24] ^ frame_buffer[26] ^ frame_buffer[28] ^ frame_buffer[30];
    frame_buffer[0] = q1;

    q2 = frame_buffer[2] ^ frame_buffer[5] ^ frame_buffer[6] ^ frame_buffer[9] ^ frame_buffer[10] ^ frame_buffer[13] ^ frame_buffer[14] ^ frame_buffer[17] ^ frame_buffer[18] ^ frame_buffer[21] ^ frame_buffer[22] ^ frame_buffer[25] ^ frame_buffer[26] ^ frame_buffer[29] ^ frame_buffer[30];
    frame_buffer[1] = q2;

    q4 = frame_buffer[4] ^ frame_buffer[5] ^ frame_buffer[6] ^ frame_buffer[11] ^ frame_buffer[12] ^ frame_buffer[13] ^ frame_buffer[14] ^ frame_buffer[19] ^ frame_buffer[20] ^ frame_buffer[21] ^ frame_buffer[22] ^ frame_buffer[27] ^ frame_buffer[28] ^ frame_buffer[29] ^ frame_buffer[30];
    frame_buffer[3] = q4;

    q8 = frame_buffer[8] ^ frame_buffer[9] ^ frame_buffer[10] ^ frame_buffer[11] ^ frame_buffer[12] ^ frame_buffer[13] ^ frame_buffer[14] ^ frame_buffer[23] ^ frame_buffer[24] ^ frame_buffer[25] ^ frame_buffer[26] ^ frame_buffer[27] ^ frame_buffer[28] ^ frame_buffer[29] ^ frame_buffer[30];
    frame_buffer[7] = q8;

    q16 = frame_buffer[16] ^ frame_buffer[17] ^ frame_buffer[18] ^ frame_buffer[19] ^ frame_buffer[20] ^ frame_buffer[21] ^ frame_buffer[22] ^ frame_buffer[23] ^ frame_buffer[24] ^ frame_buffer[25] ^ frame_buffer[26] ^ frame_buffer[27] ^ frame_buffer[28] ^ frame_buffer[29] ^ frame_buffer[30];
    frame_buffer[15] = q16;
}


/// <summary>
/// Converts an array of binary chars ints to chars by calculating the decimal value of every 8 consecutive elements in array
/// </summary>
/// <param name="source">source int array</param>
/// <param name="dest">destination char array</param>
/// <param name="num_of_bytes">size of char array</param>
/// <returns>send result</returns>
void bin_to_dec(char* source, char* dest, int num_of_bytes) {
    int temp = 0;
    int curr_val = 0;
    int ref_index = 0;

    for (int i = 0; i < (num_of_bytes); i++) {
        curr_val = 0;
        ref_index = 8 * i;
        for (int j = 0; j < BITS_IN_BYTE; j++) {
            temp = (int)source[j + ref_index];
            temp *= pow(2, (double)BITS_IN_BYTE - 1 - j);
            curr_val += temp;
        }
        dest[i] = (int)curr_val;
    }
}


/// <summary>
/// Sends a string of known size via a socket
/// </summary>
/// <param name="buffer">string buffer</param>
/// <param name="message_len">length of the string</param>
/// <param name="p_connection_socket">pointer to the socket</param>
/// <returns>send result</returns>
int send_packet(char* buffer, const int message_len, SOCKET* p_connection_socket) {
    char* p_current_place = buffer;
    int bytes_transferred, error_reason;
    int remaining_bytes = message_len;

    // send the string (not zero terminated)
    while (remaining_bytes > 0) {
        bytes_transferred = send((*p_connection_socket), p_current_place, remaining_bytes, 0);
        if (bytes_transferred == SOCKET_ERROR)
        {
            // check if send has been disabled
            error_reason = WSAGetLastError();
            if (error_reason == WSAESHUTDOWN || error_reason == WSAENOTSOCK || error_reason == WSAEINTR) {
                fprintf(stderr, "Error: send failed because %d.\n", error_reason);
                return 1;
            }
        }

        total_bytes_sent += bytes_transferred;

        // check how many bytes left
        remaining_bytes -= bytes_transferred;
        p_current_place += bytes_transferred;
    }
    return 0;
}


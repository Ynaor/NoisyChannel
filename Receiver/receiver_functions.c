/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (?????????)
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
#pragma comment(lib, "Ws2_32.lib")

#include "HardCodedData.h"
#include "receiver_functions.h"


#ifdef _DEBUG
static int g_port;
static char* g_ip;
static unsigned char write_mask = 0;
static unsigned int total_bytes_written = 0;
static unsigned int total_bytes_received = 0;
static unsigned int next_bit_index = 0;         // next bit index in a byte, max val = 7;
static unsigned char write_byte = 0;            // next byte to write to file
static unsigned int errors_corrected = 0;
FILE* input_data = NULL; // debug



/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <returns>zero if successful, one otherwise</returns>
int main() {
    //SOCKET main_socket = INVALID_SOCKET;
    //g_ip = address;
    //g_port = port;
    char f_name[MAX_FN] = { 0 };

    // Initialize Winsock
    /*
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
    */
    // start server communications
    while (TRUE) {

        total_bytes_received = 0;
        total_bytes_written = 0;
        write_mask = 0;
        write_byte = 0;
        errors_corrected = 0;
        /*
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
        /// NEED TO HANDLE Too LONG FILE NAME *********************************************
        if (strcmp(f_name, "quit") == 0) {
            return 0;
        }
        */

        if (communicate_server()) {
            return 1;
        }

        printf("received: %u bytes\n", total_bytes_received);
        printf("wrote: %u bytes\n", total_bytes_written);
        printf("corrected: %u errors\n", errors_corrected);
    }
}


/// <summary>
/// Receive data from server decode and write to file
/// </summary>
/// <param name="file_name">file name</param>
/// <param name="p_socket">Socket pointer</param>
/// <returns>zero if successful, one otherwise</returns>
int communicate_server() {
    FILE* fp;
    if (fopen_s(&fp, "decoded_data.txt", "wb")) {
        fprintf(stderr, "Error: failed to open file decoded_data");
        return 1;
    }

    //debug
    if (fopen_s(&input_data, "input_data.txt", "rb")) {
        fprintf(stderr, "Error: failed to open file decoded_data");
        return 1;
    }

    TransferResult_t recv_result;                       // recv_packet result

    int packet_size = 0;                                // number of bytes in received packet

    int parsed_message[MAX_BITS_IN_PACKET] = { 0 };     // packet message parsed to bits

    char message[MAX_BYTES_IN_PACKET];

    // each iteration is per packet
    while (TRUE) {
        /*
        recv_result = recv_packet(message, MAX_BYTES_IN_PACKET, p_socket, &packet_size);

        if (recv_result == TRNS_FAILED) {
            if (!fclose(fp))
                printf("Error: unable to close file\n");
            return 1;
        }
        */

        //debug:
        int ch = 0;
        int count = 0;
        while ((ch = fgetc(input_data))!=EOF && count< MAX_BYTES_IN_PACKET){
            message[count] = ch;
            count++;
        }
        packet_size = count;

        
        get_bits(message, parsed_message, packet_size);
        if (parse_packet(fp, parsed_message, packet_size)) {
            if (fclose(fp))
                printf("Error: unable to close file\n");
            return 1;
        }
            return 0;
        
        /*
            // debug
        int parsed_message[32] = {0,1,0,1,1,1,0,1,0,0,0,1,0,1,1,1,0,0,0,0,1,0,1,1,0,0,0,0,1,0,1};
        int decoded_message[26] = { 0 };
        decode_hamming(parsed_message, decoded_message);
        return 0;
        */
    }
}


/// <summary>
/// parse packet - decode and write data bits to file
/// </summary>
/// <param name="p_file">pointer to file </param>
/// <param name="source">Source </param>
/// <param name="packet_size">The size of packet in bytes</param>
/// <returns>zero if successful, one otherwise</returns>
int parse_packet(FILE* p_file, int* source, int packet_size) {

    int total_bits = packet_size * BITS_IN_BYTE;
    int frames = total_bits / BITS_IN_FRAME;
    int parsed_frame[DATA_BITS_IN_FRAME];
    int temp = 0;

    for (int i = 0; i < frames; i++) {
        decode_hamming(&(source[i * BITS_IN_FRAME]), parsed_frame);
        for (int j = 0; j < DATA_BITS_IN_FRAME; j++) {
            temp = parsed_frame[j];
            temp *= pow(2, (double)BITS_IN_BYTE - 1 - next_bit_index);
            write_byte += temp;
            if (next_bit_index == 7) {
                if (file_write_byte(p_file))
                    return 1;
                next_bit_index = 0;
                write_byte = 0;
            }
            else
                next_bit_index++;
        }
    }
    return 0;
}


/// <summary>
/// Decode Hamming
/// </summary>
/// <param name="encoded_buffer">Hamming encoded data</param>
/// <param name="decoded_buffer">Decoded data</param>
void decode_hamming(int* encoded_buffer, int* decoded_buffer) {
    int error_index = 0;
    for (int i = 0; i < BITS_IN_FRAME; i++) {
        if (encoded_buffer[i] == 1)
            error_index ^= (i + 1);
    }

    if (error_index != 0) {
        encoded_buffer[error_index - 1] = !encoded_buffer[error_index - 1];
        errors_corrected++;
    }

    int decoded_buffer_index = 0;
    for (int i = 2; i < BITS_IN_FRAME; i++) {
        if (ceil(log2((double)i + 1)) == floor(log2((double)i + 1)))
            continue;
        decoded_buffer[decoded_buffer_index] = encoded_buffer[i];
        decoded_buffer_index++;
    }
}


/// <summary>
/// Write byte in write_byte global var to file
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
int file_write_byte(FILE* p_file) {
    if (fputc(write_byte, p_file) != write_byte) {
        printf("Error: could not write to file");
        return 1;
    }
    total_bytes_written++;
    write_byte = 0;
    return 0;
}


/// <summary>
/// save a given char as an array of bits
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
void get_bits(char* read_target, int* data_buffer, int packet_size) {

    int read_mask = 0;
    int temp;
    int ref_index = 0;

    for (int i = 0; i < packet_size; i++) {

        read_mask = 1 << (BITS_IN_BYTE - 1);
        ref_index = i * BITS_IN_BYTE;

        for (int j = 0; j < BITS_IN_BYTE; j++) {
            temp = read_mask & read_target[i];
            if (temp > 0) {
                data_buffer[ref_index + j] = 1;
            }
            read_mask >>= 1;
        }
    }
}
#endif _DEBUG


#ifndef _DEBUG

static int g_port;
static char* g_ip;
static unsigned char write_mask = 0;
static unsigned int total_bytes_written = 0;
static unsigned int total_bytes_received = 0;
static unsigned int next_bit_index = 0;         // next bit index in a byte, max val = 7;
static unsigned char write_byte = 0;            // next byte to write to file
static unsigned int errors_corrected = 0;


/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <returns>zero if successful, one otherwise</returns>
int boot_client(char* address, int port){
    SOCKET main_socket = INVALID_SOCKET;
    g_ip = address;
    g_port = port;
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

        total_bytes_received = 0;
        total_bytes_written = 0;
        write_mask = 0;
        write_byte = 0;
        errors_corrected = 0;

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
        if (scanf("%s", f_name) == NULL) {
            printf("Error: could not create output file\n");
            return 1;
        }
        /// NEED TO ANDLE Too LONG FILE NAME *********************************************
        if (strcmp(f_name, "quit") == 0) {
            return 0;
        }

        if (communicate_server(f_name, &main_socket)) {
            return 1;
        }

        printf("received: %u bytes\n", total_bytes_received);
        printf("wrote: %u bytes\n", total_bytes_written);
        printf("corrected: %u errors\n", errors_corrected);
    }
}


/// <summary>
/// Receive data from server decode and write to file
/// </summary>
/// <param name="file_name">file name</param>
/// <param name="p_socket">Socket pointer</param>
/// <returns>zero if successful, one otherwise</returns>
int communicate_server(char* file_name, SOCKET* p_socket) {
    FILE* fp;
    if (fopen_s(&fp, file_name, "w")) {
        fprintf(stderr, "Error: failed to open file");
        return 1;
    }

    TransferResult_t recv_result;                       // recv_packet result

    int packet_size = 0;                                // number of bytes in received packet

    int parsed_message[MAX_BITS_IN_PACKET] = { 0 };     // packet message parsed to bits

    char message[MAX_BYTES_IN_PACKET];


    // each iteration is per packet
    while (TRUE) {

        recv_result = recv_packet(message, MAX_BYTES_IN_PACKET, p_socket, &packet_size);

        if (recv_result == TRNS_FAILED) {
            if (!fclose(fp))
                printf("Error: unable to close file\n");
            return 1;
        }


        get_bits(message, parsed_message, packet_size);
        if (parse_packet(fp, parsed_message, packet_size)) {
            if (!fclose(fp))
                printf("Error: unable to close file\n");
            return 1;
        }
        
        if (recv_result == TRNS_DISCONNECTED) {
            if (!fclose(fp)) {
                printf("Error: unable to close file\n");
                return 1;
            }
            return 0;
        }
    }
}


/// <summary>
/// parse packet - decode and write data bits to file
/// </summary>
/// <param name="p_file">pointer to file </param>
/// <param name="source">Source </param>
/// <param name="packet_size">The size of packet in bytes</param>
/// <returns>zero if successful, one otherwise</returns>
int parse_packet(FILE* p_file, int* source, int packet_size) {
    
    int total_bits = packet_size * BITS_IN_BYTE;
    int frames = total_bits / BITS_IN_FRAME;
    int parsed_frame[DATA_BITS_IN_FRAME];
    int temp = 0;

    for (int i = 0; i < frames; i++) {
        decode_hamming(&(source[i * BITS_IN_FRAME]), parsed_frame);
        for (int j = 0; j < DATA_BITS_IN_FRAME; j++) {
            temp = parsed_frame[j];
            temp *= pow(2, (double)BITS_IN_BYTE - 1 - next_bit_index);
            write_byte += temp;
            if (next_bit_index == 7) {
                if (file_write_byte(p_file))
                    return 1;
                next_bit_index = 0;
                write_byte = 0;
            }
            else
                next_bit_index++;
        }
    }
    return 0;
}


/// <summary>
/// Decode Hamming
/// </summary>
/// <param name="encoded_buffer">Hamming encoded data</param>
/// <param name="decoded_buffer">Decoded data</param>
void decode_hamming(int* encoded_buffer, int* decoded_buffer) {
    int error_index = 0;
    for (int i = 0; i < BITS_IN_FRAME; i++) {
        if (encoded_buffer[i] == 1)
            error_index ^= (i + 1);
    }

    if (error_index != 0) {
        encoded_buffer[error_index - 1] = !encoded_buffer[error_index - 1];
        errors_corrected++;
    }

    int decoded_buffer_index = 0;
    for (int i = 2; i < BITS_IN_FRAME; i++) {
        if (ceil(log2((double)i + 1)) == floor(log2((double)i + 1)))
            continue;
        decoded_buffer[decoded_buffer_index] = encoded_buffer[i];
        decoded_buffer_index++;
    }
}


/// <summary>
/// Write byte in write_byte global var to file
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
int file_write_byte(FILE* p_file) {
    if (fputc(write_byte, p_file) != write_byte) {
        printf("Error: could not write to file");
        return 1;
    }
    total_bytes_written++;
    write_byte = 0;
    return 0;
}


/// <summary>
/// save a given char as an array of bits
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
void get_bits(char* read_target, int* data_buffer, int packet_size) {

    int read_mask = 0;
    int temp;
    int ref_index = 0;

    for (int i = 0; i < packet_size; i++) {

        read_mask = 1 << (BITS_IN_BYTE - 1);
        ref_index = i * BITS_IN_BYTE;

        for (int j = 0; j < BITS_IN_BYTE; j++) {
            temp = read_mask & read_target[i];
            if (temp > 0) {
                data_buffer[ref_index + j] = 1;
            }
            read_mask >>= 1;
        }
    }
}


/// <summary>
/// Recieve packet via socket
/// </summary>
/// <param name="buffer">output string</param>
/// <param name="packet_length">packet length</param>
/// <param name="p_connection_socket">pointer to the connection socket</param>
/// <param name="bytes_received">total bytes received in this function</param>
/// <returns>TRNS_SUCCEDED if a full packet was received, TRNS_DISCONNECTED if while receiving a packet server finished transmition, TRNS_FAILED if there was an error </returns>
TransferResult_t recv_packet(char* buffer, const int packet_length, SOCKET* p_connection_socket, int* bytes_received) {
    char* p_current_place = buffer;
    int bytes_transferred, bytes_left = packet_length, ret_val = 0, error = 0;

    // recieve bytes of data until done
    while (bytes_left > 0) {

        bytes_transferred = recv(*p_connection_socket, p_current_place, bytes_left, 0);
        
        if (bytes_transferred == 0 || bytes_transferred == SOCKET_ERROR) {
            
            if (bytes_transferred == SOCKET_ERROR) {
                error = WSAGetLastError();
                // Assuming connection termination is only because the server completed sending the data 
                if (error == WSAENOTSOCK || error == WSAEINTR)
                    return TRNS_DISCONNECTED;
                printf("Error: receive failed because of %d.\n", error);
                ret_val = TRNS_FAILED;
            }
            
            else
                ret_val = TRNS_DISCONNECTED;
            
            // close socket
            if (*p_connection_socket != INVALID_SOCKET) {

                if (closesocket(*p_connection_socket) == INVALID_SOCKET) {
                    error = WSAGetLastError();
                    if (error != WSAENOTSOCK && error != WSAEINTR)
                        printf("ERROR: Failed to close socket. Error number %d.\n", WSAGetLastError());
                }
                else
                    *p_connection_socket = INVALID_SOCKET;
            
            }
            
            return ret_val;
        
        }
        
        bytes_received += bytes_transferred;
        total_bytes_received += bytes_transferred;
        bytes_left -= bytes_transferred;
        p_current_place += bytes_transferred;
    }

    return TRNS_SUCCEEDED;
}
#endif _DEBUG

/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (312497084)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#ifndef __client_functions_H__
#define __client_functions_H__
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <math.h>
#pragma comment(lib, "Ws2_32.lib")
#include "HardCodedData.h"

/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <returns>zero if successful, one otherwise</returns>
int boot_client(char* address, int port);

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <math.h>
#pragma comment(lib, "Ws2_32.lib")

#include "HardCodedData.h"


#ifdef HAMMING


/// <summary>
/// Receive data from server decode and write to file
/// </summary>
/// <param name="file_name">file name</param>
/// <param name="p_socket">Socket pointer</param>
/// <returns>zero if successful, one otherwise</returns>
int communicate_server();


/// <summary>
/// parse packet - decode and write data bits to file
/// </summary>
/// <param name="p_file">pointer to file </param>
/// <param name="source">Source </param>
/// <param name="packet_size">The size of packet in bytes</param>
/// <returns>zero if successful, one otherwise</returns>
int parse_packet(FILE* p_file, int* source, int packet_size);


/// <summary>
/// Decode Hamming
/// </summary>
/// <param name="encoded_buffer">Hamming encoded data</param>
/// <param name="decoded_buffer">Decoded data</param>
void decode_hamming(int* encoded_buffer, int* decoded_buffer);


/// <summary>
/// Write byte in write_byte global var to file
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
int file_write_byte(FILE* p_file);


/// <summary>
/// save a given char as an array of bits
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
void get_bits(char* read_target, int* data_buffer, int packet_size);
#endif HAMMING 


#ifndef HAMMING

/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <returns>zero if successful, one otherwise</returns>
int boot_client(char* address, int port);


/// <summary>
/// Receive data from server decode and write to file
/// </summary>
/// <param name="file_name">file name</param>
/// <param name="p_socket">Socket pointer</param>
/// <returns>zero if successful, one otherwise</returns>
int communicate_server(char* file_name, SOCKET* p_socket);


/// <summary>
/// parse packet - decode and write data bits to file
/// </summary>
/// <param name="p_file">pointer to file </param>
/// <param name="source">Source </param>
/// <param name="packet_size">The size of packet in bytes</param>
/// <returns>zero if successful, one otherwise</returns>
int parse_packet(FILE* p_file, int* source, int packet_size);


/// <summary>
/// Decode Hamming
/// </summary>
/// <param name="encoded_buffer">Hamming encoded data</param>
/// <param name="decoded_buffer">Decoded data</param>
void decode_hamming(int* encoded_buffer, int* decoded_buffer);


/// <summary>
/// Write byte in write_byte global var to file
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
int file_write_byte(FILE* p_file);


/// <summary>
/// save a given char as an array of bits
/// </summary>
/// <param name="p_file">pointer to file</param>
/// <returns>zero if successful, one otherwise</returns>
void get_bits(char* read_target, int* data_buffer, int packet_size);


/// <summary>
/// Recieve packet via socket
/// </summary>
/// <param name="buffer">output string</param>
/// <param name="packet_length">packet length</param>
/// <param name="p_connection_socket">pointer to the connection socket</param>
/// <param name="bytes_received">total bytes received in this function</param>
/// <returns>TRNS_SUCCEDED if a full packet was received, TRNS_DISCONNECTED if while receiving a packet server finished transmition, TRNS_FAILED if there was an error </returns>
TransferResult_t recv_packet(char* buffer, const int packet_length, SOCKET* p_connection_socket, int* bytes_received);
#endif HAMMING
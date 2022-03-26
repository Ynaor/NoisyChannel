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


/// <summary>
/// send file
/// </summary>
/// <param name="file_name">file name</param>
/// <param name="p_socket">pointer to socket</param>
/// <returns>zero if successful, one otherwise</returns>
int send_file(char* file_name, SOCKET* p_socket);


void write_frame_to_packet(FILE* fp, char* frame_buffer, char* packet);


/// <summary>
/// Create Hamming block by adding the hamming check bits to the data bits
/// </summary>
/// <param name="data_buffer">Hamming block data bits</param>
/// <param name="frame_buffer">hamming block</param>
void add_hamming(char* data_buffer, char* frame_buffer);


/// <summary>
/// Converts an array of binary chars ints to chars by calculating the decimal value of every 8 consecutive elements in array
/// </summary>
/// <param name="source">source int array</param>
/// <param name="dest">destination char array</param>
/// <param name="num_of_bytes">size of char array</param>
/// <returns>send result</returns>
void bin_to_dec(char* source, char* dest, int num_of_bytes);


/// <summary>
/// Sends a string of known size via a socket
/// </summary>
/// <param name="buffer">string buffer</param>
/// <param name="message_len">length of the string</param>
/// <param name="p_connection_socket">pointer to the socket</param>
/// <returns>send result</returns>
int send_packet(char* buffer, const int message_len, SOCKET* p_connection_socket);
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


/// <summary>
/// Read file to create Hamming interval data
/// </summary>
/// <param name="p_file">file pointer</param>
/// <param name="buffer">data buffer in which to save the read bits</param>
/// <returns>zero if MAX_DATA_BITS were read, one if reached end of file</returns>
int read_file_bits(FILE* p_file, int* data_buffer, int* bits_read);


/// <summary>
/// Create Hamming block by adding the hamming check bits to the data bits
/// </summary>
/// <param name="data_buffer">Hamming block data bits</param>
/// <param name="frame_buffer">hamming block</param>
void stupid_hamming(int* data_buffer, int* frame_buffer);


/// <summary>
/// Sends a string of known size via a socket
/// </summary>
/// <param name="buffer">string buffer</param>
/// <param name="message_len">length of the string</param>
/// <param name="p_connection_socket">pointer to the socket</param>
/// <returns>send result</returns>
int send_packet(char* buffer, const int message_len, SOCKET* p_connection_socket);


/// <summary>
/// Merge two arrays from a given position in the first array
/// </summary>
/// <param name="basa_array">The array to have another array added to</param>
/// <param name="last_index">The index in the base_array that the second_array will be added to</param>
/// <param name="seconed_array">The array that is added</param>
/// <param name="size">The number of elements of second_array to be added to base_array</param>
void concatenate_array(int* basa_array, int last_index, int* seconed_array, int size);


/// <summary>
/// Converts an array of ints to chars by calculating the decimal value of every 8 consecutive bits
/// </summary>
/// <param name="source">source int array</param>
/// <param name="dest">destination char array</param>
/// <param name="num_of_bytes">size of char array</param>
/// <returns>send result</returns>
void int_to_char(int* source, char* dest, int num_of_bytes);


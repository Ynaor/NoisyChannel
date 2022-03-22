/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (312497084)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#ifndef __HardCodedData_H__
#define __HardCodedData_H__

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//#include <WinSock2.h>
#include <Windows.h>
//#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


// Bounds (when possible)
#define BITS_IN_BYTE			8
#define PORT_MIN_VALUE			0
#define PORT_MAX_VALUE			65535											// Max port number
#define DATA_BITS_IN_FRAME		26												// Total number of data bits in frame 
#define BITS_IN_FRAME			31												// Total number of bits in frame, including the check bits
#define DEFAULT_HAMMING_BITS	5												// Default number of Hamming check bits
#define FRAMES_IN_PACKET		32												// Max number of Hamming intervals in a packet	          			
#define MAX_BYTES_IN_PACKET		FRAMES_IN_PACKET		* BITS_IN_FRAME			// Total number of bytes in packet
#define DATA_BYTES_IN_PACKET	FRAMES_IN_PACKET		* DATA_BITS_IN_FRAME	// Total number of data bytes in packet
#define MAX_BITS_IN_PACKET		MAX_BYTES_IN_PACKET		* BITS_IN_BYTE			// Max number of bits in packet
#define DATA_BITS_IN_PACKET		DATA_BYTES_IN_PACKET	* BITS_IN_BYTE			// Total number of bits in packet

#define MAX_FN					300												// Define max file name length to 300 (includin extentions)


typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } TransferResult_t;

#endif



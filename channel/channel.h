/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (312497084)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/


#pragma once

#pragma comment(lib, "Ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// ************************************************
// ***********       Includes       ***************
// ************************************************
#include <winsock2.h>
#include <ws2def.h>
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <random>
#include <ws2tcpip.h>


// ************************************************
// ***********       Defines       ****************
// ************************************************
#define RANDOM_PORT         0
#define SINGLE_BIT_MASK		1
#define BITS_IN_BYTE        8
#define BUFFER_SIZE_BYTES	100000    // 31 bytes per "frame", 20 frames per packet
#define TWO_POWER_FIFTEEN   32768  // 2^15
#define TWO_POWER_SIXTEEN   65536  // 2^16
#define HOSTNAME_MAX_LEN    350


// ************************************************
// ***********  Channel functions  ****************
// ************************************************

// initiates a new winsock with wsaData attached to it
void WinsockInit(WSADATA* wsaData);

// generates random noise according to the probability argument passed by the user
void RandomNoise(int aProbability, char* aBuffer, unsigned int aRandSeed, int *aFlippedBits, int abytesRecieved);

// generates deterministic noise according to the cycle length argument passed by the user
void DeterministicNoise(int aCycle, char* aBuffer, int *aFlippedBits, int abytesRecieved);

// retrieves the current host ip address, will be used for input ip address for both clients (sender and reciever)
void getHostIp(in_addr* aHostAddr);

// create a new socket and bind it if this socket is for listening
SOCKET newSocket(sockaddr_in* aClientAddr, int* aAutoPort, BOOL aIsListen);

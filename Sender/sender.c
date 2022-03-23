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

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sender_functions.h"

#ifndef _DEBUG
/// <summary>
/// Sender main. Inputs:
///		1. IP (X.X.X.X format)
///		2. server listening port
/// </summary>
/// <returns>zero if succesful, one otherwise</returns>
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("ERROR: Number of arguments to the process is incorrect\n");
		exit(1);
	}

	int port = atoi(argv[2]);

	if (port < PORT_MIN_VALUE || port > PORT_MAX_VALUE) {
		printf("ERROR: invalid port number\n");
		exit(1);
	}

	int ret_val = boot_client(argv[1], port);

	return ret_val;
}

#endif _DEDBUG
/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (312497084)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#include "channel.h"

int main(int argc, char* argv[])
{

	// Global variables
	int FlippedBits = 0;
	int senderPort = 0;
	int recieverPort = 0;
	int sockAddrSize = sizeof(sockaddr_in);
	char* noiseFlag;
	int noiseSeed;
	int noiseLevel;
	char continueString[4];
	sockaddr_in ReceiverAddr;
	sockaddr_in SenderAddr;
	in_addr ChannelIpAddress;


	// Setting the Buffer for incoming messages. using malloc for future increase of size during runtime
	char *messageBuffer = (char*)malloc(sizeof(char)*BUFFER_SIZE_BYTES + 1);
	if (messageBuffer == NULL)
	{
		std::cerr << "Error allocating memory\n";
		exit(1);
	}

	// check that number of arguments is valid
	if (argc < 3)
	{
		std::cerr << "Not enough arguments given to Channel.exe\n";
		exit(1);
	}

	noiseFlag = argv[1];
	noiseLevel = atoi(argv[2]);
	if (argc == 4)					// random noise state
	{
		noiseSeed = atoi(argv[3]);
	}

	// Init Winsock2
	WSADATA wsadata;
	WinsockInit(&wsadata);

	// Main logic
	while (TRUE)
	{
		// Creating 2 listening sockets for the Sender & Receiver
		SOCKET SenderListenSock = newSocket(&SenderAddr, &senderPort, TRUE);
		SOCKET RecieverListenSock = newSocket(&ReceiverAddr, &recieverPort, TRUE);

		// getting the current host ip address -> will be set as server ip
		getHostIp(&ChannelIpAddress);

		// printing connection info for the user
		std::cout << "sender socket: IP address = " << inet_ntoa(ChannelIpAddress);
		std::cout << " port = " << senderPort << "\n";
		std::cout << "Reciever socket: IP address = " << inet_ntoa(ChannelIpAddress);
		std::cout << " port = " << recieverPort << "\n";

		// accepting connection for data send/recieve
		SOCKET SenderDataSock = accept(SenderListenSock, (SOCKADDR*)&SenderAddr, &sockAddrSize);
		SOCKET RecieverDataSock = accept(RecieverListenSock, (SOCKADDR*)&ReceiverAddr, &sockAddrSize);
		std::cout << "Got connection for both clients!\n";

		// start receiving message
		int bytesRecieved = recv(SenderDataSock, messageBuffer, BUFFER_SIZE_BYTES, 0);
		messageBuffer[bytesRecieved] = '\0';     // set closing char for the incoming data
		
		// Done receiving data from sender
		closesocket(SenderDataSock);

		#ifdef _DEBUG
		std::cout << "Recieved " << bytesRecieved << " bytes\n ";
		#endif
	
		
		// Adding noise according to user specified flag - deterministic or random
		if (!strcmp("-d", noiseFlag)) // deterministic noise
		{
			DeterministicNoise(noiseLevel, messageBuffer, &FlippedBits, bytesRecieved);
		}
		if (!strcmp("-r", noiseFlag)) // Random noise
		{
			RandomNoise(noiseLevel, messageBuffer, noiseSeed, &FlippedBits, bytesRecieved);
		}

		// Send the message (with noise) to the reciever
		int bytesSent = send(RecieverDataSock, messageBuffer, bytesRecieved, 0);
		std::cout << "retransmitted " << bytesSent << " bytes, ";
		std::cout << "flipped " << FlippedBits << " bits \n";
		FlippedBits = 0;

		// Done sending - Close the Socket
		closesocket(RecieverDataSock);

		// Will allow to send another message
		std::cout << "continue? (yes/no)\n";
		std::cin >> continueString;

		if (!strcmp(continueString, "no"))
		{
			std::cout << "Bye Bye!\n";
			exit(0);
		}
	}
	free (messageBuffer);  // cleaning up the memory
	return 0;
}
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


	// Buffer size - will increase if file size is too large for current buffer
	char *messageBuffer = (char*)malloc(sizeof(char)*BUFFER_SIZE_BYTES + 1);
	if (messageBuffer == NULL)
	{
		std::cerr << "Error allocating memory\n";
		exit(1);
	}
	// TODO: deal with input errors
	noiseFlag = argv[1];
	noiseLevel = atoi(argv[2]);
	if (argc == 4)             // random noise state
	{
		noiseSeed = atoi(argv[3]);
	}


	WSADATA wsadata;
	WinsockInit(&wsadata);


	while (TRUE)
	{
		SOCKET SenderListenSock = newSocket(&SenderAddr, &senderPort, TRUE);
		SOCKET RecieverListenSock = newSocket(&ReceiverAddr, &recieverPort, TRUE);

		getHostIp(&ChannelIpAddress); // getting the current host ip address -> will be set as server ip

		// printing connection info for the user
		std::cout << "sender socket: IP address = " << inet_ntoa(ChannelIpAddress);
		std::cout << " port = " << senderPort << "\n";
		std::cout << "Reciever socket: IP address = " << inet_ntoa(ChannelIpAddress);
		std::cout << " port = " << recieverPort << "\n";

		// accepting connection for data send/recieve
		SOCKET SenderDataSock = accept(SenderListenSock, (SOCKADDR*)&SenderAddr, &sockAddrSize);
		SOCKET RecieverDataSock = accept(RecieverListenSock, (SOCKADDR*)&ReceiverAddr, &sockAddrSize);
		std::cout << "Got connection for both clients!\n";

		
		// start getting message
		int bytesRecieved = recv(SenderDataSock, messageBuffer, BUFFER_SIZE_BYTES, 0);
		messageBuffer[BUFFER_SIZE_BYTES] = '\0';                               // set the whole buffer as one string
		
		// Done recieving data from sender
		closesocket(SenderDataSock);

		#ifdef _DEBUG
		std::cout << "Recieved " << bytesRecieved << " bytes\n ";
		#endif
	
		
		// Adding noise according to user specified flag
		
		if (!strcmp("-d", noiseFlag)) // deterministic noise
		{
			DeterministicNoise(noiseLevel, messageBuffer, &FlippedBits);
		}
		if (!strcmp("-r", noiseFlag))
		{
			RandomNoise(noiseLevel, messageBuffer, noiseSeed, &FlippedBits);
		}

		// send message with noise
		int bytesSent = send(RecieverDataSock, messageBuffer, bytesRecieved, 0);
		std::cout << "retransmitted " << bytesSent << " bytes, ";
		std::cout << "flipped " << FlippedBits << " bits \n";
		FlippedBits = 0;
		closesocket(RecieverDataSock);


		std::cout << "continue? (yes/no)\n";
		std::cin >> continueString;

		if (!strcmp(continueString, "no"))
		{
			std::cout << "Bye Bye!\n";
			exit(0);
		}
	}

	return 0;

}
#include "channel.h"


// ********************************************
// **********  Socket Handling   **************
// ********************************************


// Will be generating random noise with given seed 
void RandomNoise(int aProbability, char* aBuffer, unsigned int aRandSeed, int *aFlippedBits)
{
	unsigned char mask; 
	std::default_random_engine generator(0); 
	std::uniform_int_distribution<int> distribution(0,TWO_POWER_SIXTEEN);
	long int randomNumber;

	for (int byte = 0; byte < BUFFER_SIZE_BYTES; byte++) 
	{
		mask = SINGLE_BIT_MASK;
		for (int bit = 0; bit < BITS_IN_BYTE; bit++)
		{
			randomNumber = distribution(generator);      // generating a random number in range [0, 2^16]
			if (randomNumber < aProbability)
			{
				aBuffer[byte] = aBuffer[byte] ^ mask;    // flip the desired bit
				(*aFlippedBits)++;
			}

			mask <<= 1; // moving mask bit to the left
		}
	}
}

void DeterministicNoise(int aCycle, char* aBuffer, int *aFlippedBits)
{
	int counter = 0;
	unsigned char mask;
	
	for (int byte = 0; byte < BUFFER_SIZE_BYTES; byte++)
	{
		mask = SINGLE_BIT_MASK;
		for (int bit = 0; bit < BITS_IN_BYTE; bit++)
		{
			if (counter == aCycle)
			{
				aBuffer[byte] = aBuffer[byte] ^ mask;    // flip the desired bit when we counter == cycle
				(*aFlippedBits)++;
				counter = 0;                             // reset counter
			}
			mask <<= 1;
			counter++;
		}
	}
}

void WinsockInit(WSADATA *wsaData)
{
	const auto api_ver = MAKEWORD(2, 2);
	if (WSAStartup(api_ver, wsaData) != NO_ERROR)
	{
		std::cerr << "Winsock initialization failed\n" << WSAGetLastError();
		exit(1);
	}
}

void getHostIp(in_addr *aHostAddr)
{
	char hostName[HOSTNAME_MAX_LEN + 1] = {0};
	struct hostent* hostIpAddr;	
	
	// With the Assist of Ido Barak
	if (gethostname(hostName, HOSTNAME_MAX_LEN - 1) != 0)
	{
		std::cerr << "Error getting local HostName \n";
		exit(1);
	}

	hostIpAddr = gethostbyname(hostName);
	if (hostIpAddr == NULL)
	{
	std::cerr << "Error getting local IPv4 address \n";
		exit(1);
	}

	// set the host ip address for printing
	memcpy(aHostAddr, hostIpAddr->h_addr_list[0], sizeof(in_addr));
 }

SOCKET newSocket(sockaddr_in *aClientAddr, int* aAutoPort, BOOL aIsListen)
{
	SOCKET s;

	// set socket parameters
	aClientAddr->sin_family = AF_INET;

	#ifdef _DEBUG
	aClientAddr->sin_port = 5555;
	aClientAddr->sin_addr.s_addr = inet_addr("127.0.0.1");
	#else
	aClientAddr->sin_port = RANDOM_PORT;
	aClientAddr->sin_addr.s_addr = INADDR_ANY;
	#endif
	

	// create the new socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cerr << "Could not create TCP socket\n" << WSAGetLastError();
		exit(1);
	}

	// if socket is for listening: will handle it correctly
	if (aIsListen)
	{
		// bind
		int bindRes = bind(s, (SOCKADDR*)aClientAddr, sizeof(*aClientAddr));
		if (bindRes)
		{
			std::cerr << "Error while binding new socket\n" << WSAGetLastError();
			exit(1);
		}

		//listen
		int listenRes = listen(s, 1);
		if (listenRes)
		{
			std::cerr << "Could not listen to socket\n" << WSAGetLastError();
			exit(1);
		}
	}

	// set the auto selected port from operating system
	int addrSize = sizeof(*aClientAddr);
	getsockname(s, (SOCKADDR*)aClientAddr, &addrSize);


	*aAutoPort = ntohs(aClientAddr->sin_port);
}
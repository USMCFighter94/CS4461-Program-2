#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

void errorAndExit(char *s) {
    perror(s);
    exit(1);
}

/**
* establishConnection - setup connection to server
* @return - socket to connect to
*/
int establishConnection(int port) {
  struct sockaddr_in si_me;
  int sock;

  // Create UDP socket
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    errorAndExit("socket");

  // Zero out mem structure
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind socket to port
  if (bind (sock, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
    errorAndExit("bind");

  return sock;
}

int main(void) {
  struct sockaddr_in si_other; // Address of the sender
  unsigned int slen = sizeof(si_other); // Length of the address of the sender
  int portMapperSocket, dnsResolverSocket, recv_len;
  char buffer[4];

  portMapperSocket = establishConnection(4461);
  // Listen for data
  while (1) {
    printf("PortMapper: Waiting...\n");
    fflush(stdout);

    // Try to receive data, this is a blocking call
    if ((recv_len = recvfrom(portMapperSocket, buffer, 4, 0, (struct sockaddr *) &si_other, &slen)) == -1)
      errorAndExit("recvfrom");

    char *responseString = malloc(10 * sizeof(char));
    // Check data received and prepare response
    responseString = (strcpy(buffer, "DNS") != 0) ? "(0, 0000)" : "(1, 4723)";

    // Reply with the same data
    if (sendto(portMapperSocket, responseString, strlen(responseString), 0, (struct sockaddr *) &si_other, slen) == -1)
      errorAndExit("sendto");

    free(responseString);
  }
  // Close socket
  close(portMapperSocket);

  dnsResolverSocket = establishConnection(4461);
  // Listen for data
  while (1) {
    printf("DNSLookup: Waiting...\n");
    fflush(stdout);

    // Try to receive data, this is a blocking call
    if ((recv_len = recvfrom(dnsResolverSocket, buffer, 4, 0, (struct sockaddr *) &si_other, &slen)) == -1)
      errorAndExit("recvfrom");

    char *responseString = malloc(10 * sizeof(char));
    // Check data received and prepare response
    responseString = (strcpy(buffer, "DNS") != 0) ? "(0, 0000)" : "(1, 4723)";

    // Reply with the same data
    if (sendto(dnsResolverSocket, responseString, strlen(responseString), 0, (struct sockaddr *) &si_other, slen) == -1)
      errorAndExit("sendto");

    free(responseString);
  }
  // Close socket
  close(dnsResolverSocket);
  return 0;
}

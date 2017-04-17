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

  return sock;
}

int main(void) {
  struct sockaddr_in si_other; // Address of the sender
  unsigned int slen = sizeof(si_other); // Length of the address of the sender
  int portMapperSocket, dnsResolverSocket, recv_len;
  char buffer[20];

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(4461);
  si_other.sin_addr.s_addr = htonl(INADDR_ANY);

  printf("DNSLookup: Contacting port mapper...\n");
  portMapperSocket = establishConnection(4461);

  // Contact the port mapper to get the port for the DNS Resolver
  if (sendto(portMapperSocket, "DNS", strlen("DNS"), 0, (struct sockaddr *) &si_other, slen) == -1)
  errorAndExit("sendto");

  printf("DNSLookup: Connected to port mapper on port 4461.\n");
  // Try to receive data, this is a blocking call
  if ((recv_len = recvfrom(portMapperSocket, buffer, 10, 0, (struct sockaddr *) &si_other, &slen)) == -1)
  errorAndExit("recvfrom");

  // Check data received and prepare response
  char *responseString = malloc(10 * sizeof(char));

  const char *ch = ",";
  responseString = strtok(strstr(buffer, ch), ")"); // strstr gets everything after the comma, strtok strips out the last )
  printf("DNS Lookup: DNS Lookups are on port %s.\n", responseString + 2);

  // free(responseString);
  // Close socket
  close(portMapperSocket);

  // dnsResolverSocket = establishConnection(4461);
  // // Listen for data
  // while (1) {
  //   printf("DNSLookup: Waiting...\n");
  //   fflush(stdout);
  //
  //   // Try to receive data, this is a blocking call
  //   if ((recv_len = recvfrom(dnsResolverSocket, buffer, 4, 0, (struct sockaddr *) &si_other, &slen)) == -1)
  //     errorAndExit("recvfrom");
  //
  //   char *responseString = malloc(10 * sizeof(char));
  //   // Check data received and prepare response
  //   responseString = (strcpy(buffer, "DNS") != 0) ? "(0, 0000)" : "(1, 4723)";
  //
  //   // Reply with the same data
  //   if (sendto(dnsResolverSocket, responseString, strlen(responseString), 0, (struct sockaddr *) &si_other, slen) == -1)
  //     errorAndExit("sendto");
  //
  //   free(responseString);
  // }
  // // Close socket
  // close(dnsResolverSocket);
  return 0;
}

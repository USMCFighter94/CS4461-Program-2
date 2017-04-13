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
int establishConnection(void) {
  struct sockaddr_in si_me;
  int sock;

  // Create UDP socket
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    errorAndExit("socket");

  // Zero out mem structure
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(4461);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind socket to port
  if (bind (sock, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
    errorAndExit("bind");

  return sock;
}

int main(void) {
  struct sockaddr_in si_other;
  int sock, recv_len;
  unsigned int slen = sizeof(si_other);
  char buffer[512];

  sock = establishConnection();
  // Listen for data
  while (1) {
    printf("Waiting...\n");
    fflush(stdout);

    // Try to receive data, this is a blocking call
    if ((recv_len = recvfrom(sock, buffer, 512, 0, (struct sockaddr *) &si_other, &slen)) == -1)
      errorAndExit("recvfrom");

    // Print data received
    printf("Data: %s\n", buffer);

    // Reply with the same data
    if (sendto(sock, buffer, recv_len, 0, (struct sockaddr *) &si_other, slen) == -1)
      errorAndExit("sendto");
  }
  // Close socket
  close(sock);

}

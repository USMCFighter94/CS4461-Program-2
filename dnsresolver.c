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

char* getField(char* line, int num) {
    char* token;
    for (token = strtok(line, ","); token && *token; token = strtok(NULL, ",\n"))
        if (!num--)
            return token;
    return NULL;
}

char* readCSV(char* url) {
  char line[128];
  FILE* dnsEntries = fopen("dnsentries.csv", "r");

  while (fgets(line, 128, dnsEntries)) {
      char* field = getField(strdup(line), 0);

      if (strcmp(field, url) == 0) {
        char* ipAddress = getField(strdup(line), 1);
        printf("Got IP Address%s for url %s\n", ipAddress, url);
        return ipAddress;
      }
  }
  return "0.0.0.0";
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
  si_me.sin_port = htons(4723);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind socket to port
  if (bind (sock, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
    errorAndExit("bind");

  return sock;
}

int main(void) {
  struct sockaddr_in si_other; // Address of the sender
  unsigned int slen = sizeof(si_other); // Length of the address of the sender
  int sock, recv_len;
  char *string, buffer[50];

  sock = establishConnection();
  // Listen for data
  while (1) {
    printf("DNS Resolver: Waiting...\n");
    fflush(stdout);

    // Try to receive data, this is a blocking call
    if ((recv_len = recvfrom(sock, buffer, 50, 0, (struct sockaddr *) &si_other, &slen)) == -1)
      errorAndExit("recvfrom");

    char *responseString = malloc(16 * sizeof(char));
    string = readCSV("amazon.com");
    // Check data received and prepare response
    if (strcpy(string, "0.0.0.0") == 0)
      sprintf(responseString, "(1,%s)", string);
    else
      responseString = "(0, 0.0.0.0)";

    // Reply with the same data
    if (sendto(sock, responseString, strlen(responseString), 0, (struct sockaddr *) &si_other, slen) == -1)
      errorAndExit("sendto");

    free(responseString);
  }
  // Close socket
  close(sock);
  return 0;
}

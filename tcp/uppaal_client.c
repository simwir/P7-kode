#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "uppaal_client.h"

/**
 * Function called by UPPAAL upon model initialisation.
 */
void __ON_CONSTRUCT__() {}

/**
 * Fuction called by UPPAAL upon model termination.
 */
void __ON_DESTRUCT__() {}


/**
 * Connects to the Webots TCP server and returns a socket identifier.
 */
int wb_connect(char* hostname, int port) {
  int socket_fd;
  struct hostent *server;
  struct sockaddr_in server_address;

  // Create socket.
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Setup host address.
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  server = gethostbyname(hostname);

  if (server) {
    memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
    return socket_fd;
  } else {
    return close(socket_fd);
  }
}


/**
 * Disconnects from the Webots TCP server.
 */
bool wb_disconnect(int socket_id) {
  if (close(socket_id) == -1) {
    return false;
  } else {
    return true;
  }
}


/**
 * Sends a command to the Webots TCP server.
 */
bool send_command(int socket_id, char* input) {
  char* output;
  struct command_t *command = parse_command(input);

  // Format output
  // sprintf(output, ...)

  if (send(socket_id, output, strlen(output), 0) == -1) {
    return false;
  } else {
    return true;
  }
}


/**
 * Parse an input string to command_t format.
 */
struct command_t *parse_command(char* input) {
  int i;
  char *copy, *part;
  struct command_t *command = malloc(sizeof(struct command_t));
  command->parameters = malloc(0);

  // Copy input to allow strtok.
  copy = strdup(input);
  part = strtok(copy, " ");
  i = 0;
  while (part != NULL) {
    if (i == 0) {
      // Set command_t command.
      command->command = strdup(part);
    } else {
      // Append parameter to command_t.
      command->parameters = realloc(command->parameters, i * sizeof(char));
      command->parameters[i - 1] = strdup(part);
    }

    // Next substring in input.
    part = strtok(NULL, " ");
    i++;
  }
  free(copy);

  return command;
}

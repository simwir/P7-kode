struct command_t {
  char* command;
  char** parameters;
};

void __ON_CONSTRUCT__();
void __ON_DESTRUCT__();
int wb_connect(char* hostname, int port);
bool wb_disconnect(int socket_id);
bool send_command(int socket_id, char* commnad);
struct command_t *parse_command(char* command);

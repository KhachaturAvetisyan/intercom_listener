void send_msg_socket(int fd, const char* msg)
{
    int16_t size_msg = (int16_t) strlen(msg);

    // send size of command
    if(send(fd, &size_msg, 2, 0) < 0)
    {
        perror("send size error");
        exit(EXIT_FAILURE);
    }

    // send command
    if(send(fd, msg, size_msg, 0) < 0)
    {
        perror("send msg error");
        exit(EXIT_FAILURE);
    }

}

void read_msg_socket(int fd, char** msg)
{
    int16_t size_msg = 0;

    // read size of command output
    if (recv(fd, &size_msg, 2, 0) < 0) {
        perror("read size error");
        exit(EXIT_FAILURE);
    }

    //read command output
    *msg = (char *)(malloc(size_msg));
    if (recv(fd, *msg, size_msg, 0) < 0) {
        perror("read msg error");
        exit(EXIT_FAILURE);
    }

}
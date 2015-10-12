int send_msg(char* str)
{
    char buf[SIZE];
    // len = length of message 
    buf[len -1] = '\n';
    //...
    send(fd, buf, len);
}

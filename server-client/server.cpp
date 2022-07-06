#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#define BUFFER_LENGTH 128

static const char* server_path = "/tmp/server";

int main() {

    int sd1 = -1, sd2 = -1, rc;
    char buffer[BUFFER_LENGTH];
    struct sockaddr_un addr;

    // socket() * Gets the file descriptor *
    sd1 = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sd1 < 0) {
        perror("socket() Error ");
        return -1;
    }

    // unlink() * Remove the link name *
    unlink(addr.sun_path);

    // bind() * Associating socket file descriptor with local address *
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, server_path, sizeof(addr.sun_path)-1);

    rc = bind(sd1, (struct sockaddr*)&addr, sizeof(addr));
    if (rc < 0) {
        printf("%d", sd1);
        perror("bind() Error ");
        return -1;
    }

    // listen() * Setting the number of connections allowed on the incoming queue *
    rc = listen(sd1, 10);
    if (rc < 0) {
        perror("listen() Error: ");
        return -1;
    }

    printf("Ready for connect().\n");

    // accept() * Creating new file descriptor after client connects *
    sd2 = accept(sd1, NULL, NULL);
    if (sd2 < 0) {
        perror("accept() Error: ");
        return -1;
    }

    // setsockopt() * Constrains recv() to not wake up until it receives all 256 bytes of data. *
    int length = BUFFER_LENGTH;
    rc = setsockopt(sd2, SOL_SOCKET, SO_RCVLOWAT,(char *)&length, sizeof(length));
    if (rc < 0) {
        perror("setsockopt() Error: ");
        return -1;
    }

    // recv() * Receives data from client *
    rc = recv(sd2, buffer, sizeof(buffer), 0);
    if (rc < 0) {
        perror("recv() Error: ");
        return -1;
    }

    if (rc == 0 || rc < sizeof(buffer)) {
        printf("The client closed the connection before all of the data was sent.\n");
        return -1;
    }

    printf("The message is: %s \n", buffer);
    printf("%d bytes of data were received\n", rc);

    // send() * Echo the data back to the client *
    rc = send(sd2, buffer, sizeof(buffer), 0);
    if (rc < 0) {
        perror("send() Error: ");
        return -1;
    }

    // Close down open socket descriptors.
    if (sd1 != -1) close(sd1);
    if (sd2 != -1) close(sd2);

    // Remove the UNIX path name from the file system
    unlink(server_path);

    return 0;
}
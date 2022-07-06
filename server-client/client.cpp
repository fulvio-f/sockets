#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#define BUFFER_LENGTH 128

static const char* server_path = "/tmp/server";

int main(int argc, char *argv[]) {

    int sd1 = -1, rc, echoBytes;
    char buffer[BUFFER_LENGTH];
    struct sockaddr_un addr;

    // socket() * Gets the file descriptor *
    sd1 = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sd1 < 0) {
        perror("socket() Error: ");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, server_path);

    // connect() * Establishes connection with the server *
    rc = connect(sd1, (struct sockaddr *)&addr, SUN_LEN(&addr));
    if (rc < 0) {
        perror("connect() Error:");
        return -1;
    }

    // send() * Send message to server *
    strcpy(buffer, argv[1]);
    rc = send(sd1, buffer, sizeof(buffer), 0);
    if (rc < 0) {
        perror("send() Error: ");
        return -1;
    }

    // recv() * Receiving echo from server *
    echoBytes = 0;
    while (echoBytes < BUFFER_LENGTH) {
        rc = recv(sd1, & buffer[echoBytes],
        BUFFER_LENGTH - echoBytes, 0);
        if (rc < 0) {
            perror("recv() Error: ");
            break;
        }
        else if (rc == 0) {
            printf("The server closed the connection.\n");
            break;
        }
        echoBytes += rc;
    }

    // Close down open socket descriptors.
    if (sd1 != -1) close(sd1);

    return 0;
}
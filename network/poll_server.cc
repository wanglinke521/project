#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

using namespace std;
#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int lfd, cfd;
    struct sockaddr_in address;
    struct sockaddr_in client_address;
    int client_addrlen = sizeof(client_address);
    
    char buffer[BUFFER_SIZE];

    lfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(lfd, (struct sockaddr*)&address, sizeof(address));

    listen(lfd, 1024);

    pollfd fds[1024];
    fds[0].fd = lfd;
    fds[0].events = POLLIN;

    int cfd_count = 0;

    while(true) {
        int pollcout = poll(fds, cfd_count + 1, -1);

        if(fds[0].revents & POLLIN) {
            cfd = accept(lfd, (struct sockaddr*)&client_address, (socklen_t*)&client_addrlen);
            cfd_count++;
            fds[cfd_count].fd = cfd;
            fds[cfd_count].events = POLLIN;
        }

        for(int i = 1; i <= cfd_count; i++) {
            if (fds[i].revents & POLLIN) {
                int n = read(fds[i].fd, buffer, BUFFER_SIZE);
                if (n == 0) {
                    close(fds[i].fd);
                    cfd_count--;
                    fds[i] = fds[cfd_count];
                    i--;
                } else {
                    for (int j = 0; j < n; j++) {
                        buffer[j] = toupper(buffer[j]);
                    }
                    write(fds[i].fd, buffer, n);
                }
            }
        }
    }
    close(lfd);
    return 0;
}
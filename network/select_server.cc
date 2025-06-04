#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

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
    fd_set read_fds, all_fds;
    int max_fd = lfd;

    FD_ZERO(&read_fds);
    FD_ZERO(&all_fds);
    FD_SET(lfd, &all_fds);

    while(true) {

        read_fds = all_fds;
        select(max_fd + 1 , &read_fds, NULL, NULL, NULL);
        
        if (FD_ISSET(lfd, &read_fds)) {
            cfd = accept(lfd, (struct sockaddr *)&client_address, (socklen_t*)&client_addrlen);
            FD_SET(cfd, &all_fds);
            max_fd = cfd > max_fd ? cfd : max_fd;
        }
        for (int i = lfd + 1; i <= max_fd; i++) {
            if(FD_ISSET(i, &read_fds)) {
                int n = read(i, buffer, sizeof(buffer));
                if (n == 0) {
                    close(i);
                    FD_CLR(i, &all_fds);
                    continue;
                }
                for (int i = 0; i < n; i++) {
                    buffer[i] = toupper(buffer[i]);
                }
                write(i, buffer, n);
            }
        }
    }

    close(lfd);

    return 0;
}
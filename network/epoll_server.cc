#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <cctype>

using namespace std;
#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_EVENTS 1024

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

    int epollfd = epoll_create1(0);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, lfd, &ev);
    while (true) {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == lfd) {
                cfd = accept(lfd, (struct sockaddr*)&client_address, (socklen_t*)&client_addrlen);
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, cfd, &ev);
            } else {
                int n = read(events[i].data.fd, buffer, BUFFER_SIZE);
                if (n == 0) {
                    close(events[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                } else {
                    for (int j = 0; j < n; j++) {
                        buffer[j] =  toupper(buffer[j]);
                    }
                    write(events[i].data.fd, buffer, n);
                }
            }
        }
    }
    

    close(lfd);
    return 0;
}
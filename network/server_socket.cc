#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;
#define PORT 8080
#define BUFFER_SIZE 4096

/*
1. 创建监听套接字
2. 绑定本地ip和端口
3. listten 设置监听上限
4. accept 阻塞监听客户端
*/

int main() {
    int lfd, cfd;
    struct sockaddr_in address;
    struct sockaddr_in client_address;

    char buffer[BUFFER_SIZE];
    int client_addrlen = sizeof(client_address);

    lfd = socket(AF_INET, SOCK_STREAM, 0);        
    if (lfd < 0) {
      cout << "create lfd error" << endl;
      return -1;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(lfd, (struct sockaddr *)&address, sizeof(address));
    listen(lfd, 1024);
    cfd = accept(lfd, (struct sockaddr *)&client_address, (socklen_t*)&client_addrlen);

    int n = read(cfd, buffer, BUFFER_SIZE);
    for (int i = 0; i < n; i++) {
        buffer[i] = toupper(buffer[i]);
    }
    write(cfd, buffer, n);
    
    close(cfd);
    close(lfd);
    return 0;
}
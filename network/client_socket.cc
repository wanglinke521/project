#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;
#define PORT 8080
#define BUFFER_SIZE 4096

/*
1.创建服务端套接字
2.connet连接服务端
*/

int main() {
    int client_fd;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    char rev_buffer[BUFFER_SIZE];

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr); // 字符串ip转换为网络字节序

    connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address));

    fgets(buffer, BUFFER_SIZE, stdin);
    write(client_fd, buffer, strlen(buffer));
    int n = read(client_fd, rev_buffer, BUFFER_SIZE);
    std::cout << rev_buffer << std::endl;

    return 0;
}
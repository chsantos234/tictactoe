#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Erro ao criar socket\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Erro ao conectar\n";
        return -1;
    }

    while (true) {
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cerr << "Conexão com o servidor perdida\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << buffer;

        if (strstr(buffer, "Sua vez") != NULL) {
            std::string move;
            std::cout << "Digite a linha e coluna (exemplo: 1 1): ";
            std::getline(std::cin, move);
            send(sock, move.c_str(), move.size(), 0);
        }
    }

    close(sock);
    return 0;
}

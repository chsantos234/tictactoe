#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024

char board[3][3];
sem_t player_turn[2];

void initializeBoard() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

std::string getBoardString() {
    std::string boardString;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            boardString += board[i][j];
            if (j < 2) boardString += " | ";
        }
        boardString += "\n";
        if (i < 2) boardString += "--|---|--\n";
    }
    return boardString;
}

void printBoard() {
    std::cout << getBoardString() << std::endl;
}

bool isWin(char mark) {
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == mark && board[i][1] == mark && board[i][2] == mark) ||
            (board[0][i] == mark && board[1][i] == mark && board[2][i] == mark))
            return true;
    }
    if ((board[0][0] == mark && board[1][1] == mark && board[2][2] == mark) ||
        (board[0][2] == mark && board[1][1] == mark && board[2][0] == mark))
        return true;

    return false;
}

bool isDraw() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ') return false;
    return true;
}

void *player_thread(void *arg) {
    int *args = (int *)arg;
    int player = args[0];
    int client_socket = args[1];
    char mark = (player == 0) ? 'X' : 'O';
    char buffer[BUFFER_SIZE];

    while (true) {
        sem_wait(&player_turn[player]); // espera o turno do jogador

        std::string boardState = getBoardString();
        snprintf(buffer, BUFFER_SIZE, "Tabuleiro atual:\n%s\nSua vez (jogador %d, %c):\n", boardState.c_str(), player + 1, mark);
        send(client_socket, buffer, strlen(buffer), 0);
        
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cerr << "Conexão perdida com o jogador " << player + 1 << std::endl;
            break;
        }

        int row = buffer[0] - '0';
        int col = buffer[1] - '0';

        if (board[row][col] == ' ') {
            board[row][col] = mark;
            if (isWin(mark)) {
                snprintf(buffer, BUFFER_SIZE, "Jogador %d venceu!\n", player + 1);
                send(client_socket, buffer, strlen(buffer), 0);
                close(client_socket);
                break;
            }
            if (isDraw()) {
                snprintf(buffer, BUFFER_SIZE, "Empate!\n");
                send(client_socket, buffer, strlen(buffer), 0);
                close(client_socket);
                break;
            }
            sem_post(&player_turn[1 - player]); // libera o semáforo
        } else {
            snprintf(buffer, BUFFER_SIZE, "Posição inválida. Tente novamente.\n");
            send(client_socket, buffer, strlen(buffer), 0);
            sem_post(&player_turn[player]);
        }
    }
    return NULL;
}

int main() {
    int server_fd, new_socket[2];
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro ao fazer bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Erro ao ouvir");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Aguardando jogadores..." << std::endl;

    new_socket[0] = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    std::cout << "Jogador 1 conectado." << std::endl;

    new_socket[1] = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    std::cout << "Jogador 2 conectado. Iniciando o jogo!" << std::endl;

    initializeBoard();
    sem_init(&player_turn[0], 0, 1);
    sem_init(&player_turn[1], 0, 0);

    pthread_t tid[2];
    int args1[] = {0, new_socket[0]};
    int args2[] = {1, new_socket[1]};

    pthread_create(&tid[0], NULL, player_thread, args1);
    pthread_create(&tid[1], NULL, player_thread, args2);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    sem_destroy(&player_turn[0]);
    sem_destroy(&player_turn[1]);

    close(new_socket[0]);
    close(new_socket[1]);
    close(server_fd);
    return 0;
}

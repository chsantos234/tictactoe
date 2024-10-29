#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>

std::binary_semaphore player1Turn(1); // Starts with player 1's turn
std::binary_semaphore player2Turn(0); // Player 2 waits

char board[3][3]; // The tic-tac-toe board

void displayBoard() {
    std::cout << "\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << board[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

bool checkWin(char mark) {
    // Check rows, columns, and diagonals
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == mark && board[i][1] == mark && board[i][2] == mark) return true;
        if (board[0][i] == mark && board[1][i] == mark && board[2][i] == mark) return true;
    }
    if (board[0][0] == mark && board[1][1] == mark && board[2][2] == mark) return true;
    if (board[0][2] == mark && board[1][1] == mark && board[2][0] == mark) return true;
    
    return false;
}

bool checkDraw() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == '-') {
                return false;
            }
        }
    }
    return true;
}

void playerMove(char mark, std::binary_semaphore& myTurn, std::binary_semaphore& opponentTurn) {
    while (true) {
        myTurn.acquire();  // Wait for my turn

        if (checkWin('X') || checkWin('O') || checkDraw()) {
            opponentTurn.release(); // Ensure the other player can exit
            break;
        }

        int row, col;
        std::cout << "Player " << mark << "'s turn. Enter row and column (0-2): ";
        std::cin >> row >> col;

        if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == '-') {
            board[row][col] = mark;
            displayBoard();

            if (checkWin(mark)) {
                std::cout << "Player " << mark << " wins!\n";
                opponentTurn.release(); // Let the opponent finish
                break;
            }

            if (checkDraw()) {
                std::cout << "It's a draw!\n";
                opponentTurn.release(); // Let the opponent finish
                break;
            }

            opponentTurn.release(); // Let the opponent play
        } else {
            std::cout << "Invalid move! Try again.\n";
            myTurn.release(); // Let the player try again
        }
    }
}

int main() {
    // Initialize the board
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = '-';
        }
    }

    std::cout << "Tic-Tac-Toe Game Start!\n";
    displayBoard();

    // Create player threads
    std::thread player1(playerMove, 'X', std::ref(player1Turn), std::ref(player2Turn));
    std::thread player2(playerMove, 'O', std::ref(player2Turn), std::ref(player1Turn));

    // Wait for both players to finish
    player1.join();
    player2.join();

    std::cout << "Game over!\n";
    return 0;
}
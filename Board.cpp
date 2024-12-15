#include "Piece.h"
#include "Pawn.h"
#include "Rook.h"
#include "Board.h"
#include <iostream>
#include <iomanip>

Board::Board() {
    board = vector<vector<Piece*>>(8, vector<Piece*>(8, nullptr));
}

Board::~Board() {
    // free dynamically allocated pieces
    for (auto& row : board) {
        for (auto& piece : row) {
            delete piece;
        }
    }
}

void Board::initialise() {
    // pawns
    for (int i = 0; i < 8; ++i) {
        board[1][i] = new Pawn('B');
        board[6][i] = new Pawn('W');
    }

    // rooks
    board[0][0] = board[0][7] = new Rook('B');
    board[7][0] = board[7][7] = new Rook('W');

    // knights
    board[0][1] = board[0][6] = new Knight('B');
    board[7][1] = board[7][6] = new Knight('W');

    // bishops
    board[0][2] = board[0][5] = new Bishop('B');
    board[7][2] = board[7][5] = new Bishop('W');

    // queens
    board[0][3] = new Queen('B');
    board[7][3] = new Queen('W');

    // kings
    board[0][4] = new King('B');
    board[7][4] = new King('W');
}

void Board::display() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == nullptr) {
                std::cout << ". ";
            } else {
                string type = board[i][j]->getType();
                char pieceChar = type[0]; // First letter of the type
                std::cout << (board[i][j]->getColor() == 'W' ? pieceChar : tolower(pieceChar)) << " ";
            }
        }
        std::cout << std::endl;
    }
}

bool Board::isValidMove(int startX, int startY, int endX, int endY) {
    return (startX != endX && startY != endY) 
        && startX >= 0 && startX < 8 && startY >= 0 && startY < 8
        && endX >= 0 && endX < 8 && endY >= 0 && endY < 8;
}

bool Board::isPiecePinned(int startX, int startY, char currentPlayer) {
    // remove the piece and see if king is in check
    auto [x, y] = currentPlayer == 'W' ? getWhiteKing() : getBlackKing();
    // TODO: run king's in check function
}

bool Board::movePiece(int startX, int startY, int endX, int endY, char currentPlayer) {
    if (!isValidMove(startX, startY, endX, endY)) {
        std::cout << "Outside of grid bounds!" << std::endl; 
        return false;
    }
    if (board[startX][startY] == nullptr || board[startX][startY]->getColor() != currentPlayer) {
        std::cout << "Invalid piece selection!" << std::endl;
        return false;
    }
    if (isPiecePinned(startX, startY, currentPlayer)) {
        std::cout << "Piece is pinned!" << std::endl;
    }
    if (!board[startX][startY]->isValidPieceMove(startX, startY, endX, endY, board)) {
        std::cout << "Invalid move!" << std::endl;
        return false;
    }

    delete board[endX][endY];
    board[endX][endY] = board[startX][startY];
    board[startX][startY] = nullptr;
    return true;
}
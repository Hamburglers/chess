#include <iostream>
#include <iomanip>
#include "Bishop.h"
#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Piece.h"
#include "Queen.h"
#include "Rook.h"

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
    // clear terminal so looks nice
    std::cout << "\033[2J\033[H";
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == nullptr) {
                std::cout << ". ";
            } else {
                string type = board[i][j]->getType();
                char pieceChar = type[0]; // First letter of the type
                std::cout << (board[i][j]->getColor() == 'W' ? pieceChar : static_cast<char>(tolower(pieceChar))) << " ";
            }
        }
        std::cout << std::endl;
    }
}

bool Board::isWithinBoard(int startX, int startY, int endX, int endY) {
    return (startX != endX || startY != endY) 
        && startX >= 0 && startX < 8 && startY >= 0 && startY < 8
        && endX >= 0 && endX < 8 && endY >= 0 && endY < 8;
}

tuple<int, int> Board::getWhiteKing() {
    return whiteKing;
}

tuple<int, int> Board::getBlackKing() {
    return blackKing;
}

bool Board::isLegalMove(int startX, int startY, int endX, int endY) {
    // Backup the current state
    Piece* movingPiece = board[startX][startY];
    Piece* capturedPiece = board[endX][endY];

    // Make the move temporarily
    char currentPlayer = board[startX][startY]->getColor();
    board[endX][endY] = movingPiece;
    board[startX][startY] = nullptr;

    // Update king position if the moved piece is a king
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            whiteKing = {endX, endY};
        } else {
            blackKing = {endX, endY};
        }
    }

    // Check if the current player's king is in check
    bool isInCheck = false;
    auto [kingX, kingY] = (currentPlayer == 'W') ? whiteKing : blackKing;
    for (int i = 0; i < 8 && !isInCheck; i++) {
        for (int j = 0; j < 8 && !isInCheck; j++) {
            if (board[i][j] && board[i][j]->getColor() != currentPlayer) {
                // Check if the opponent piece can attack the king
                if (board[i][j]->isValidPieceMove(i, j, kingX, kingY, board)) {
                    isInCheck = true;
                }
            }
        }
    }
    // Undo the move to restore the original board state
    board[startX][startY] = movingPiece;
    board[endX][endY] = capturedPiece;

    // Restore king's position if it was moved
    if (movingPiece->getType() == "King") {
        if (currentPlayer == 'W') {
            whiteKing = {startX, startY};
        } else {
            blackKing = {startX, startY};
        }
    }

    return !isInCheck;
}


bool Board::movePiece(int startX, int startY, int endX, int endY, char currentPlayer) {
    if (!isWithinBoard(startX, startY, endX, endY)) {
        std::cout << "Outside of grid bounds!" << std::endl; 
        return false;
    }
    if (board[startX][startY] == nullptr || board[startX][startY]->getColor() != currentPlayer) {
        std::cout << "Invalid piece selection!" << std::endl;
        return false;
    }
    if (board[endX][endY] != nullptr && board[endX][endY]->getColor() == currentPlayer) {
        std::cout << "Cannot capture own piece!" << std::endl;
        return false;
    }
    if (!board[startX][startY]->isValidPieceMove(startX, startY, endX, endY, board)) {
        std::cout << "Invalid move!" << std::endl;
        return false;
    }

    if (!isLegalMove(startX, startY, endX, endY)) {
        std::cout << "Not a legal move!" << std::endl;
        return false;
    }

    // update position of king
    if (board[startX][startY]->getType() == "King") {
        if (board[startX][startY]->getColor() == 'W') {
            whiteKing = std::make_tuple(endX, endY);
        } else {
            blackKing = std::make_tuple(endX, endY);
        }
    }

    delete board[endX][endY];
    board[endX][endY] = board[startX][startY];
    board[startX][startY] = nullptr;
    board[endX][endY]->makeMove();
    return true;
}

Piece* Board::getPieceAt(int row, int col) const {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        return board[row][col];
    }
    return nullptr;
}

char Board::getPieceColor(int row, int col) const {
    Piece* piece = getPieceAt(row, col);
    if (piece) {
        return piece->getColor();
    }
    return ' ';
}

std::vector<std::pair<int, int>> Board::getLegalMoves(int startX, int startY, char currentPlayer) {
    std::vector<std::pair<int, int>> legalMoves;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (i == startX and j == startY) {
                continue;
            }
            if (board[startX][startY]->isValidPieceMove(startX, startY, i, j, board) && isLegalMove(startX, startY, i, j)) {
                // Check legality and revert the move
                legalMoves.emplace_back(i, j);
            }
        }
    }
    return legalMoves;
}
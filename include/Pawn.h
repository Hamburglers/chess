#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

class Pawn : public Piece {
private:
    mutable bool hasMoved;
public:
    Pawn(char color);

    string getType() const override;

    bool isValidPieceMove(int startX, int startY, int endX, int endY, const vector<vector<Piece*>>& board) const override;
};

#endif
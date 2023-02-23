//
// Created by Tao G on 2/7/2023.
//

#ifndef SEKHMET_MOVE_H
#define SEKHMET_MOVE_H

#include "../core/defs.h"
#include <string>
#include <algorithm>

class Move {

public:

    Move();

    Move(unsigned int, unsigned int, PieceType, unsigned int=0);


    enum Flag {
        NULL_MOVE = 1 << 0,
        CAPTURE = 1 << 1,
        DOUBLE_PAWN_PUSH = 1 << 2,
        KSIDE_CASTLE = 1 << 3,
        QSIDE_CASTLE = 1 << 4,
        EN_PASSANT = 1 << 5,
        PROMOTION = 1 << 6,
    };

    unsigned int getFrom() const;
    unsigned int getTo() const;
    unsigned int getFlags() const;
    void setFlag(Flag);

    PieceType getPieceType() const;

    PieceType getCapturedPieceType() const;
    void setCapturedPieceType(PieceType);

    PieceType getPromotionPieceType() const;
    void setPromotionPieceType(PieceType);

    int getValue() const;
    void setValue(int);

    bool operator==(Move) const;

    std::string getNotation() const; // UCI notation

    static unsigned int notationToSquare(std::string);
    static std::string squareToNotation(int);


private:

    /**
   * @brief A packed integer containing all of the move data.
   *
   * 28 bits are used in total to store move information. The format is as
   * follows:
   *
   * @code
   * MSB  |----6----|---5---|---4---|-3--|-2--|-1--|  LSB
   *      28        21      15      9    6    3    0
   * @endcode
   *
   * - 6 - Flags (7 bits)
   * - 5 - To square (6 bits)
   * - 4 - From square (6 bits)
   * - 3 - Captured PieceType (if applicable) (3 bits)
   * - 2 - Promotion PieceType (if applicable) (3 bits)
   * - 1 - PieceType of moving piece (3 bits)
   *
   * Moves are stored as a packed integer consisting of 28 bits total.
   */
    unsigned int _move;

    int _value;

    const static char RANKS[];
    const static char FILES[];
};


#endif //SEKHMET_MOVE_H

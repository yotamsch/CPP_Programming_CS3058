
#include "PieceRPS.h"


int PieceRPS::_piece_counter = 0;

PieceRPS::PieceRPS(PieceType type, bool is_joker, int owner) { 
    _piece_type = type; 
    _is_joker = is_joker;
    _owner = owner;
    ++_piece_counter; 
  //  _owner->IncrementPieceCount(_piece_type); NEEDS REPLACEMENT!!
}

/**
 * @brief Overloading on the lower than (<) operator. According to the rules of the game. Returns true only if the left side is lower "in strength" than the right side.
 * 
 * @param p The right hand side to compare to.
 * @return true If the left side is lower "in strength" than the right side
 * @return false Otherwise
 */
bool PieceRPS::operator<(const PieceRPS& p) {
    if (_piece_type == PieceType::NONE || p._piece_type == PieceType::BOMB || (p._piece_type == PieceType::ROCK && _piece_type == PieceType::SCISSORS) || (p._piece_type == PieceType::SCISSORS && _piece_type == PieceType::PAPER) || (p._piece_type == PieceType::PAPER && _piece_type == PieceType::ROCK) || (_piece_type == PieceType::FLAG && p._piece_type != PieceType::NONE)) {
        return true;
    }
    return false;
}

PieceRPS& PieceRPS::operator=(const PieceRPS& p) {
    if (this != &p) {
        this->_is_joker = p._is_joker;
        this->_piece_type = p._piece_type;
        this->_owner = p._owner;
    }
    return *this;
}

/**
 * @brief Overloading on the print to ostream (<<) operator.
 * 
 * @param output The ostream to output into
 * @param piece The play piece to print
 * @return ostream& The modified ostream
 */
ostream& operator<<(ostream& output, const PieceRPS& piece) {
    PieceType type = piece._is_joker ? PieceType::JOKER : piece.GetPieceType();
    if (piece.GetPlayerType() == PlayerType::PLAYER_1) {
        output <<  (char)toupper(PieceTypeToChar(type));
    } else {
        output <<  (char)tolower(PieceTypeToChar(type));
    }
    return output;
}

/**
 * @brief Clears a piece content and nullifies the piece (empty piece).
 * 
 */
void PieceRPS::NullifyPiece() {
    this->_piece_type = PieceType::NONE;
    this->_owner = -1;
    this->_is_joker = false;
}

/**
 * @brief Removed the piece from the owning player.
 * 
 */
void PieceRPS::RemovePieceFromPlayer() {
    if (this->_piece_type == PieceType::NONE)
        return;
   // this->_owner->DecrementPieceCount(this->_piece_type); NEEDS REPLACEMENT!!
}

bool PieceRPS::SetType(PieceType type) { 
    if (_is_joker) {
      //  _owner->DecrementPieceCount(_piece_type); NEEDS REPLACEMENT!!
        _piece_type = type; 
    //    _owner->IncrementPieceCount(_piece_type); NEEDS REPLACEMENT!!
        return true;
    } 
    return false; 
}

// Other
const Point& PieceRPS::getPosition() const{}
char PieceRPS::getPiece() const{}
char PieceRPS::getJokerRep() const{}
//added by Tala:
void PieceRPS::setPosition(const Point& rPoint){}


/**
 * @brief Converts the recieved type to a representing char
 * 
 * @param type The type to convert (e.g. ROCK, PAPER...)
 * @return char The representing character
 */
char PieceTypeToChar(PieceType type) {
    switch(type) {
        case PieceType::NONE:
            return ' ';
        case PieceType::ROCK:
            return 'R';
        case PieceType::PAPER:
            return 'P';
        case PieceType::SCISSORS:
            return 'S';
        case PieceType::FLAG:
            return 'F';
        case PieceType::BOMB:
            return 'B';
        case PieceType::JOKER:
            return 'J';
        default:
            return '?';
    }
}

PieceType CharToPieceType(char chr) {
    switch(chr) {
        case 'R':
            return PieceType::ROCK;
        case 'P':
            return PieceType::PAPER;
        case 'S':
            return PieceType::SCISSORS;
        case 'F':
            return PieceType::FLAG;
        case 'B':
            return PieceType::BOMB;
        case 'J':
            return PieceType::JOKER;
        default:
            return PieceType::NONE;
    }
}

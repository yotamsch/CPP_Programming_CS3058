/**
 * @brief The implementation file of the AutoPlayerAlgorithm (AI) class.
 * 
 * @file AutoPlayerAlgorithm.cpp
 * @author Yotam Sechayk
 * @date 2018-05-10
 */
#include "AutoPlayerAlgorithm.h"
#include "GameUtilitiesRPS.h"
#include "JokerChangeRPS.h"
#include "MoveRPS.h"
#include "PieceRPS.h"
#include "PointRPS.h"

#include <algorithm>
#include <random>

// %% INFO %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Adds a specified piece to the board and updates the positions of this type of piece for the corresponding player
 * 
 * @param p - the specified piece
 * @param position - the position on board of the specified piece
 */
void AutoPlayerAlgorithm::info::addPiece(AutoPlayerAlgorithm::piece p, int position)
{
    if (p._M_player == NO_PLAYER) {
        return;
    }

    if (p._M_player == this->_M_this_player._M_id) {
        if (p._M_piece == FLAG_CHR || p._M_piece == UNKNOWN_CHR)
            this->_M_this_player._M_flags.insert(position);
        else if (p._M_isJoker)
            this->_M_this_player._M_jokers.insert(position);
        this->_M_this_player._M_pieces.insert(position);
    } else {
        if (p._M_piece == FLAG_CHR || p._M_piece == UNKNOWN_CHR)
            this->_M_other_player._M_flags.insert(position);
        else if (p._M_isJoker)
            this->_M_other_player._M_jokers.insert(position);
        this->_M_other_player._M_pieces.insert(position);
    }
    this->_M_board[position] = p;
}

/**
 * @brief Given a position on board, removes the piece that is currently at this position and updates the positions of this type of piece for the corresponding player
 * 
 * @param position - the position where the piece-to-be-deleted is
 */
void AutoPlayerAlgorithm::info::removePiece(int position)
{
    auto& p = this->_M_board[position];
    if (p._M_player == NO_PLAYER)
        return;

    if (p._M_player == this->_M_this_player._M_id) {
        this->_M_this_player._M_flags.erase(position);
        this->_M_this_player._M_jokers.erase(position);
        this->_M_this_player._M_pieces.erase(position);
    } else {
        this->_M_other_player._M_flags.erase(position);
        this->_M_other_player._M_jokers.erase(position);
        this->_M_other_player._M_pieces.erase(position);
    }
    emptyPiece(position);
}

/**
 * @brief Removes the piece from the "thought to be" flags of the corresponding player
 * 
 * @param position - where the flag is currently
 */
void AutoPlayerAlgorithm::info::removeFlag(int position)
{
    auto& p = this->_M_board[position];
    if (p._M_player == NO_PLAYER)
        return;

    if (p._M_player == this->_M_this_player._M_id) {
        this->_M_this_player._M_flags.erase(position);
    } else {
        this->_M_other_player._M_flags.erase(position);
    }
}

/**
 * @brief Given a position on board, resets the piece at the position. After invoking the function, the board at the specified position will be neutralized piece-wise
 * 
 * @param position - the specified position which we want to be empty of pieces
 */
void AutoPlayerAlgorithm::info::emptyPiece(int position)
{
    this->_M_board[position]._M_player = 0;
    this->_M_board[position]._M_isJoker = false;
    this->_M_board[position]._M_piece = '\0';
}

/**
 * @brief Given a positon, sets the piece at this position to be a joker.
 * 
 * @param position - the specified position
 */
void AutoPlayerAlgorithm::info::updateJoker(int position)
{
    this->_M_board[position]._M_isJoker = true;
}

/**
 * @brief Swaps the positions of two pieces on the board
 * 
 * @param pos1 - the piece at pos1 will be at pos2 at end of function
 * @param pos2 - the piece at pos2 will be at pos1 at end of function
 */
void AutoPlayerAlgorithm::info::swapPieces(int pos1, int pos2)
{
    auto p1 = this->_M_board[pos1];
    auto p2 = this->_M_board[pos2];

    removePiece(pos1);
    removePiece(pos2);

    addPiece(p1, pos2);
    addPiece(p2, pos1);

    removeFlag(pos1);
    removeFlag(pos2);
}

/**
 * @brief updates the struct "info" with a new move from->to. Adds a new move to the move history.
 * 
 * @param from - starting position
 * @param to - destination position
 */
void AutoPlayerAlgorithm::info::addMove(int from, int to)
{
    this->_M_moves.push_back({ from, to });
}

/**
 * @brief - Gets the last committed move in the game. The function assumes there exists a move in the vector.
 * 
 * @return const AutoPlayerAlgorithm::move& - reference to the last move committed
 */
const AutoPlayerAlgorithm::move& AutoPlayerAlgorithm::info::peekMove() const
{
    return this->_M_moves.back();
}

// %% GENERAL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief The function calculates a random position on board and returns that position as an int
 * 
 * @return int - the random position on board
 */
/*static*/ int AutoPlayerAlgorithm::getRandomPos()
{
    const int range_from = 0;
    const int range_to = DIM_X * DIM_Y;
    return rand() % (range_to - range_from) + range_from;
}

/**
 * @brief Calculates the X dimension of a combined position.
 * 
 * @param vPos - the combined position
 * @return int - the calculated X dimension
 */
/*static*/ int AutoPlayerAlgorithm::getXDim(int vPos)
{
    return vPos % DIM_X;
}

/**
 * @brief Calculates the Y dimension of a combined position.
 * 
 * @param vPos - the combined position
 * @return int - the calculated Y dimension
 */
/*static*/ int AutoPlayerAlgorithm::getYDim(int vPos)
{
    return vPos / DIM_X;
}

/**
 * @brief Gets the combined positioning from the X and Y dimensions
 * 
 * @param vX - the X dimension parameter
 * @param vY - the Y dimension parameter
 * @return int - the combined positioning
 */
/*static*/ int AutoPlayerAlgorithm::getPos(int vX, int vY)
{
    return vY * DIM_X + vX;
}

/**
 * @brief Returns a random Joker representation from the possible ones
 * 
 * @return char - a random Joker representation
 */
/*static*/ char AutoPlayerAlgorithm::getRandomJokerRep()
{
    char vTypes[] = { BOMB_CHR, ROCK_CHR, PAPER_CHR, SCISSORS_CHR };
    int choose = rand() % 4;
    return vTypes[choose];
}

/**
 * @brief Checks if position (x,y) is valid
 * 
 * @param x - the X dimension
 * @param y - the Y dimension
 * @return true - iff the position is valid
 * @return false - otherwise
 */
bool AutoPlayerAlgorithm::isPosValid(int x, int y)
{
    bool result = true;

    if (x >= DIM_X || x < 0 || y >= DIM_Y || y < 0) {
        result = false;
    }
    return result;
}

/**
 * @brief - Checks if a position (x,y) and (n_x,n_y) are valid, and a move from (x,y) to (n_x,n_y) is valid
 * 
 * @param x - the old x coord
 * @param y - the old y coord
 * @param n_x - the new x coord
 * @param n_y - the new y coord
 * @return true - if the range of the arguments is within the board and also the move from (x,y) to (n_x,n_y) is valid according to the game rules: one step only either horizontally or vertically
 * @return false - otherwise
 */
bool AutoPlayerAlgorithm::isPosValid(int x, int y, int n_x, int n_y)
{
    bool result = true;

    if (!isPosValid(x,y) || !isPosValid(n_x,n_y)) {
        result = false;
    }
    if (std::abs(n_x - x) > 1 || std::abs(n_y - y) > 1) {
        result = false;
    }
    if (std::abs(n_x - x) == 1 && std::abs(n_y - y) == 1) {
        result = false;
    }
    return result;
}

// %% POSITION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Gets a random position on board that hasn't been chosen yet
 * 
 * @return int - random position 
 */
int AutoPlayerAlgorithm::getPositionNotSelectedYet() const
{
    int vRandPosition = 0;
    do {
        vRandPosition = getRandomPos();
    } while (_info._M_this_player._M_pieces.count(vRandPosition) != 0);
    return vRandPosition;
}

/**
 * @brief - finds a random not-taken-position on the board to place a piece of type vType and actually places it there
 * 
 * @param vLimit - maximal number of this type of piece that each player shall have according to the game rules
 * @param vType - type of the piece that needs to be positioned
 * @param vectorToFill - a vector that is filled inside the function with PiecePositions according to the random positioning of the pieces
 */
void AutoPlayerAlgorithm::positionPiecesOfType(int vLimit,
    char vType, std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
    int i;
    int vRandPosition;
    bool vIsJoker = vType == JOKER_CHR;
    std::unique_ptr<PiecePosition> pPiece;

    for (i = 0; i < vLimit; ++i) {
        vRandPosition = getPositionNotSelectedYet();
        vType = vIsJoker ? getRandomJokerRep() : vType;
        pPiece = std::make_unique<PieceRPS>(this->_info._M_this_player._M_id, vIsJoker, vType, PointRPS(getXDim(vRandPosition), getYDim(vRandPosition)));
        vectorToFill.push_back(std::move(pPiece));
        this->_info.addPiece({ this->_info._M_this_player._M_id, vIsJoker, vType }, vRandPosition);
    }
}

// %% MOVE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Gets the number of moving pieces (R,P,S) of the player player. 
 * 
 * @param data - reference to the struct "info", used to access the board
 * @param player - the player we require about
 * @return int - number of moving piece the player player has
 */
int AutoPlayerAlgorithm::getNumOfMovingPieces(AutoPlayerAlgorithm::info& data, AutoPlayerAlgorithm::player_info& player) const
{
    int counter = 0;
    for (auto p : player._M_pieces) {

        switch (data._M_board[p]._M_piece) {
        case FLAG_CHR:
        case BOMB_CHR:
            break;
        default:
            ++counter;
            break;
        }
    }
    return counter;
}

/**
 * @brief Check if a move from vOriginPos to vDestPos is allowed(valid according to the game rules) and also if the move is not "definitely losing" i.e. the player's piece is attacking a stronger opponent piece
 * 
 * 
 * @param data - reference to struct info, used to access the board
 * @param vOriginPos - source position
 * @param vDestPos - destination position
 * @return true - if the move is valid and also causes the current player to NOT lose this fight
 * @return false - otherwise
 */
bool AutoPlayerAlgorithm::isMovePossible(AutoPlayerAlgorithm::info& data, int x, int y, int n_x, int n_y)
{
    int vOriginPos, vDestPos;

    if (!isPosValid(x, y, n_x, n_y)) {
        return false;
    }

    vOriginPos = getPos(x,y);
    vDestPos = getPos(n_x, n_y);

    if (data._M_board[vOriginPos]._M_player == data._M_board[vDestPos]._M_player || data._M_board[vOriginPos]._M_piece == FLAG_CHR || data._M_board[vOriginPos]._M_piece == BOMB_CHR) {
        return false;
    }
    return willWinFight(data, vOriginPos, vDestPos);
}

/**
 * @brief For a specified position, checks what possible moves the piece at positon may have and updates these possible moves in the given vector rMoves
 * 
 * @param data - reference to the struct info, used to access the board
 * @param vPos - the specified position
 * @param rMoves - a vector to be filled by the function with all the allowed moves for the piece at vPos
 */
void AutoPlayerAlgorithm::getPossibleMovesForPiece(AutoPlayerAlgorithm::info& data, int vPos, std::vector<int>& rMoves)
{
    int x = getXDim(vPos);
    int y = getYDim(vPos);

    // just in case
    if (data._M_board[vPos]._M_player != data._M_this_player._M_id) {
        return;
    }

    for (int ix = x - 1; ix <= x + 1; ++ix) {
        for (int iy = y - 1; iy <= y + 1; ++iy) {
            if (isMovePossible(data, x, y, ix, iy)) {
                rMoves.push_back(getPos(ix, iy));
            }
        }
    }
}

/**
 * @brief Checks if the piece located at vPos is threatening an opponent piece
 * 
 * @param data - reference to the stuct info, used to access board infromation
 * @param vPos - the specified position
 * @return true -if the piece at vPos can eat an opponent piece in one step
 * @return false -otherwise
 */
bool AutoPlayerAlgorithm::isPieceThreatening(AutoPlayerAlgorithm::info& data, int vPos)
{
    int x, y;
    bool isThreat = false;

    x = getXDim(vPos);
    y = getYDim(vPos);

    for (int ix = x - 1; ix <= x + 1; ++ix) {
        for (int iy = y - 1; iy <= y + 1; ++iy) {
            if (isPosValid(x, y, ix, iy) && data._M_board[getPos(x, y)]._M_player != NO_PLAYER && data._M_board[getPos(ix, iy)]._M_player != NO_PLAYER && data._M_board[getPos(x, y)]._M_player != data._M_board[getPos(ix, iy)]._M_player) {
                if (data._M_other_player._M_flags.count(getPos(ix, iy)) <= 0)
                    isThreat |= willWinFight(data, getPos(x, y), getPos(ix, iy));
                else
                    // if opp piece might be a flag, attempt "eat" it
                    return true;
            }
        }
    }
    return isThreat;
}

/**
 * @brief  Checks if the piece located at vPos is in danger by opponent
 * 
 * @param data - reference to the stuct info, used to access board infromation
 * @param vPos - the specified position
 * @return true - if the piece at vPos can be reached and eaten by an opponent piece in one step
 * @return false - otherwise
 */
bool AutoPlayerAlgorithm::isPieceInDanger(AutoPlayerAlgorithm::info& data, int vPos)
{
    int x, y;
    bool isDanger = false;

    x = getXDim(vPos);
    y = getYDim(vPos);

    for (int ix = x - 1; ix <= x + 1; ++ix) {
        for (int iy = y - 1; iy <= y + 1; ++iy) {
            if (isPosValid(x, y, ix, iy) && data._M_board[getPos(x, y)]._M_player != NO_PLAYER && data._M_board[getPos(x, y)]._M_player != data._M_board[getPos(ix, iy)]._M_player) {
                // if opp piece might be a flag, doen't danger this piece
                if (data._M_other_player._M_flags.count(getPos(ix, iy)) <= 0)
                    isDanger |= !willWinFight(data, getPos(x, y), getPos(ix, iy));
            }
        }
    }
    return isDanger;
}

/**
 * @brief Checks if moving from vOrigPos to vDestPos will result in winning the fight 
 * 
 * @param data - reference to struct info, used to access the board
 * @param vOriginPos - source position 
 * @param vDestPos - destination position 
 * @return true - if the move will result in winning the fight certainly or in a hight chance
 * @return false - otherwise
 */
bool AutoPlayerAlgorithm::willWinFight(AutoPlayerAlgorithm::info& data, int vOriginPos, int vDestPos)
{
    // NOTE: assumes that origin and dest are on the board and correct pieces of two different players
    AutoPlayerAlgorithm::piece& origPiece = data._M_board[vOriginPos];
    AutoPlayerAlgorithm::piece& destPiece = data._M_board[vDestPos];
    double chance = (double)std::rand() / (RAND_MAX);

    // checks normal rules and doesn't take chances
    if ((destPiece._M_piece == origPiece._M_piece) || (destPiece._M_piece == ROCK_CHR && origPiece._M_piece == SCISSORS_CHR) || (destPiece._M_piece == SCISSORS_CHR && origPiece._M_piece == PAPER_CHR) || (destPiece._M_piece == PAPER_CHR && origPiece._M_piece == ROCK_CHR) || (destPiece._M_piece == BOMB_CHR && origPiece._M_piece != BOMB_CHR) || (destPiece._M_piece != FLAG_CHR && origPiece._M_piece == FLAG_CHR)) {
        return false;
    }
    // if piece/s unknown, win with a chance of UNKNOWN_WIN_CHANCE
    if ((destPiece._M_piece == UNKNOWN_CHR || origPiece._M_piece == UNKNOWN_CHR) && chance >= UNKNOWN_WIN_CHANCE) {
        return false;
    }
    return true;
}

/**
 * @brief Caluculates the average distance of the k closest vFromPlayer pieces to vPos 
 * 
 * @param data - reference to struct info, used to access the board 
 * @param vFromPlayer - the "opponent" player
 * @param vPos - the position of a current player's pieces
 * @param k - the number of nearest pieces needed to get average
 * @return float - average of the distances of the k nearest opponent pieces to vPos
 */
float AutoPlayerAlgorithm::calcKNearestDistance(AutoPlayerAlgorithm::info& data, int vFromPlayer, int vPos, int k)
{
    std::vector<float> dist;
    float res = 0.0f;
    int orig_x, orig_y;

    orig_x = getXDim(vPos);
    orig_y = getYDim(vPos);

    for (int pos = 0; pos < DIM_X * DIM_Y; ++pos) {
        if (pos != vPos && data._M_board[pos]._M_player == vFromPlayer && data._M_board[pos]._M_piece != BOMB_CHR && data._M_board[pos]._M_piece != FLAG_CHR) {
            dist.push_back(std::abs(getXDim(pos) - orig_x) + std::abs(getYDim(pos) - orig_y));
        }
    }
    std::sort(dist.begin(), dist.end());
    int i;
    for (i = 0; i < k && i < dist.size(); ++i)
        res += dist[i];

    return res / i;
}

/**
 * @brief - The function calculates the "score" for the current player of a specific representation of the board. It takes into account how many moving pieces this player has compared to the opponent, how many pieces are in danger compared to threatening etc.
 * 
 * @param data - reference to the struct info, used to extract inforation about the board
 * @return float - the score of the board for the player, i.e. meaning how good the current  board status is for the player
 */
float AutoPlayerAlgorithm::calcPlayerBoardScore(AutoPlayerAlgorithm::info& data)
{
    const int ALL_PIECES_EATEN = 10;
    const int ALL_FLAGS_EATEN = 15;
    const int NUM_OF_PIECES = getNumOfMovingPieces(data, data._M_this_player);

    const int K_PROXIMITY = 0.66f * NUM_OF_PIECES;
    const float PIECES_PARAM = 9.0f;
    const float ENEMY_FLAG_EXIST_PARAM = -2.5f;
    const float DANGER_PARAM = -4.0f / NUM_OF_PIECES;
    const float THREAT_PARAM = 3.0f / NUM_OF_PIECES;
    const float OPP_FLAG_DIST_PARAM = -9.0f / (float)data._M_other_player._M_flags.size();

    float score = 0.0f; // lower is worse heigher is better
    float avg = 0.0f;
    int player = data._M_this_player._M_id;
    int opp = data._M_other_player._M_id;
    int counter;
    int flag_amount;
    std::vector<int> flagSample;

    // number of pieces in danger
    // average L2 ditance between THIS flag to opponent pieces
    for (auto pos : data._M_this_player._M_pieces) {
        switch (data._M_board[pos]._M_piece) {
        case BOMB_CHR:
        case FLAG_CHR:
            break;
        default:
            if (isPieceInDanger(data, pos))
                score += DANGER_PARAM;
            if (isPieceThreatening(data, pos))
                score += THREAT_PARAM;
            break;
        }
    }
    // average L2 distance oponent flag to this player's pieces

    avg = 0.0f;
    counter = 0;
    flag_amount = std::max((int)((data._M_other_player._M_pieces.size() + 1) / 2), FLAG_LIMIT);

    // advances the pieces towards the "flags". if too many flags, picks a few of them
    if (data._M_other_player._M_flags.size() <= flag_amount) {
        for (auto pos : data._M_other_player._M_flags) {
            avg += OPP_FLAG_DIST_PARAM * calcKNearestDistance(data, player, pos, K_PROXIMITY);
        }
        if (avg != 0)
            avg /= (float)data._M_other_player._M_flags.size();
    } else {
        for (auto itr = data._M_other_player._M_flags.begin(); counter < flag_amount;) {
            std::advance(itr, std::rand() % data._M_other_player._M_flags.size());
            if (itr == data._M_other_player._M_flags.end())
                itr = data._M_other_player._M_flags.begin();
            avg += OPP_FLAG_DIST_PARAM * calcKNearestDistance(data, player, *itr, K_PROXIMITY);
            ++counter;
        }
        if (avg != 0)
            avg /= counter;
    }

    score += avg;

    // more existing player pieces is good, encourage "eating"
    score += PIECES_PARAM * (getNumOfMovingPieces(data, data._M_this_player) - getNumOfMovingPieces(data, data._M_other_player));
    // attempt to eat the flags
    score += ENEMY_FLAG_EXIST_PARAM * (float)data._M_other_player._M_flags.size();

    if (data._M_other_player._M_pieces.size() <= 0)
        score += ALL_PIECES_EATEN;
    if (data._M_other_player._M_flags.size() <= 0)
        score += ALL_FLAGS_EATEN;

    return score;
}

/**
 * @brief Performs a given move on the current board in 'info'
 * 
 * @param data - reference to the struct info, used to access board
 * @param vMove - a copy of the struct move
 */
void AutoPlayerAlgorithm::performMoveOnBoard(AutoPlayerAlgorithm::info& data, AutoPlayerAlgorithm::move& vMove)
{
    data.swapPieces(vMove._M_from, vMove._M_to);
    data.removePiece(vMove._M_from);
}

/**
 * @brief Gets a "score" for a potential move on the board, as in "how good this move will be for the player"
 * 
 * @param data - a reference to the struct info, used to perform a hypothetical move on it 
 * @param vMove - hypothetical move of type copy of the struct move
 * @return float - the calculated score
 */
float AutoPlayerAlgorithm::getScoreForMove(AutoPlayerAlgorithm::info& data, AutoPlayerAlgorithm::move& vMove)
{
    AutoPlayerAlgorithm::info data_cpy(data);

    // just in case
    if (data._M_board[vMove._M_from]._M_player == NO_PLAYER) {
        return 0.0f;
    }

    performMoveOnBoard(data_cpy, vMove);
    return calcPlayerBoardScore(data_cpy);
}

/**
 * @brief Gets a "score" for a potential joker-change on the board,
 *  as in "how good this joker-change will be for the player"
 * 
 * @param data - a copy of the struct info, used to perform a hypothetical move on it 
 * @param vChange - the potential joker-change
 * @return float - the calculated score
 */
float AutoPlayerAlgorithm::getScoreForJokerChange(AutoPlayerAlgorithm::info data, AutoPlayerAlgorithm::joker_change vChange)
{
    // just in case
    if (data._M_board[vChange._M_position]._M_player == NO_PLAYER || !data._M_board[vChange._M_position]._M_isJoker) {
        return 0.0f;
    }

    data._M_board[vChange._M_position]._M_piece = vChange._M_new_rep;
    return calcPlayerBoardScore(data);
}

/**
 * @brief Gets the best move possible out of all available moves for this player
 * 
 * @param data - a reference of the struct info
 * @return AutoPlayerAlgorithm::move - the move that has the highest score among all other valid moves
 */
AutoPlayerAlgorithm::move AutoPlayerAlgorithm::getBestMoveForPlayer(AutoPlayerAlgorithm::info& data)
{
    AutoPlayerAlgorithm::move currMove, maxMove;
    std::vector<int> possibleMoves;
    float currScore = 0;
    float maxScore = calcPlayerBoardScore(data);

    // get the best move possible out of all available moves
    for (auto pos = data._M_this_player._M_pieces.begin(); pos != data._M_this_player._M_pieces.end(); ++pos) {
        getPossibleMovesForPiece(data, *pos, possibleMoves);
        for (auto mov = possibleMoves.begin(); mov != possibleMoves.end(); ++mov) {
            currMove = { *pos, *mov };
            // make sure doesn't go back and forth
            if (data._M_moves.size() > 0 && data.peekMove()._M_from == *mov && data.peekMove()._M_to == *pos)
                continue;
            // will use a copy of the data
            currScore = getScoreForMove(data, currMove);
            if ((maxMove._M_from == -1 && maxMove._M_to == -1 && currScore >= maxScore) || (currScore > maxScore)) {
                maxMove = currMove;
                maxScore = currScore;
            }
        }
        possibleMoves.clear();
    }
    return maxMove;
}

/**
 * @brief Gets the best joker-change possible out of all available joker-changes for this player
 * 
 * @param data - a reference of the struct info
 * @return AutoPlayerAlgorithm::joker_change - the joker-change that has the highest score among all other valid joker-changes
 */
AutoPlayerAlgorithm::joker_change AutoPlayerAlgorithm::getBestJokerChangeForPlayer(AutoPlayerAlgorithm::info& data)
{
    AutoPlayerAlgorithm::joker_change currChange, bestChange;
    float currScore = 0;
    float maxScore = calcPlayerBoardScore(data);
    std::array<char, 4> possibleChanges = { ROCK_CHR, PAPER_CHR, SCISSORS_CHR, BOMB_CHR };

    // for all the current jokers, go over all the possible joker rep changes
    for (auto pos : data._M_this_player._M_jokers) {
        for (auto change : possibleChanges) {
            currChange = { pos, change };
            currScore = getScoreForJokerChange(data, currChange);
            if (currScore >= maxScore) {
                bestChange = currChange;
                maxScore = currScore;
            }
        }
    }
    return bestChange;
}

// %% INTERFACE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Fills vectorToFill with the initial board positions of this player.
 * Implemented with Smart Random method, which spreads the pices randomly.
 * It will not place two pieces in the same spot.
 * 
 * @param player - int representation of current player  
 * @param vectorToFill - the vector sepcified above to be filled
 */
void AutoPlayerAlgorithm::getInitialPositions(int player,
    std::vector<unique_ptr<PiecePosition>>& vectorToFill)
{
    // set the player number
    this->_info._M_this_player._M_id = player;

    // set the seed for the randomization
    this->_seed_value = (unsigned)time(NULL);
    srand(this->_seed_value + player * PRIME_NUMBER);

    // insert flags
    positionPiecesOfType(FLAG_LIMIT, FLAG_CHR, vectorToFill);
    // insert bombs
    positionPiecesOfType(BOMB_LIMIT, BOMB_CHR, vectorToFill);
    // insert and choose joker
    positionPiecesOfType(JOKER_LIMIT, JOKER_CHR, vectorToFill);
    // insert rock
    positionPiecesOfType(ROCK_LIMIT, ROCK_CHR, vectorToFill);
    // insert paper
    positionPiecesOfType(PAPER_LIMIT, PAPER_CHR, vectorToFill);
    // insert scissors
    positionPiecesOfType(SCISSORS_LIMIT, SCISSORS_CHR, vectorToFill);
}

/**
 * @brief Notifies the AutoPlayerAlgorithm about the initial board after placing both players' pieces and also notifies about all the fights that took place in the initial positioning.
 * The function uses the parameter b (Board) to extract information and store it in the suitable data structures
 * 
 * @param b - a reference to the Board 
 * @param fights - a vector of unique pointers to be filled by the function with all the initial fights
 */
void AutoPlayerAlgorithm::notifyOnInitialBoard(const Board& b,
    const std::vector<unique_ptr<FightInfo>>& fights)
{
    // asume i know the board dimentions
    // go over the board and mark pieces in internal board
    int i, player, pos;

    // go over fights and place/update pieces
    for (auto&& fight : fights) {
        pos = getPos(fight->getPosition().getX(), fight->getPosition().getY());
        // tie or other player won
        if (fight->getWinner() != this->_info._M_this_player._M_id) {
            this->_info.removePiece(pos);
            // another player won
            if (fight->getWinner() != NO_PLAYER) {
                this->_info.addPiece({ fight->getWinner(), false, fight->getPiece(fight->getWinner()) }, pos);
            }
        }
    }
    // mark the rest of the board if a known piece exists
    for (i = 0; i < DIM_X * DIM_Y; ++i) {
        player = b.getPlayer(PointRPS(getXDim(i), getYDim(i)));
        if (player != NO_PLAYER && player != this->_info._M_this_player._M_id) {
            if (this->_info._M_other_player._M_id != player)
                this->_info._M_other_player._M_id = player;
            if (this->_info._M_board[i]._M_player == NO_PLAYER)
                this->_info.addPiece({ player, false, UNKNOWN_CHR }, i);
        }
    }
}

/**
 * @brief Notifies the AutoPlayerAlgorithm about an opponent move
 * 
 * @param move - a reference to the opponent move
 */
void AutoPlayerAlgorithm::notifyOnOpponentMove(const Move& move)
{
    // save the move into history vector
    this->_info.addMove(getPos(move.getFrom().getX(), move.getFrom().getY()), getPos(move.getTo().getX(), move.getTo().getY()));
    // now _M_moves is sure to have at least one move (i.e. size() > 0)

    this->_info.updateJoker(this->_info.peekMove()._M_from);
    this->_info.removeFlag(this->_info.peekMove()._M_to);
    this->_info.removeFlag(this->_info.peekMove()._M_from);

    if (this->_info._M_board[this->_info.peekMove()._M_to]._M_player == NO_PLAYER) {
        // clean move, no fight
        this->_info.swapPieces(this->_info.peekMove()._M_from, this->_info.peekMove()._M_to);
    }
}

/**
 * @brief Notifies the AutoPlayerAlgorithm about the latest fight
 * 
 * @param fightInfo - a reference to FightInfo, holds the information about : winner, where the fight happened, and what pieces particicpatd in it
 */
void AutoPlayerAlgorithm::notifyFightResult(const FightInfo& fightInfo)
{
    int fightPos = getPos(fightInfo.getPosition().getX(), fightInfo.getPosition().getY());

    // since there was a fight, _M_moves is sure to have at least one move (i.e. size() > 0)

    if (fightPos != this->_info.peekMove()._M_to) {
        // safety check
        return;
    }

    if (fightInfo.getWinner() == NO_PLAYER) {
        // both lost
        this->_info.removePiece(this->_info.peekMove()._M_from);
        this->_info.removePiece(this->_info.peekMove()._M_to);
        return;
    }

    if (fightInfo.getWinner() == this->_info._M_board[this->_info.peekMove()._M_to]._M_player) {
        this->_info.removePiece(this->_info.peekMove()._M_from);
    } else {
        this->_info.swapPieces(this->_info.peekMove()._M_from, this->_info.peekMove()._M_to);
        this->_info.removePiece(this->_info.peekMove()._M_from);
    }

    if (fightInfo.getWinner() != this->_info._M_this_player._M_id) {
        this->_info._M_board[this->_info.peekMove()._M_to]._M_piece = fightInfo.getPiece(fightInfo.getWinner());
    }
}

/**
 * @brief Calculates the smartest move that the current player can make in this board state and returns a pointer to this move
 * 
 * @return unique_ptr<Move> - a pointer to the best move that the auto player would want to make next
 */
unique_ptr<Move> AutoPlayerAlgorithm::getMove()
{
    std::unique_ptr<Move> retMove;
    AutoPlayerAlgorithm::move bestMove;

    bestMove = getBestMoveForPlayer(this->_info);
    if (bestMove._M_from == -1 && bestMove._M_to == -1) {
        return nullptr;
    }

    retMove = std::make_unique<MoveRPS>(PointRPS(getXDim(bestMove._M_from), getYDim(bestMove._M_from)), PointRPS(getXDim(bestMove._M_to), getYDim(bestMove._M_to)));

    // add to history
    this->_info.addMove(bestMove._M_from, bestMove._M_to);
    // now _M_moves is sure to have at least one move (i.e. size() > 0)

    // perform the move if it's a clean move
    if (this->_info._M_board[bestMove._M_to]._M_player == NO_PLAYER) {
        this->_info.swapPieces(this->_info.peekMove()._M_from, this->_info.peekMove()._M_to);
    }

    // if only one piece "can" be flag, mark it as flag
    if (this->_info._M_other_player._M_flags.size() <= FLAG_LIMIT) {
        for (auto pos : this->_info._M_other_player._M_flags) {
            this->_info._M_board[pos]._M_piece = FLAG_CHR;
        }
    }

    return std::move(retMove);
}

/**
 * @brief Calculates the smartest joker-change that the current player can make in this board state and returns a pointer to this joker-change

 * 
 * @return unique_ptr<JokerChange> - a pointer to the best joker-change that the auto player would want to make next
 */
unique_ptr<JokerChange> AutoPlayerAlgorithm::getJokerChange()
{
    std::unique_ptr<JokerChange> retJokerChange;
    AutoPlayerAlgorithm::joker_change bestJokerChange;

    bestJokerChange = getBestJokerChangeForPlayer(this->_info);
    if (bestJokerChange._M_position == -1 || this->_info._M_board[bestJokerChange._M_position]._M_piece == bestJokerChange._M_new_rep) {
        return nullptr;
    }

    retJokerChange = std::make_unique<JokerChangeRPS>(PointRPS(getXDim(bestJokerChange._M_position), getYDim(bestJokerChange._M_position)), bestJokerChange._M_new_rep);

    // perform joker change
    this->_info._M_board[bestJokerChange._M_position]._M_piece = bestJokerChange._M_new_rep;

    return std::move(retJokerChange);
}

// %% DEBUG ORIENTED %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * @brief Prints the current AutoPlayer object state nicely. Used mainly for DEBUG. Was left to give a tool to view the state of the object simply.
 * 
 */
void AutoPlayerAlgorithm::prettyPrint()
{
    std::cout << "player: " << this->_info._M_this_player._M_id << std::endl;
    std::cout << "seed base: " << this->_seed_value << std::endl;
    std::cout << "flags at: ";
    for (int pos : this->_info._M_this_player._M_flags)
        std::cout << "(" << getXDim(pos) << "," << getYDim(pos) << "), ";
    std::cout << std::endl;
    std::cout << "pieces at: ";
    for (int pos : this->_info._M_this_player._M_pieces)
        std::cout << "(" << getXDim(pos) << "," << getYDim(pos) << "), ";
    std::cout << std::endl;
    std::cout << "enemy flags (maybe): ";
    for (int pos : this->_info._M_other_player._M_flags)
        std::cout << "(" << getXDim(pos) << "," << getYDim(pos) << "), ";
    std::cout << std::endl;
    std::cout << "board score: " << calcPlayerBoardScore(this->_info) << std::endl;
    prettyPrintBoard();
}

/**
 * @brief Prints the current AutoPlayer's "board" object  nicely. Used mainly for DEBUG. Was left to give a tool to view the state of "board" simply.
 * 
 */
void AutoPlayerAlgorithm::prettyPrintBoard()
{
    std::cout << "board:" << std::endl;
    int idx = 0, i = 0;
    std::cout << "  ";
    for (i = 0; i < DIM_X; ++i)
        std::cout << " " << i << " ";
    i = 0;
    for (auto piece : this->_info._M_board) {
        if (idx % DIM_X == 0)
            std::cout << std::endl
                      << i++ << " ";
        if (piece._M_player == NO_PLAYER)
            std::cout << "[ ]";
        else if (piece._M_player == this->_info._M_this_player._M_id)
            std::cout << "["
                      << "\033[0;32m" << piece._M_piece << "\033[0m"
                      << "]";
        else
            std::cout << "["
                      << "\033[1;34m" << piece._M_piece << "\033[0m"
                      << "]";
        ++idx;
    }
    std::cout << std::endl;
}

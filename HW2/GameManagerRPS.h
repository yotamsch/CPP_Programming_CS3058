
#ifndef __H_GAME_MANAGER_RPS
#define __H_GAME_MANAGER_RPS

#include <iostream>
#include <string>
#include <vector>
#define AUTO_VS_AUTO 1
#define FILE_VS_FILE 2
#define AUTO_VS_FILE 3
#define FILE_VS_AUTO 4

#define INFO "[INFO]"
#define ERROR "[ERROR]"
#define DIM_X 10
#define DIM_Y 10


using namespace std;

/**
 * @brief Inline function. Prints a message to the screen with the desired label type.
 * 
 * @param type A string representing the type (i.e. INFO, ERROR, ...).
 * @param msg The message to be printed alongside the label.
 */
//void printMessageToScreen(const std::string&& rType, const std::string&& rMsg) { std::cout << rType << " " << rMsg << std::endl; }

/**
 * @brief The full play of the RPS game.
 * 
 * @param vGameStyle An integer {1: auto-vs-auto, 2: file-vs-file, 3: auto-vs-file, 4: file-vs-auto}
 * @return int In case of success - 0, otherwise - 1
 */
int PlayRPS(int vGameStyle);



/**
 * @brief An enum representing the player type (player 1 or player 2)
 * 
 */
enum class PlayerType {NONE=-1, PLAYER_1=1, PLAYER_2};

/**
 * @brief An enum representing the different types of play pieces
 * 
 */
enum class PieceType {NONE=-1, ROCK=0, PAPER, SCISSORS, FLAG, BOMB, JOKER};
#endif // !__H_GAME_MANAGER_RPS
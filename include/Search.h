#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include <cstdint>
#include "HexBoard.h"
#include "TranspositionTable.h"
#include "MoveGeneration.h"

int16_t alpha_beta_search(uint8_t depth, int16_t alpha, int16_t beta, uint8_t actual_killer_move_index, KillerMovesSlot *killer_moves);

extern uint8_t root_depth;
extern uint16_t PV_move;

#endif // SEARCH_H_INCLUDED

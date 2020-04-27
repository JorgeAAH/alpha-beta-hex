#include <iostream>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <chrono>
#include "HexBoard.h"
#include "Search.h"
#include "TranspositionTable.h"
#include "MoveGeneration.h"

using namespace std;

int main() {
    KillerMovesSlot killer_moves[8];
    engine_transposition_table.initialize_transposition_table(25);
    engine_transposition_table.new_search();
    //To test 29/03/2020.
    number_of_transposition_table_lookups = 0;
    number_of_transposition_table_hits = 0;
    number_of_transposition_table_stores = 0;
    //End test 29/03/2020.
    for (uint8_t depth = 1; depth < 5; depth++){
        root_depth = depth;
        std::cout << "Value: " << -alpha_beta_search(depth, -3000, 3000, 0, killer_moves) << "    ";
        std::cout << "Move: " << PV_move << std::endl;
    }
    engine_transposition_table.release_memory();
    std::cout << "TT lookups: " << number_of_transposition_table_lookups << std::endl;
    std::cout << "TT hits: " << number_of_transposition_table_hits << std::endl;
    std::cout << "TT stores: " << number_of_transposition_table_stores << std::endl;

//    engine_board.evaluate_board();

//    printf("64bit: %llp", engine_board.get_zobrist_hash());
}

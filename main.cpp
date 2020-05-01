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
//    KillerMovesSlot killer_moves[8];
//    engine_transposition_table.initialize_transposition_table(25);
//    engine_transposition_table.new_search();
//    //To test 29/03/2020.
//    number_of_transposition_table_lookups = 0;
//    number_of_transposition_table_hits = 0;
//    number_of_transposition_table_stores = 0;
//    //End test 29/03/2020.
//    for (uint8_t depth = 1; depth < 6; depth++){
//        root_depth = depth;
//        std::cout << "Value: " << -alpha_beta_search(depth, -3000, 3000, 0, killer_moves) << "    ";
//        std::cout << "Move: " << PV_move << std::endl;
//    }
//    engine_transposition_table.release_memory();
//    std::cout << "TT lookups: " << number_of_transposition_table_lookups << std::endl;
//    std::cout << "TT hits: " << number_of_transposition_table_hits << std::endl;
//    std::cout << "TT stores: " << number_of_transposition_table_stores << std::endl;

//    engine_board.make_move(42);
//    engine_board.make_move(41);
//    engine_board.make_move(40);
//    engine_board.make_move(39);
//    engine_board.make_move(38);
//    engine_board.make_move(37);
//    engine_board.make_move(36);
//    engine_board.make_move(35);
//    engine_board.make_move(34);
//    for (int i = 0; i < 1000000; i++){
//        engine_board.evaluate_board();
//    }
//    std::cout << engine_board.evaluate_board();

    KillerMovesSlot killer_moves[8];
    engine_transposition_table.initialize_transposition_table(25);
    engine_transposition_table.new_search();
    //To test 29/03/2020.
    number_of_transposition_table_lookups = 0;
    number_of_transposition_table_hits = 0;
    number_of_transposition_table_stores = 0;
    //End test 29/03/2020.
    while(0 == engine_board.check_winning_conditions()){
        for (uint8_t depth = 1; depth < 4; depth++){
            root_depth = depth;
            std::cout << "Value: " << -alpha_beta_search(depth, -3000, 3000, 0, killer_moves) << "    ";
        }
        root_depth = 4;
        std::cout << "Value: " << -alpha_beta_search(4, -3000, 3000, 0, killer_moves) << "    ";
        std::cout << "Move: " << PV_move << std::endl;
        engine_board.make_move(PV_move);
        engine_transposition_table.new_search();
    }
    engine_transposition_table.release_memory();
    std::cout << "TT lookups: " << number_of_transposition_table_lookups << std::endl;
    std::cout << "TT hits: " << number_of_transposition_table_hits << std::endl;
    std::cout << "TT stores: " << number_of_transposition_table_stores << std::endl;

//    printf("64bit: %llp", engine_board.get_zobrist_hash());
}

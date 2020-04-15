//TEMPORAL
#include <iostream>
//TEMPORAL
#include "Search.h"

uint8_t root_depth;
uint16_t PV_move;

int16_t alpha_beta_search(uint8_t depth, int16_t alpha, int16_t beta, uint8_t actual_killer_move_index, KillerMovesSlot *killer_moves){
    //Transposition table lookup.
    int16_t alpha_original = alpha;
    bool is_hash_move_valid = false;
    uint16_t hash_move;
    TranspositionData transposition_table_return;
    if (engine_transposition_table.transposition_table_lookup(engine_board.get_zobrist_hash(), transposition_table_return) == true){
        if (0 == transposition_table_return.residual_depth){
            if (0 == depth){
                return transposition_table_return.evaluation_value;
            }
        } else {
            is_hash_move_valid = true;
            hash_move = transposition_table_return.best_move;
            //TEMPORAL ¡¡¡We can enhance the results if we use >= instead from ==!!!
            //BUT, the evaluation function have to have corrections from tempo.
            if (transposition_table_return.residual_depth == depth){
                switch (transposition_table_return.bound){
                    case 1:
                        return transposition_table_return.evaluation_value;
                    case 2:
                        if (transposition_table_return.evaluation_value > alpha){
                            alpha = transposition_table_return.evaluation_value;
                        }
                        break;
                    case 3:
                        if (transposition_table_return.evaluation_value < beta){
                            beta = transposition_table_return.evaluation_value;
                        }
                }
                if (alpha >= beta){
                    return transposition_table_return.evaluation_value;
                }
            }
        }
    }
    //Evaluation of the node if we have achieved depth zero.
    if (depth == 0){
        int16_t engine_evaluation = engine_board.evaluate_board();
        if (engine_evaluation > 1500){
            engine_evaluation -= int16_t(root_depth);
        }
        if (engine_evaluation < -1500){
            engine_evaluation += int16_t(root_depth);
        }
        transposition_table_return.evaluation_value = engine_evaluation;
        transposition_table_return.residual_depth = 0;
        engine_transposition_table.transposition_table_store(engine_board.get_zobrist_hash(), transposition_table_return);
        return engine_evaluation;
    }
    //We check for winning conditions.
    int8_t checking_winning_conditions = engine_board.check_winning_conditions();
    if (checking_winning_conditions != 0){
        int16_t returning_variable = int16_t(checking_winning_conditions*engine_board.get_player_to_move()*2000);
        if (returning_variable > 0){
            returning_variable -= int16_t(root_depth-depth);
        } else {
            returning_variable += int16_t(root_depth-depth);
        }
        return returning_variable;
    }
    //We generate the moves.
    GeneratedMoves *generated_moves = engine_board.generate_possible_moves();
    //We reorder generated moves to prove first hash move (Better will be a stagged move generation).
    uint16_t number_of_ordered_moves = 0;
    if (is_hash_move_valid == true){
        bool continue_searching_hash_move = true;
        uint16_t index_of_search = number_of_ordered_moves;
        //PRECAUTION !!! Note that hash move can be illegal (Type 1 collision in TT).
        while(continue_searching_hash_move == true && index_of_search < generated_moves->number_of_generated_moves){
            if (generated_moves->moves[index_of_search]==hash_move){
                generated_moves->moves[index_of_search] = generated_moves->moves[number_of_ordered_moves];
                generated_moves->moves[number_of_ordered_moves] = hash_move;
                continue_searching_hash_move = false;
                number_of_ordered_moves++;
            }
            index_of_search++;
        }
    }
    //After the hash move, we will try killer moves.
    //Killer move number one.
    bool continue_searching_killer_move = true;
    uint16_t index_of_search = number_of_ordered_moves;
    while (continue_searching_killer_move == true && index_of_search < generated_moves->number_of_generated_moves){
        if (generated_moves->moves[index_of_search] == killer_moves[actual_killer_move_index].move_1){
            generated_moves->moves[index_of_search] = generated_moves->moves[number_of_ordered_moves];
            generated_moves->moves[number_of_ordered_moves] = killer_moves[actual_killer_move_index].move_1;
            continue_searching_killer_move = false;
            number_of_ordered_moves++;
        }
        index_of_search++;
    }
    //Killer move number two.
    index_of_search = number_of_ordered_moves;
    while (continue_searching_killer_move == true && index_of_search < generated_moves->number_of_generated_moves){
        if (generated_moves->moves[index_of_search] == killer_moves[actual_killer_move_index].move_2){
            generated_moves->moves[index_of_search] = generated_moves->moves[number_of_ordered_moves];
            generated_moves->moves[number_of_ordered_moves] = killer_moves[actual_killer_move_index].move_2;
            continue_searching_killer_move = false;
            number_of_ordered_moves++;
        }
        index_of_search++;
    }
    //Killer move number three.
    index_of_search = number_of_ordered_moves;
    while (continue_searching_killer_move == true && index_of_search < generated_moves->number_of_generated_moves){
        if (generated_moves->moves[index_of_search] == killer_moves[actual_killer_move_index].move_3){
            generated_moves->moves[index_of_search] = generated_moves->moves[number_of_ordered_moves];
            generated_moves->moves[number_of_ordered_moves] = killer_moves[actual_killer_move_index].move_3;
            continue_searching_killer_move = false;
            number_of_ordered_moves++;
        }
        index_of_search++;
    }
    //We launch new searches.
    int16_t actual_value = -2000;
    uint16_t actual_best_move;
    for (uint16_t index_move = 0; index_move < generated_moves->number_of_generated_moves; index_move++){
        engine_board.make_move(generated_moves->moves[index_move]);
        int16_t subsearch_value = -alpha_beta_search(depth-1, -beta, -alpha, actual_killer_move_index+1, killer_moves);
        engine_board.unmake_move(generated_moves->moves[index_move]);
        if ((subsearch_value > actual_value) == true){
            actual_value = subsearch_value;
            actual_best_move = generated_moves->moves[index_move];
        }
        if ((actual_value > alpha) == true){
            alpha = actual_value;
        }
        if ((alpha >= beta) == true){
            //We save the move that causes the cutoff.
            if (killer_moves[actual_killer_move_index].move_1 == actual_best_move){
                //Do nothing.
            } else {
                if (killer_moves[actual_killer_move_index].move_2 == actual_best_move){
                    killer_moves[actual_killer_move_index].move_2 = killer_moves[actual_killer_move_index].move_1;
                    killer_moves[actual_killer_move_index].move_1 = actual_best_move;
                } else {
                    killer_moves[actual_killer_move_index].move_3 = killer_moves[actual_killer_move_index].move_2;
                    killer_moves[actual_killer_move_index].move_2 = killer_moves[actual_killer_move_index].move_1;
                    killer_moves[actual_killer_move_index].move_1 = actual_best_move;
                }
            }
            break; //We end the search now.
        }
    }
    //Transposition table store.
    transposition_table_return.best_move = actual_best_move;
    transposition_table_return.residual_depth = depth;
    transposition_table_return.evaluation_value = actual_value;
    if (actual_value <= alpha_original){
        transposition_table_return.bound = 3;
    } else {
        if (actual_value >= beta){
            transposition_table_return.bound = 2;
        } else {
            transposition_table_return.bound = 1;
        }
    }
    engine_transposition_table.transposition_table_store(engine_board.get_zobrist_hash(), transposition_table_return);
    //We return root move selected.
    if (depth == root_depth){
        PV_move = actual_best_move;
    }
    delete generated_moves;
    return actual_value;
}

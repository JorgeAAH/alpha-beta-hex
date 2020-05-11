#include "HexBoard.h"
#include <cmath>
#include <random>
#include <iostream>
#include "IncrementalEvaluationMatrix.h"
#include "UnionFindData.h"

HexBoard engine_board(13);

GeneratedMoves::GeneratedMoves(uint16_t number_of_hexes){
    moves = new uint16_t[number_of_hexes];
    number_of_generated_moves = 0;
}

GeneratedMoves::~GeneratedMoves(){
    delete[] moves;
}

HexBoard::HexBoard(uint16_t side_size_parameter){
    side_size = side_size_parameter;
    number_of_hexes = side_size*side_size;
    big_board_size = (side_size+2)*(side_size+2);
    number_of_moves_maked = 0;
    player_to_move = -1;
    big_board = new int8_t[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        big_board[i] = 2;
    }
    //We construct mailbox_to_big_board and clear
    //big_board game hexes.
    mailbox_to_big_board = new uint16_t[number_of_hexes];
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t row_number = i / side_size;
        uint16_t column_number = i % side_size;
        row_number++;
        column_number++;
        mailbox_to_big_board[i] = row_number*(side_size+2)+column_number;
        big_board[mailbox_to_big_board[i]] = 0;
    }
    //CRUCIAL to game performance; move generation.
    move_generator_to_big_board = new uint16_t[number_of_hexes];
    float *hex_distance = new float[number_of_hexes];
    bool *is_hex_selected = new bool[number_of_hexes];
    //(A small  number is better).
    for (uint16_t i = 0; i < number_of_hexes; i++){
        float row_number = float (i / side_size);
        float column_number = float (i % side_size);
        float row_distance_to_center = row_number-float(side_size-1)/2;
        float column_distance_to_center = column_number-float(side_size-1)/2;
        float euclidean_distance_contribution = std::sqrt(row_distance_to_center*row_distance_to_center
                                                          +column_distance_to_center*column_distance_to_center);
        float distance_to_large_diagonal = (row_distance_to_center-column_distance_to_center)/2;
        float distance_to_short_diagonal = (row_distance_to_center+column_distance_to_center)/2;
        float diagonal_distance_contribution = std::sqrt(distance_to_short_diagonal*distance_to_short_diagonal
                                                         +0.5F*distance_to_large_diagonal*distance_to_large_diagonal);
        hex_distance[i]=diagonal_distance_contribution+euclidean_distance_contribution;
        is_hex_selected[i]=false;
    }
    //Using hex_distance to improve move generation.
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t actual_selected_hex = 0;
        float actual_minimal_distance = 1000.0F;
        for (uint16_t j = 0; j < number_of_hexes; j++){
            if ((hex_distance[j] < actual_minimal_distance) && (is_hex_selected[j]==false)){
                actual_minimal_distance = hex_distance[j];
                actual_selected_hex = j;
            }
        }
        is_hex_selected[actual_selected_hex] = true;
        move_generator_to_big_board[i] = mailbox_to_big_board[actual_selected_hex];
    }
    delete []is_hex_selected;
    delete []hex_distance;
    //Generation of alpha and beta masks.
    blue_connected_alpha = new uint16_t[side_size];
    blue_connected_beta = new uint16_t[side_size];
    red_connected_alpha = new uint16_t[side_size];
    red_connected_beta = new uint16_t[side_size];
    for (uint16_t i = 0; i < side_size; i++){
        blue_connected_alpha[i] = (i+1)*(side_size+2)+1;
        blue_connected_beta[i] = (i+1)*(side_size+2)+side_size;
        red_connected_alpha[i] = (side_size+2) + (i+1);
        red_connected_beta[i] = (side_size)*(side_size+2) + (i+1);
    }
    std::mt19937_64 twister(0x490b6d35d8f18c41ULL);
    std::uniform_int_distribution<long long unsigned int > dist;
    zobrist_const_empty_board = dist(twister);
    zobrist_hash = zobrist_const_empty_board;
    zobrist_const = new uint64_t*[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        zobrist_const[i] = new uint64_t[2];
        zobrist_const[i][0] = dist(twister);
        zobrist_const[i][1] = dist(twister);
    }
    //Generation of IncrementalEvaluationMatrix for blue and red perspective
    evaluation_from_blue_perspective = new IncrementalEvaluationMatrix(side_size, big_board_size, 1, big_board, mailbox_to_big_board, blue_connected_alpha, blue_connected_beta);
    evaluation_from_red_perspective = new IncrementalEvaluationMatrix(side_size, big_board_size, -1, big_board, mailbox_to_big_board, red_connected_alpha, red_connected_beta);
}

HexBoard::~HexBoard(){
    delete []big_board;
    delete []mailbox_to_big_board;
    delete []move_generator_to_big_board;
    delete []blue_connected_alpha;
    delete []blue_connected_beta;
    delete []red_connected_alpha;
    delete []red_connected_beta;
    for(uint16_t i = 0; i < big_board_size; i++){
        delete [] zobrist_const[i];
    }
    delete []zobrist_const;
    delete evaluation_from_blue_perspective;
    delete evaluation_from_red_perspective;
}

uint64_t HexBoard::get_zobrist_hash(){
    return zobrist_hash;
}

uint16_t HexBoard::make_move(uint16_t move_made){
    zobrist_hash = zobrist_hash ^ zobrist_const[move_made][(player_to_move+1)/2];
    big_board[move_made]=player_to_move;
    player_to_move *= -1;
    number_of_moves_maked++;
    evaluation_from_blue_perspective->set_hex_to_board(big_board, move_made);
    evaluation_from_red_perspective->set_hex_to_board(big_board, move_made);
    return 0;
}

uint16_t HexBoard::unmake_move(uint16_t move_to_be_unmade){
    zobrist_hash = zobrist_hash ^ zobrist_const[move_to_be_unmade][(big_board[move_to_be_unmade]+1)/2];
    big_board[move_to_be_unmade]=0;
    player_to_move *= -1;
    number_of_moves_maked--;
    evaluation_from_blue_perspective->set_hex_to_board(big_board, move_to_be_unmade);
    evaluation_from_red_perspective->set_hex_to_board(big_board, move_to_be_unmade);
    return 0;
}

uint16_t HexBoard::reset_the_board(){
    for (uint16_t i = 0; i < number_of_hexes; i++){
        big_board[mailbox_to_big_board[i]]=0;
    }
    player_to_move = -1;
    zobrist_hash = zobrist_const_empty_board;
    number_of_moves_maked = 0;
    for (uint16_t i = 0; i < number_of_hexes; i++){
        evaluation_from_blue_perspective->set_hex_to_board(big_board, mailbox_to_big_board[i]);
        evaluation_from_red_perspective->set_hex_to_board(big_board, mailbox_to_big_board[i]);
    }
    return 0;
}

int8_t HexBoard::get_player_to_move(){
    return player_to_move;
}

int8_t HexBoard::check_winning_conditions(){
    UnionFindData connection_data(big_board_size);
    //We begin with the process of making connections square by square.
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t board_position = mailbox_to_big_board[i];
        if (big_board[board_position]==1){
            if (big_board[board_position+1]==1){
                connection_data.make_union(board_position,board_position+1);
            }
            if (big_board[board_position+(side_size+1)]==1){
                connection_data.make_union(board_position,board_position+(side_size+1));
            }
            if (big_board[board_position+(side_size+2)]==1){
                connection_data.make_union(board_position,board_position+(side_size+2));
            }
        }
        if (big_board[board_position]==-1){
            if (big_board[board_position+1]==-1){
                connection_data.make_union(board_position,board_position+1);
            }
            if (big_board[board_position+(side_size+1)]==-1){
                connection_data.make_union(board_position,board_position+(side_size+1));
            }
            if (big_board[board_position+(side_size+2)]==-1){
                connection_data.make_union(board_position,board_position+(side_size+2));
            }
        }
    }
    //After we make the connections we have to search for groups connected with both sides.
    bool *is_connected_alpha = new bool[big_board_size];
    bool *is_connected_beta = new bool[big_board_size];

    for (uint16_t i = 0; i < big_board_size; i++){
        is_connected_alpha[i] = false;
        is_connected_beta[i] = false;
    }

    //Searching for connections with alpha for blue.
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_square = blue_connected_alpha[i];
        if(big_board[actual_square]==1){
            is_connected_alpha[connection_data.find_parent(actual_square)] = true;
        }
    }
    //Searching for connections with beta for blue.
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_square = blue_connected_beta[i];
        if(big_board[actual_square]==1){
            is_connected_beta[connection_data.find_parent(actual_square)] = true;
        }
    }
    //Searching for connections with alpha for red.
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_square = red_connected_alpha[i];
        if(big_board[actual_square]==-1){
            is_connected_alpha[connection_data.find_parent(actual_square)] = true;
        }
    }
    //Searching for connections with beta for red.
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_square = red_connected_beta[i];
        if(big_board[actual_square]==-1){
            is_connected_beta[connection_data.find_parent(actual_square)] = true;
        }
    }
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t board_position = mailbox_to_big_board[i];
        if(is_connected_alpha[board_position] == true && is_connected_beta[board_position] == true){
            delete []is_connected_alpha;
            delete []is_connected_beta;
            return big_board[board_position];
        }
    }
    delete []is_connected_alpha;
    delete []is_connected_beta;
    return 0;
}

int16_t HexBoard::evaluate_board(){
    int8_t is_game_finished = check_winning_conditions();
    if (is_game_finished != 0){
        int16_t return_variable = int16_t(is_game_finished)*2000;
        //Negamax implementation.
        return return_variable*int16_t(player_to_move);
    }
    //Makes evaluation from blue.
    float current_flux_for_blue = evaluation_from_blue_perspective->evaluate();
    //Makes evaluation from red.
    float current_flux_for_red = evaluation_from_red_perspective->evaluate();
    //Makes evaluation from blue perspective.
    float evaluation_score = current_flux_for_blue-current_flux_for_red;
//    std::cout << "---------------------------------" << std::endl;
//    std::cout << "Corriente azul: " << current_flux_for_blue << std::endl;
//    std::cout << "Corriente roja: " << current_flux_for_red << std::endl;
//    std::cout << "Diferencia: " << evaluation_score << std::endl;
//    evaluation_score = std::exp(evaluation_score*3.0F/float(side_size)); // Depends on board size!!!
    evaluation_score = std::exp(evaluation_score*0.5F);
    evaluation_score = (evaluation_score-(1/evaluation_score))/(evaluation_score+(1/evaluation_score));
//    std::cout << "Resultado de evaluacion: " << evaluation_score << std::endl;
    int16_t return_variable = int16_t(evaluation_score*1000.0F);
    //Negamax implementation.
    return return_variable*int16_t(player_to_move);
}

GeneratedMoves *HexBoard::generate_possible_moves(){
    GeneratedMoves *returning_moves = new GeneratedMoves(number_of_hexes);
    returning_moves->number_of_generated_moves = 0;
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t possible_move = move_generator_to_big_board[i];
        if(big_board[possible_move]==0){
            returning_moves->moves[returning_moves->number_of_generated_moves] = possible_move;
            returning_moves->number_of_generated_moves++;
        }
    }
    return returning_moves;
}

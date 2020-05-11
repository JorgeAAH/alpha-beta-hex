#include "IncrementalEvaluationMatrix.h"
#include <iostream>
#include "SparseMatrix.h"

IncrementalEvaluationMatrix::IncrementalEvaluationMatrix(uint16_t side_size_parameter, uint16_t big_board_size, int8_t player_evaluated_parameter, int8_t *big_board, uint16_t *mailbox_to_big_board, uint16_t *connected_alpha, uint16_t *connected_beta){
    side_size = side_size_parameter;
    player_evaluated = player_evaluated_parameter;
    voltage_assigned = new int8_t[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        voltage_assigned[i] = 0;
    }
    for (uint16_t i = 0; i < side_size; i++){
        voltage_assigned[connected_alpha[i]] = 1;
        voltage_assigned[connected_beta[i]] = -1;
    }
    mailbox_to_matrix = new uint16_t[big_board_size];
    if (1 == player_evaluated){ //We have blue perspective.
        for (uint16_t i = 0; i < side_size; i++){
            for (uint16_t j = 0; j < (side_size-2); j++){
                mailbox_to_matrix[(i+1)*(side_size+2)+(j+2)] = i*(side_size-2)+j;
            }
        }
    } else { //We have red perspective.
        for (uint16_t i = 0; i < (side_size-2); i++){
            for (uint16_t j = 0; j < side_size; j++){
                mailbox_to_matrix[(i+2)*(side_size+2)+(j+1)] = i*side_size+j;
            }
        }
    }
    number_of_unknowns = (side_size-2)*side_size;
    b_column_matrix = new float[number_of_unknowns];
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        b_column_matrix[i] = 0.0F;
    }
    matrix = new SparseMatrix(number_of_unknowns);
    for (uint16_t i = 0; i < side_size*side_size; i++){
        if (0 == voltage_assigned[mailbox_to_big_board[i]]){
            set_hex_to_board(big_board, mailbox_to_big_board[i]);
        }
    }
}

IncrementalEvaluationMatrix::~IncrementalEvaluationMatrix(){
    delete [] voltage_assigned;
    delete [] mailbox_to_matrix;
    delete [] b_column_matrix;
    delete matrix;
}

uint16_t IncrementalEvaluationMatrix::set_hex_to_board(int8_t *big_board, uint16_t hex_setted){
    if (0 == voltage_assigned[hex_setted]){
        uint16_t actual_matrix_position = mailbox_to_matrix[hex_setted];
        float actual_diagonal_value = 0.0F;
        b_column_matrix[actual_matrix_position] = 0.0F;
        for (uint16_t i = 0; i < 6; i++){
            uint16_t adjacent_board_position;
            switch(i){
            case 0:
                adjacent_board_position = hex_setted + 1;
                break;
            case 1:
                adjacent_board_position = hex_setted + (side_size+2);
                break;
            case 2:
                adjacent_board_position = hex_setted + (side_size+1);
                break;
            case 3:
                adjacent_board_position = hex_setted -1 ;
                break;
            case 4:
                adjacent_board_position = hex_setted - (side_size+2);
                break;
            case 5:
                adjacent_board_position = hex_setted - (side_size+1);
                break;
            }
            if (2 != big_board[adjacent_board_position]){
                float total_resistence = 0.0F;
                if (0==big_board[hex_setted]){
                    total_resistence += 0.5F;
                }
                if (1==(player_evaluated*big_board[hex_setted])){
                    total_resistence += 0.005F;
                }
                if (-1==(player_evaluated*big_board[hex_setted])){
                    total_resistence += 50.0F;
                }
                if (0==big_board[adjacent_board_position]){
                    total_resistence += 0.5F;
                }
                if (1==(player_evaluated*big_board[adjacent_board_position])){
                    total_resistence += 0.005F;
                }
                if (-1==(player_evaluated*big_board[adjacent_board_position])){
                    total_resistence += 50.0F;
                }
                float inverse_total_resistence = 1.0F/total_resistence;
                actual_diagonal_value += inverse_total_resistence;
                if (0 == voltage_assigned[adjacent_board_position]){
                    matrix->assignement(actual_matrix_position, mailbox_to_matrix[adjacent_board_position], -inverse_total_resistence);
                    //We also update adjacent hex data.
                    matrix->assignement(mailbox_to_matrix[adjacent_board_position], actual_matrix_position, -inverse_total_resistence);
                    matrix->update_row_diagonal(mailbox_to_matrix[adjacent_board_position], b_column_matrix[mailbox_to_matrix[adjacent_board_position]]);
                }
                if (1 == voltage_assigned[adjacent_board_position]){
                    b_column_matrix[actual_matrix_position] += inverse_total_resistence;
                }
                if (-1 == voltage_assigned[adjacent_board_position]){
                    b_column_matrix[actual_matrix_position] -= inverse_total_resistence;
                }
            }
        }
        matrix->assignement(actual_matrix_position, actual_matrix_position, actual_diagonal_value);
    } else {
        for (uint16_t i = 0; i < 6; i++){
            uint16_t adjacent_board_position;
            switch(i){
            case 0:
                adjacent_board_position = hex_setted + 1;
                break;
            case 1:
                adjacent_board_position = hex_setted + (side_size+2);
                break;
            case 2:
                adjacent_board_position = hex_setted + (side_size+1);
                break;
            case 3:
                adjacent_board_position = hex_setted -1 ;
                break;
            case 4:
                adjacent_board_position = hex_setted - (side_size+2);
                break;
            case 5:
                adjacent_board_position = hex_setted - (side_size+1);
                break;
            }
            if (2 != big_board[adjacent_board_position]){
                if (0 == voltage_assigned[adjacent_board_position]){
                    set_hex_to_board(big_board, adjacent_board_position);
                }
            }
        }
    }
    return 0;
}

float IncrementalEvaluationMatrix::evaluate(){
    float epsilon = 0.000000001F;
    float **plain_matrix = matrix->get_plain_matrix_with_column(b_column_matrix);
    solve_matrix_gauss(plain_matrix, number_of_unknowns, side_size);
//    if (1 == player_evaluated){
//        std::cout << std::endl;
//        for (uint16_t i = 0; i < number_of_unknowns; i++){
//            std::cout << plain_matrix[i][number_of_unknowns] << std::endl;
//        }
//    }
    float current = 0.0F;
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        if (b_column_matrix[i] > epsilon){
            current += (1.0F-plain_matrix[i][number_of_unknowns])*b_column_matrix[i];
        } else {
            if (b_column_matrix[i] < -epsilon){
                current -= (1.0F+plain_matrix[i][number_of_unknowns])*b_column_matrix[i];
            }
        }
    }
//    if (1 == player_evaluated){
//        std::cout << std::endl;
//        for (uint16_t i = 0; i < number_of_unknowns; i++){
//            std::cout << b_column_matrix[i] << std::endl;
//        }
//    }
    for(uint16_t i = 0; i < number_of_unknowns; i++){
        delete [] plain_matrix[i];
    }
    delete [] plain_matrix;
//    std::cout << std::endl;
//    std::cout << current*0.5 << std::endl;
    return current*0.5F;
}

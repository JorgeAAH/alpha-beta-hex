#include "IncrementalEvaluationMatrix.h"
#include <iostream>
#include <cmath>

uint16_t solve_matrix_gauss(float **matrix, uint16_t number_of_unknowns, uint16_t central_diagonal_width);

class SparseMatrix {
    public:
        SparseMatrix (uint16_t number_of_unknowns_parameter);
        ~SparseMatrix ();
        uint16_t assignement(uint16_t row, uint16_t column, float assignement_value);
        uint16_t update_row_diagonal(uint16_t row, float b_column_row_value);
        float *solve_matrix(float *b_column_matrix);
        float **get_plain_matrix_with_column(float *b_column_matrix);

    private:
        float *multiply_sparse_matrix_with_column_matrix(float *column_matrix);
        uint16_t number_of_unknowns;
        uint16_t *row_occupation;
        uint16_t **matrix_column_positions;
        float **matrix_values;
};

uint16_t solve_matrix_gauss(float **matrix, uint16_t total_number_of_unknowns, uint16_t central_diagonal_width){
//    for(uint16_t i = 0; i < total_number_of_unknowns; i++){
//        std::cout << "[" << matrix[i][0] ;
//        for(uint16_t j=1; j<total_number_of_unknowns; j++){
//            std::cout << "," << matrix[i][j] ;
//        }
//        std::cout << "],";
//    }
//    std::cout << std::endl;
//    std::cout << "[";
//    for(uint16_t i = 0; i < total_number_of_unknowns; i++){
//        std::cout << matrix[i][total_number_of_unknowns] << ",";
//    }
//    std::cout << std::endl;
//    std::cout << std::endl;
    float epsilon = 0.00000001F;
    for (uint16_t i=0;i<total_number_of_unknowns-1;i++){
        uint16_t optimization_big_diagonal = i+central_diagonal_width;
        for (uint16_t k=i+1;((k<total_number_of_unknowns)&&(k<=optimization_big_diagonal));k++){
            if (matrix[k][i]*matrix[k][i] > epsilon){
                float t=matrix[k][i]/matrix[i][i];
                for (uint16_t j=i;((j<total_number_of_unknowns)&&(j<=optimization_big_diagonal));j++){
                    //make the elements below the pivot elements equal to zero or eliminate the variables.
                    matrix[k][j]=matrix[k][j]-t*matrix[i][j];
                }
                matrix[k][total_number_of_unknowns]=matrix[k][total_number_of_unknowns]-t*matrix[i][total_number_of_unknowns];
            }
        }
    }
    for (uint16_t i = total_number_of_unknowns-1; i > 0; i--){
        //In the following loop the value used inside the loop isn't k, but k-1.
        //The problem is in the loop condition, as k can't be negative.
        uint16_t optimization_big_diagonal;
        if (i<central_diagonal_width){
            optimization_big_diagonal = 0;
        } else {
            optimization_big_diagonal = i-central_diagonal_width;
        }
        for (uint16_t k = i; ((k > 0)&&(k>optimization_big_diagonal)); k--){
            uint16_t t_k = k-1;
            float t= matrix[t_k][i]/matrix[i][i];
            matrix[t_k][i]=matrix[t_k][i]-t*matrix[i][i];
            matrix[t_k][total_number_of_unknowns]=matrix[t_k][total_number_of_unknowns]-t*matrix[i][total_number_of_unknowns];
        }

    }
    for (uint16_t i = 0; i < total_number_of_unknowns; i++){
        matrix[i][total_number_of_unknowns]= matrix[i][total_number_of_unknowns]/matrix[i][i];
        matrix[i][i]=1.0F;
    }
    return 0;
}

SparseMatrix::SparseMatrix(uint16_t number_of_unknowns_parameter){
    number_of_unknowns = number_of_unknowns_parameter;
    row_occupation = new uint16_t[number_of_unknowns];
    matrix_column_positions = new uint16_t*[number_of_unknowns];
    matrix_values = new float*[number_of_unknowns];
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        row_occupation[i] = 0;
        //We only need 7 places: 1 for the central hexagon and 6 for the adjacent.
        matrix_column_positions[i] = new uint16_t[7];
        matrix_values[i] = new float[7];
    }
}

SparseMatrix::~SparseMatrix(){
    delete [] row_occupation;
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        delete [] matrix_column_positions[i];
        delete [] matrix_values[i];
    }
    delete [] matrix_column_positions;
    delete [] matrix_values;
}

uint16_t SparseMatrix::assignement(uint16_t row, uint16_t column, float assignement_value){
    uint16_t actual_row_occupation = row_occupation[row];
    for (uint16_t i = 0; i < actual_row_occupation; i++){
        if (column == matrix_column_positions[row][i]){
            //The element exist on the sparse matrix, so we only add-assign.
            matrix_values[row][i] = assignement_value;
            return 0;
        }
    }
    //We create the new element of the sparse matrix.
    matrix_column_positions[row][actual_row_occupation] = column;
    matrix_values[row][actual_row_occupation] = assignement_value;
    row_occupation[row]++;
    return 0;
}

uint16_t SparseMatrix::update_row_diagonal(uint16_t row, float b_column_row_value){
    uint16_t actual_row_occupation = row_occupation[row];
    float total_sum = std::abs(b_column_row_value);
    uint16_t diagonal_position;
    bool is_diagonal_position_stored = false;
    for (uint16_t i = 0; i < actual_row_occupation; i++){
        if (row == matrix_column_positions[row][i]){
            //We have the position of the diagonal.
            diagonal_position = i;
            is_diagonal_position_stored = true;
        } else {
            total_sum -= matrix_values[row][i];
        }
    }
    if (true == is_diagonal_position_stored){
        matrix_values[row][diagonal_position] = total_sum;
    } else {
        //We create the new element of the sparse matrix.
        matrix_column_positions[row][actual_row_occupation] = row;
        matrix_values[row][actual_row_occupation] = total_sum;
        row_occupation[row]++;
    }
    return 0;
}

float *SparseMatrix::solve_matrix(float *b_column_matrix){
    //We prepare the matrix for fastest resolution.
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        uint16_t diagonal_position = 0;
        while(i != matrix_column_positions[i][diagonal_position]){
            diagonal_position++;
        }
        float diagonal_inverse = 1.0F/matrix_values[i][diagonal_position];
        b_column_matrix[i] = b_column_matrix[i]*diagonal_inverse;
        for(uint16_t j = 0; j < row_occupation[i]; j++){
            matrix_values[i][j] = matrix_values[i][j]*diagonal_inverse;
        }
    }
    //We use the following implementation of conjugate gradient descent (from Wikipedia):
    //r_0 = b - Ax_0
    float *actual_x = new float[number_of_unknowns];
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        actual_x[i] = 0.0F;
    }
    float *actual_r = multiply_sparse_matrix_with_column_matrix(actual_x);
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        actual_r[i] = b_column_matrix[i] - actual_r[i];
    }
    //p_0 = r_0
    float *actual_p = new float[number_of_unknowns];
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        actual_p[i] = actual_r[i];
    }
    //k = 0
    uint16_t k = 0;
    float *future_x = new float[number_of_unknowns];
    float *future_r = new float[number_of_unknowns];
    float *future_p = new float[number_of_unknowns];
    float actual_r_dot = 0.0F;
    float future_r_dot = 0.0F;
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        actual_r_dot += actual_r[i]*actual_r[i];
    }
    //Repeat
    while (true){
        float a;
        float B;
        //  a_k = <r_k,r_k>/<p_k,p_k>A
        float *actual_Ap = multiply_sparse_matrix_with_column_matrix(actual_p);
        float denominator = 0.0F;
        for (uint16_t i = 0; i < number_of_unknowns; i++){
            denominator += actual_p[i]*actual_Ap[i];
        }
        a = actual_r_dot/denominator;
        //  x_(k+1) = x_k + a_k*p_k
        for (uint16_t i = 0; i < number_of_unknowns; i++){
            future_x[i] = actual_x[i] + a*actual_p[i];
        }
        //  r_(k+1) = r_k - a_k*A*p_k
        for (uint16_t i = 0; i < number_of_unknowns; i++){
            future_r[i] = actual_r[i] - a*actual_Ap[i];
        }
        delete [] actual_Ap;
        //  Exit if r_(k+1) is small
        float max_error = 0.0F;
        for (uint16_t i = 0; i < number_of_unknowns; i++){
            if (max_error < std::abs(future_r[i])){
                max_error = std::abs(future_r[i]);
            }
        }
        if ((max_error < 0.01F)|| (k > 30)){
            delete [] actual_x;
            delete [] actual_r;
            delete [] actual_p;
            delete [] future_r;
            delete [] future_p;
            return future_x;
        }
        //  B_k = <r_(k+1), r_(k+1)>/<r_k, r_k>
        future_r_dot = 0.0F;
        for (uint16_t i = 0; i < number_of_unknowns; i++){
            future_r_dot += future_r[i]*future_r[i];
        }
        B = future_r_dot/actual_r_dot;
        //  p_(k+1) = r_(k+1) + B_k*p_k
        for (uint16_t i = 0; i < number_of_unknowns; i++){
            future_p[i] = future_r[i]+B*actual_p[i];
        }
        k++;
        //Preparing next iteration.
        float *temp;
        temp = actual_x;
        actual_x = future_x;
        future_x = temp;
        temp = actual_r;
        actual_r = future_r;
        future_r = temp;
        temp = actual_p;
        actual_p = future_p;
        future_p = temp;
        actual_r_dot = future_r_dot;
    }
}

float *SparseMatrix::multiply_sparse_matrix_with_column_matrix(float *column_matrix){
    float *result_matrix = new float[number_of_unknowns];
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        result_matrix[i] = 0.0F;
        uint16_t actual_row_occupation = row_occupation[i];
        for (uint16_t j = 0; j < actual_row_occupation; j++){
            result_matrix[i] += matrix_values[i][j]*column_matrix[matrix_column_positions[i][j]];
        }
    }
    return result_matrix;
}

float **SparseMatrix::get_plain_matrix_with_column(float *b_column_matrix){
    float **plain_matrix = new float*[number_of_unknowns];
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        plain_matrix[i] = new float[number_of_unknowns + 1];
        for (uint16_t j = 0; j < number_of_unknowns + 1; j++){
            plain_matrix[i][j] = 0.0F;
        }
    }
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        for (uint16_t j = 0; j < row_occupation[i]; j++){
            plain_matrix[i][matrix_column_positions[i][j]] = matrix_values[i][j];
        }
        plain_matrix[i][number_of_unknowns] = b_column_matrix[i];
    }
    return plain_matrix;
}

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

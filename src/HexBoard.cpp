#include <cmath>
#include <random>
#include <iostream>
#include "HexBoard.h"

HexBoard engine_board(13);

uint16_t solve_matrix(float **matrix, uint16_t number_of_unknowns, uint16_t central_diagonal_width);

class UnionFindData {
    public:
        UnionFindData(uint16_t big_board_size);
        UnionFindData(const UnionFindData &copied_structure);
        ~UnionFindData();
        bool are_connected(uint16_t id_a, uint16_t id_b);
        uint16_t make_union(uint16_t id_a, uint16_t id_b);
        uint16_t find_parent(uint16_t id_a);

    private:
        //We initialize this array with a dynamic size given by the size of the board.
        uint16_t *id_parent;
        //Number_of_children only takes a non-zero value if its the absolute parent of
        //his group.
        //We initialize this array with a dynamic size given by the size of the board.
        uint16_t *number_of_children;
        //number of hexes in the board.
        uint16_t big_board_size;
        //IMPORTANT!!! id_parent and number_of_children are accessed by moves to big_board, so
        //them have to had big_board size. (Obviously, some parts of the arrays will not be used).
};

class MatrixSolver {
    public:
        MatrixSolver(uint16_t number_of_unknowns);
        ~MatrixSolver();

    private:

};

GeneratedMoves::GeneratedMoves(uint16_t number_of_hexes){
    moves = new uint16_t[number_of_hexes];
    number_of_generated_moves = 0;
}

GeneratedMoves::~GeneratedMoves(){
    delete[] moves;
}

UnionFindData::UnionFindData(uint16_t big_board_size_parameter){
    big_board_size = big_board_size_parameter;
    id_parent = new uint16_t[big_board_size];
    number_of_children = new uint16_t[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        id_parent[i] = i;
        number_of_children[i] = 0;
    }
}

UnionFindData::UnionFindData(const UnionFindData &copied_structure){
    big_board_size = copied_structure.big_board_size;
    id_parent = new uint16_t[big_board_size];
    number_of_children = new uint16_t[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        id_parent[i] = copied_structure.id_parent[i];
        number_of_children[i] = copied_structure.number_of_children[i];
    }
}

UnionFindData::~UnionFindData(){
    delete [] id_parent;
    delete [] number_of_children;
}

bool UnionFindData::are_connected(uint16_t id_a, uint16_t id_b){
    if (find_parent(id_a)==find_parent(id_b)){
        return true;
    } else {
        return false;
    }
}

uint16_t UnionFindData::make_union(uint16_t id_a, uint16_t id_b){
    uint16_t *to_be_compressed;
    //It would be impossible to overflow this length, but who knows.
    to_be_compressed = new uint16_t[big_board_size];
    uint16_t number_of_to_be_compressed;
    //The program search upward the tree to find id_a parent.
    uint16_t actual_node = id_a;
    uint16_t parent_node = id_parent[actual_node];
    to_be_compressed[0] = actual_node;
    number_of_to_be_compressed = 1;
    while (actual_node != parent_node){
        actual_node = parent_node;
        parent_node = id_parent[actual_node];
        to_be_compressed[number_of_to_be_compressed] = actual_node;
        number_of_to_be_compressed++;
    }
    uint16_t parent_a_node = actual_node;
    //The program search upward the tree to find id_b parent.
    actual_node = id_b;
    parent_node = id_parent[actual_node];
    to_be_compressed[number_of_to_be_compressed] = actual_node;
    number_of_to_be_compressed++;
    while (actual_node != parent_node){
        actual_node = parent_node;
        parent_node = id_parent[actual_node];
        to_be_compressed[number_of_to_be_compressed] = actual_node;
        number_of_to_be_compressed++;
    }
    uint16_t parent_b_node = actual_node;
    //Now, we have parent_a_node, parent_b_node, id_a, id_b and a list
    //of nodes to have their paths compressed, which includes parent_a_node
    //and parent_b_node.
    if (parent_a_node==parent_b_node){
        //The nodes a and b are in the same set. We only compress the paths.
        for (uint16_t i = 0; i < number_of_to_be_compressed; i++){
            id_parent[to_be_compressed[i]] = parent_a_node;
        }
    } else {
        //Nodes a and b were in different set. We have to unify the sets.
        //We check victory conditions:
        if (number_of_children[parent_a_node] >= number_of_children[parent_b_node]){
            //Parent_a_node is the new absolute parent.
            number_of_children[parent_a_node] = number_of_children[parent_a_node]
                                                + number_of_children[parent_b_node] + 1;
            number_of_children[parent_b_node] = 0;
            //We make all previous nodes connected directly with parent_a.
            for (uint16_t i = 0; i < number_of_to_be_compressed; i++){
                id_parent[to_be_compressed[i]] = parent_a_node;
            }
        } else {
            //Parent_b_node is the new absolute parent.
            number_of_children[parent_b_node] = number_of_children[parent_a_node]
                                                + number_of_children[parent_b_node] + 1;
            number_of_children[parent_a_node] = 0;
            //We make all previous nodes connected directly with parent_a.
            for (uint16_t i = 0; i < number_of_to_be_compressed; i++){
                id_parent[to_be_compressed[i]] = parent_b_node;
            }
        }
    }
    delete [] to_be_compressed;
    return 0;
}

uint16_t UnionFindData::find_parent(uint16_t id_a){
    uint16_t *to_be_compressed;
    //It would be impossible to overflow this length, but who knows.
    to_be_compressed = new uint16_t[big_board_size];
    uint16_t number_of_to_be_compressed;
    //The program search upward the tree to find id_a parent.
    uint16_t actual_node = id_a;
    uint16_t parent_node = id_parent[actual_node];
    to_be_compressed[0] = actual_node;
    number_of_to_be_compressed = 1;
    while (actual_node != parent_node){
        actual_node = parent_node;
        parent_node = id_parent[actual_node];
        to_be_compressed[number_of_to_be_compressed] = actual_node;
        number_of_to_be_compressed++;
    }
    uint16_t parent_a_node = actual_node;
    for (uint16_t i = 0; i < number_of_to_be_compressed; i++){
        id_parent[to_be_compressed[i]] = parent_a_node;
    }
    delete []to_be_compressed;
    return parent_a_node;
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
}

uint64_t HexBoard::get_zobrist_hash(){
    return zobrist_hash;
}

uint16_t HexBoard::make_move(uint16_t move_made){
    zobrist_hash = zobrist_hash ^ zobrist_const[move_made][(player_to_move+1)/2];
    big_board[move_made]=player_to_move;
    player_to_move *= -1;
    number_of_moves_maked++;
    return 0;
}

uint16_t HexBoard::unmake_move(uint16_t move_to_be_unmade){
    zobrist_hash = zobrist_hash ^ zobrist_const[move_to_be_unmade][(big_board[move_to_be_unmade]+1)/2];
    big_board[move_to_be_unmade]=0;
    player_to_move *= -1;
    number_of_moves_maked--;
    return 0;
}

uint16_t HexBoard::reset_the_board(){
    for (uint16_t i = 0; i < number_of_hexes; i++){
        big_board[mailbox_to_big_board[i]]=0;
    }
    player_to_move = -1;
    zobrist_hash = zobrist_const_empty_board;
    number_of_moves_maked = 0;
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
    float current_flux_for_blue = evaluation_from_blue_perspective();
    //Makes evaluation from red.
    float current_flux_for_red = evaluation_from_red_perspective();
    //Makes evaluation from blue perspective.
    float evaluation_score = current_flux_for_blue-current_flux_for_red;
//    std::cout << "---------------------------------" << std::endl;
//    std::cout << "Corriente azul: " << current_flux_for_blue << std::endl;
//    std::cout << "Corriente roja: " << current_flux_for_red << std::endl;
//    std::cout << "Diferencia: " << evaluation_score << std::endl;
    evaluation_score = std::exp(evaluation_score/2.0F);
    evaluation_score = (evaluation_score-(1/evaluation_score))/(evaluation_score+(1/evaluation_score));
//    std::cout << "Resultado de evaluacion: " << evaluation_score << std::endl;
    int16_t return_variable = int16_t(evaluation_score*1000.0F);
    //Negamax implementation.
    return return_variable*int16_t(player_to_move);
}

float HexBoard::evaluation_from_blue_perspective(){
    int8_t *voltage_assigned = new int8_t[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        voltage_assigned[i] = 0;
    }
    for (uint16_t i = 0; i < side_size; i++){
        voltage_assigned[blue_connected_alpha[i]] = 1;
        voltage_assigned[blue_connected_beta[i]] = -1;
    }
    uint16_t number_of_unknowns = (side_size-2)*side_size;
    float **matrix = new float*[number_of_unknowns];
    for(uint16_t i = 0; i < number_of_unknowns; i++){
        matrix[i] = new float[number_of_unknowns+1];
        for (uint16_t j = 0; j < number_of_unknowns+1; j++){
            matrix[i][j] = 0.0F;
        }
    }
    uint16_t *mailbox_to_matrix = new uint16_t[big_board_size];
    for (uint16_t i = 0; i < side_size; i++){
        for (uint16_t j = 0; j < (side_size-2); j++){
            mailbox_to_matrix[(i+1)*(side_size+2)+(j+2)] = i*(side_size-2)+j;
        }
    }
    //The matrix filler cycle began:
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t actual_board_position = mailbox_to_big_board[i];
        if (voltage_assigned[actual_board_position]==0){
            uint16_t actual_matrix_position = mailbox_to_matrix[actual_board_position];
            for (uint16_t j = 0; j < 6; j++){
                uint16_t adjacent_board_position;
                switch (j){
                case 0:
                    adjacent_board_position = actual_board_position+1;
                    break;
                case 1:
                    adjacent_board_position = actual_board_position+(side_size+2);
                    break;
                case 2:
                    adjacent_board_position = actual_board_position+(side_size+1);
                    break;
                case 3:
                    adjacent_board_position = actual_board_position-1;
                    break;
                case 4:
                    adjacent_board_position = actual_board_position-(side_size+2);
                    break;
                case 5:
                    adjacent_board_position = actual_board_position-(side_size+1);
                    break;
                }
                if (big_board[adjacent_board_position] != 2){
                    float total_resistence = 0.0F;
                    //Code dependent of the blue player.
                    if (big_board[actual_board_position]==0){
                        total_resistence += 0.5F;
                    }
                    if (big_board[actual_board_position]==1){
                        total_resistence += 0.005F;
                    }
                    if (big_board[actual_board_position]==-1){
                        total_resistence += 50.0F;
                    }
                    if (big_board[adjacent_board_position]==0){
                        total_resistence += 0.5F;
                    }
                    if (big_board[adjacent_board_position]==1){
                        total_resistence += 0.005F;
                    }
                    if (big_board[adjacent_board_position]==-1){
                        total_resistence += 50.0F;
                    }
                    float inverse_total_resistence = 1.0F/total_resistence;
                    matrix[actual_matrix_position][actual_matrix_position] += inverse_total_resistence;
                    if (voltage_assigned[adjacent_board_position]==0){
                        matrix[actual_matrix_position][mailbox_to_matrix[adjacent_board_position]] -= inverse_total_resistence;
                    }
                    if (voltage_assigned[adjacent_board_position]==1){
                        matrix[actual_matrix_position][number_of_unknowns]+=inverse_total_resistence;
                    }
                    if (voltage_assigned[adjacent_board_position]==-1){
                        matrix[actual_matrix_position][number_of_unknowns]-=inverse_total_resistence;
                    }
                }
            }
        }
    }
    solve_matrix(matrix, number_of_unknowns, side_size-2);
    //Calculation of total current flux.
    //Calculation of current in the alpha side.
    float entering_current = 0.0F;
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_board_position = blue_connected_alpha[i];
        for (uint16_t j = 0; j < 2; j++){
            uint16_t adjacent_board_position;
            switch (j){
            case 0:
                adjacent_board_position = actual_board_position-(side_size+1);
                break;
            case 1:
                adjacent_board_position = actual_board_position+1;;
                break;
            }
            if (big_board[adjacent_board_position] != 2){
                float total_resistence = 0.0F;
                //Code dependent of the blue player.
                if (big_board[actual_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[actual_board_position]==1){
                    total_resistence += 0.005F;
                }
                if (big_board[actual_board_position]==-1){
                    total_resistence += 50.0F;
                }
                if (big_board[adjacent_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[adjacent_board_position]==1){
                    total_resistence += 0.005F;
                }
                if (big_board[adjacent_board_position]==-1){
                    total_resistence += 50.0F;
                }
                float inverse_total_resistence = 1.0F/total_resistence;
                entering_current += (1.0F-matrix[mailbox_to_matrix[adjacent_board_position]][number_of_unknowns])*inverse_total_resistence;
            }
        }
    }
    //Calculation of current in the beta side.
    float exiting_current = 0.0F;
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_board_position = blue_connected_beta[i];
        for (uint16_t j = 0; j < 2; j++){
            uint16_t adjacent_board_position;
            switch (j){
            case 0:
                adjacent_board_position = actual_board_position+(side_size+1);
                break;
            case 1:
                adjacent_board_position = actual_board_position-1;;
                break;
            }
            if (big_board[adjacent_board_position] != 2){
                float total_resistence = 0.0F;
                //Code dependent of the blue player.
                if (big_board[actual_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[actual_board_position]==1){
                    total_resistence += 0.005F;
                }
                if (big_board[actual_board_position]==-1){
                    total_resistence += 50.0F;
                }
                if (big_board[adjacent_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[adjacent_board_position]==1){
                    total_resistence += 0.005F;
                }
                if (big_board[adjacent_board_position]==-1){
                    total_resistence += 50.0F;
                }
                float inverse_total_resistence = 1.0F/total_resistence;
                exiting_current += (matrix[mailbox_to_matrix[adjacent_board_position]][number_of_unknowns]+1.0F)*inverse_total_resistence;
            }
        }
    }
    //And the average of exiting and entering current
    float total_current_flux_for_blue = (exiting_current+entering_current)/2;
    delete []voltage_assigned;
    delete []mailbox_to_matrix;
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        delete [] matrix[i];
    }
    delete []matrix;
    return total_current_flux_for_blue;
}

float HexBoard::evaluation_from_red_perspective(){
    int8_t *voltage_assigned = new int8_t[big_board_size];
    for (uint16_t i = 0; i < big_board_size; i++){
        voltage_assigned[i] = 0;
    }
    for (uint16_t i = 0; i < side_size; i++){
        voltage_assigned[red_connected_alpha[i]] = 1;
        voltage_assigned[red_connected_beta[i]] = -1;
    }
    uint16_t number_of_unknowns = (side_size-2)*side_size;
    float **matrix = new float*[number_of_unknowns];
    for(uint16_t i = 0; i < number_of_unknowns; i++){
        matrix[i] = new float[number_of_unknowns+1];
        for (uint16_t j = 0; j < number_of_unknowns+1; j++){
            matrix[i][j] = 0.0F;
        }
    }
    uint16_t *mailbox_to_matrix = new uint16_t[big_board_size];
    for (uint16_t i = 0; i < (side_size-2); i++){
        for (uint16_t j = 0; j < side_size; j++){
            mailbox_to_matrix[(i+2)*(side_size+2)+(j+1)] = i*side_size+j;
        }
    }
    //The matrix filler cycle began:
    for (uint16_t i = 0; i < number_of_hexes; i++){
        uint16_t actual_board_position = mailbox_to_big_board[i];
        if (voltage_assigned[actual_board_position]==0){
            uint16_t actual_matrix_position = mailbox_to_matrix[actual_board_position];
            for (uint16_t j = 0; j < 6; j++){
                uint16_t adjacent_board_position;
                switch (j){
                case 0:
                    adjacent_board_position = actual_board_position+1;
                    break;
                case 1:
                    adjacent_board_position = actual_board_position+(side_size+2);
                    break;
                case 2:
                    adjacent_board_position = actual_board_position+(side_size+1);
                    break;
                case 3:
                    adjacent_board_position = actual_board_position-1;
                    break;
                case 4:
                    adjacent_board_position = actual_board_position-(side_size+2);
                    break;
                case 5:
                    adjacent_board_position = actual_board_position-(side_size+1);
                    break;
                }
                if (big_board[adjacent_board_position] != 2){
                    float total_resistence = 0.0F;
                    //Code dependent of the red player.
                    if (big_board[actual_board_position]==0){
                        total_resistence += 0.5F;
                    }
                    if (big_board[actual_board_position]==1){
                        total_resistence += 50.0F;
                    }
                    if (big_board[actual_board_position]==-1){
                        total_resistence += 0.005F;
                    }
                    if (big_board[adjacent_board_position]==0){
                        total_resistence += 0.5F;
                    }
                    if (big_board[adjacent_board_position]==1){
                        total_resistence += 50.0F;
                    }
                    if (big_board[adjacent_board_position]==-1){
                        total_resistence += 0.005F;
                    }
                    float inverse_total_resistence = 1.0F/total_resistence;
                    matrix[actual_matrix_position][actual_matrix_position] += inverse_total_resistence;
                    if (voltage_assigned[adjacent_board_position]==0){
                        matrix[actual_matrix_position][mailbox_to_matrix[adjacent_board_position]] -= inverse_total_resistence;
                    }
                    if (voltage_assigned[adjacent_board_position]==1){
                        matrix[actual_matrix_position][number_of_unknowns]+=inverse_total_resistence;
                    }
                    if (voltage_assigned[adjacent_board_position]==-1){
                        matrix[actual_matrix_position][number_of_unknowns]-=inverse_total_resistence;
                    }
                }
            }
        }
    }
    solve_matrix(matrix, number_of_unknowns, side_size);
    //Calculation of total current flux.
    //Calculation of current in the alpha side.
    float entering_current = 0.0F;
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_board_position = red_connected_alpha[i];
        for (uint16_t j = 0; j < 2; j++){
            uint16_t adjacent_board_position;
            switch (j){
            case 0:
                adjacent_board_position = actual_board_position+(side_size+1);
                break;
            case 1:
                adjacent_board_position = actual_board_position+(side_size+2);
                break;
            }
            if (big_board[adjacent_board_position] != 2){
                float total_resistence = 0.0F;
                //Code dependent of the red player.
                if (big_board[actual_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[actual_board_position]==1){
                    total_resistence += 50.0F;
                }
                if (big_board[actual_board_position]==-1){
                    total_resistence += 0.005F;
                }
                if (big_board[adjacent_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[adjacent_board_position]==1){
                    total_resistence += 50.0F;
                }
                if (big_board[adjacent_board_position]==-1){
                    total_resistence += 0.005F;
                }
                float inverse_total_resistence = 1.0F/total_resistence;
                entering_current += (1.0F-matrix[mailbox_to_matrix[adjacent_board_position]][number_of_unknowns])*inverse_total_resistence;
            }
        }
    }
    //Calculation of current in the beta side.
    float exiting_current = 0.0F;
    for (uint16_t i = 0; i < side_size; i++){
        uint16_t actual_board_position = red_connected_beta[i];
        for (uint16_t j = 0; j < 2; j++){
            uint16_t adjacent_board_position;
            switch (j){
            case 0:
                adjacent_board_position = actual_board_position-(side_size+1);
                break;
            case 1:
                adjacent_board_position = actual_board_position-(side_size+2);;
                break;
            }
            if (big_board[adjacent_board_position] != 2){
                float total_resistence = 0.0F;
                //Code dependent of the red player.
                if (big_board[actual_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[actual_board_position]==1){
                    total_resistence += 50.0F;
                }
                if (big_board[actual_board_position]==-1){
                    total_resistence += 0.005F;
                }
                if (big_board[adjacent_board_position]==0){
                    total_resistence += 0.5F;
                }
                if (big_board[adjacent_board_position]==1){
                    total_resistence += 50.0F;
                }
                if (big_board[adjacent_board_position]==-1){
                    total_resistence += 0.005F;
                }
                float inverse_total_resistence = 1.0F/total_resistence;
                exiting_current += (matrix[mailbox_to_matrix[adjacent_board_position]][number_of_unknowns]+1.0F)*inverse_total_resistence;
            }
        }
    }
    //And the average of exiting and entering current
    float total_current_flux_for_red = (exiting_current+entering_current)/2;
    delete []voltage_assigned;
    delete []mailbox_to_matrix;
    for (uint16_t i = 0; i < number_of_unknowns; i++){
        delete [] matrix[i];
    }
    delete []matrix;
    return total_current_flux_for_red;
}

uint16_t solve_matrix(float **matrix, uint16_t total_number_of_unknowns, uint16_t central_diagonal_width){
    for(uint16_t i = 0; i < total_number_of_unknowns; i++){
        std::cout << "[" << matrix[i][0] ;
        for(uint16_t j=1; j<total_number_of_unknowns; j++){
            std::cout << "," << matrix[i][j] ;
        }
        std::cout << "],";
    }
    std::cout << std::endl;
    std::cout << "[";
    for(uint16_t i = 0; i < total_number_of_unknowns; i++){
        std::cout << matrix[i][total_number_of_unknowns] << ",";
    }
    std::cout << std::endl;
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

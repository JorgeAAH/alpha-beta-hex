#ifndef INCREMENTALEVALUATIONMATRIX_H
#define INCREMENTALEVALUATIONMATRIX_H

#include <cstdint>

class SparseMatrix;

class IncrementalEvaluationMatrix {
    public:
        IncrementalEvaluationMatrix(uint16_t side_size_parameter, uint16_t big_board_size, int8_t player_evaluated_parameter, int8_t *big_board, uint16_t *mailbox_to_big_board, uint16_t *connected_alpha, uint16_t *connected_beta);
        ~IncrementalEvaluationMatrix();
        uint16_t set_hex_to_board(int8_t *big_board, uint16_t hex_setted);
        float evaluate();

    private:
        uint16_t side_size;
        int8_t player_evaluated; //1 for blue perspective, -1 for red perspective.
        int8_t *voltage_assigned;
        uint16_t *mailbox_to_matrix;
        uint16_t number_of_unknowns;
        float *b_column_matrix;
        SparseMatrix *matrix;
};

#endif // INCREMENTALEVALUATIONMATRIX_H

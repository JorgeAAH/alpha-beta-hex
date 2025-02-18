#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include <cstdint>

class SparseMatrix {
    public:
        SparseMatrix (uint16_t number_of_unknowns_parameter);
        ~SparseMatrix ();
        uint16_t assignement(uint16_t row, uint16_t column, float assignement_value);
        uint16_t update_row_diagonal(uint16_t row, float b_column_row_value);
        float *solve_matrix(float *b_column_matrix);
        float *solve_matrix_gauss(float *column_matrix, uint16_t central_diagonal_width);

    private:
        float *multiply_sparse_matrix_with_column_matrix(float *column_matrix);
        bool does_plain_matrix_exists;
        float **plain_matrix;
        uint16_t number_of_unknowns;
        uint16_t *row_occupation;
        uint16_t **matrix_column_positions;
        float **matrix_values;
};

#endif // SPARSEMATRIX_H

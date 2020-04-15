#ifndef HEXBOARD_H
#define HEXBOARD_H

#include <cstdint>

struct GeneratedMoves {
    uint16_t *moves;
    uint16_t number_of_generated_moves;
    GeneratedMoves(uint16_t number_of_hexes);
    ~GeneratedMoves();
};

class HexBoard {
    public:
        HexBoard(uint16_t side_size);
        ~HexBoard();
        uint64_t get_zobrist_hash();
        uint16_t make_move(uint16_t move_made);
        uint16_t unmake_move(uint16_t move_to_be_unmade);
        uint16_t reset_the_board();
        int8_t get_player_to_move();
        int8_t check_winning_conditions(); //Return 1 for blue has won, -1 for red, 0 for nobody.
        int16_t evaluate_board();
        GeneratedMoves *generate_possible_moves();

    private:
        float evaluation_from_blue_perspective();
        float evaluation_from_red_perspective();
        uint16_t number_of_hexes;
        uint16_t big_board_size;
        uint16_t side_size;
        uint16_t number_of_moves_maked;
        int8_t player_to_move;
        //In big_board 0 is an empty hex, +-1 is an occupied
        //hex and 2 is an outside hex (to detect borders).
        int8_t *big_board;
        uint64_t zobrist_const_empty_board; //Initial hash, hexes are empty.
        uint64_t zobrist_hash; //Actual hash.
        uint16_t *mailbox_to_big_board;
        uint16_t *move_generator_to_big_board;
        uint16_t *blue_connected_alpha;
        uint16_t *blue_connected_beta;
        uint16_t *red_connected_alpha;
        uint16_t *red_connected_beta;
        //IMPORTANT!!! zobrist_const is accessed by moves to big_board, so it have to had
        //big_board size. (Obviously, some random constants will never be used).
        uint64_t **zobrist_const;
};

extern HexBoard engine_board;

#endif // HEXBOARD_H

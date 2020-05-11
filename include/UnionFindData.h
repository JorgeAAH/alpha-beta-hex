#ifndef UNIONFINDDATA_H
#define UNIONFINDDATA_H

#include <cstdint>

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

#endif // UNIONFINDDATA_H

#include "UnionFindData.h"

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

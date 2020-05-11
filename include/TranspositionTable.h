#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include <cstdint>

struct TranspositionEntry;

struct TranspositionData{
    uint16_t best_move;
    int16_t evaluation_value;
    uint8_t residual_depth;
    uint8_t bound;
        //1 from pv-nodes (exact).
        //2 from cut-nodes (lower bound).
        //3 from all-nodes (upper bound).
};

class TranspositionTable
{
    public:
        uint16_t initialize_transposition_table(uint16_t transposition_table_size_power);
        uint16_t release_memory();
        uint16_t new_search();
        bool transposition_table_lookup(uint64_t position_key, TranspositionData &data_returned);
        uint16_t transposition_table_store(uint64_t position_key, TranspositionData data_to_be_stored);
    private:
        TranspositionEntry *array_of_entries;
        uint8_t actual_generation;
        uint16_t table_size_power;
        uint64_t table_size;
};

extern TranspositionTable engine_transposition_table;

//To test 29/03/2020.
extern uint32_t number_of_transposition_table_lookups;
extern uint32_t number_of_transposition_table_hits;
extern uint32_t number_of_transposition_table_stores;
//End test 29/03/2020.

#endif // TRANSPOSITIONTABLE_H

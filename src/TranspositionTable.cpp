#include "TranspositionTable.h"

TranspositionTable engine_transposition_table;

//To test 29/03/2020.
uint32_t number_of_transposition_table_lookups;
uint32_t number_of_transposition_table_hits;
uint32_t number_of_transposition_table_stores;
//End test 29/03/2020.

uint16_t TranspositionTable::initialize_transposition_table(uint16_t transposition_table_size_power){
    table_size_power = transposition_table_size_power;
    table_size = 1ULL;
    table_size = table_size << table_size_power;
    array_of_entries = new TranspositionEntry[table_size];
    actual_generation = 0;
    for (uint64_t i = 0ULL; i < table_size; i++){
        array_of_entries[i].generation = actual_generation;
    }
    return 0;
}

uint16_t TranspositionTable::release_memory(){
    delete []array_of_entries;
    return 0;
}

uint16_t TranspositionTable::new_search(){
    actual_generation++;
    return 0;
}

bool TranspositionTable::transposition_table_lookup(uint64_t position_key, TranspositionData &data_returned){
    //To test 29/03/2020.
    number_of_transposition_table_lookups++;
    //End test 29/03/2020.
    uint64_t lookup_index = position_key >> (64-table_size_power);
    uint32_t lookup_key = uint32_t(position_key & 0xFFFFFFFF);
    if (array_of_entries[lookup_index].position_key == lookup_key){
        //To test 29/03/2020.
        number_of_transposition_table_hits++;
        //End test 29/03/2020.
        data_returned = array_of_entries[lookup_index].position_data;
        array_of_entries[lookup_index].generation = actual_generation;
        return true;
    } else {
        return false;
    }
    return 0;
}

uint16_t TranspositionTable::transposition_table_store(uint64_t position_key, TranspositionData data_to_be_stored){
    //To test 29/03/2020.
    number_of_transposition_table_stores++;
    //End test 29/03/2020.
    //It is a better approach to assign a score to the to entries, because a if structure is too clumsy.
    uint64_t store_index = position_key >> (64-table_size_power);
    uint32_t store_key = uint32_t(position_key & 0xFFFFFFFF);
    //Generation 0 (empty entries) are always substituted.
    if (0 == array_of_entries[store_index].generation){
        //We overwrite new data, as the entry was empty.
        array_of_entries[store_index].position_key = store_key;
        array_of_entries[store_index].generation = actual_generation;
        array_of_entries[store_index].position_data = data_to_be_stored;
        return 0;
    }
    if (store_key == array_of_entries[store_index].position_key){
        //The stored data is not irrelevant to actual_generation so that flag is updated.
        array_of_entries[store_index].generation = actual_generation;
    }
    //Using generation as replacement strategy starts here.
    if (actual_generation > array_of_entries[store_index].generation){
        //The stored data is probably irrelevant so the entry is overwritten.
        array_of_entries[store_index].position_key = store_key;
        array_of_entries[store_index].generation = actual_generation;
        array_of_entries[store_index].position_data = data_to_be_stored;
        return 0;
    }
    //Using depth searched as replacement strategy starts here.
    if (data_to_be_stored.residual_depth > array_of_entries[store_index].position_data.residual_depth){
        //The stored data was searched to shallow depth, so the entry is overwritten.
        array_of_entries[store_index].position_key = store_key;
        array_of_entries[store_index].generation = actual_generation;
        array_of_entries[store_index].position_data = data_to_be_stored;
        return 0;
    }
    if (data_to_be_stored.residual_depth < array_of_entries[store_index].position_data.residual_depth){
        //The new data was searched to shallow depth, so the entry is not overwritten.
        return 0;
    }
    //Using bound type as replacement strategy starts here.
    if (data_to_be_stored.bound < array_of_entries[store_index].position_data.bound){
        //The new data has a better bound so the entry is overwritten.
        array_of_entries[store_index].position_key = store_key;
        array_of_entries[store_index].generation = actual_generation;
        array_of_entries[store_index].position_data = data_to_be_stored;
        return 0;
    }
    if (data_to_be_stored.bound > array_of_entries[store_index].position_data.bound){
        //The new data has a worse bound so the entry is not overwritten.
        return 0;
    }
    //Using evaluation value as replacement strategy starts here.
    if (2 == data_to_be_stored.bound){
        if (data_to_be_stored.evaluation_value > array_of_entries[store_index].position_data.evaluation_value){
            //The lower bound is improved in the new data so the entry is overwritten.
            array_of_entries[store_index].position_key = store_key;
            array_of_entries[store_index].generation = actual_generation;
            array_of_entries[store_index].position_data = data_to_be_stored;
        }
        return 0;
    }
    if (3 == data_to_be_stored.bound){
        if (data_to_be_stored.evaluation_value < array_of_entries[store_index].position_data.evaluation_value){
            //The upper bound is improved in the new data so the entry is overwritten.
            array_of_entries[store_index].position_key = store_key;
            array_of_entries[store_index].generation = actual_generation;
            array_of_entries[store_index].position_data = data_to_be_stored;
        }
        return 0;
    }
    return 0;
}

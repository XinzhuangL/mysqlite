#ifndef COMM_H
#define COMM_H

#include "statement.h"


typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);

void print_constants();

void print_leaf_node(void* node);

void indent(uint32_t level);

void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level);

#endif
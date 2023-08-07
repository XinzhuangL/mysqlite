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

#endif
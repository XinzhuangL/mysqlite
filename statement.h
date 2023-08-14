#ifndef STATEMENT_H
#define STATEMENT_H


#include "input.h"
#include "table.h"



typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
} PrepareResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_DUPLICATE_KEY
} ExecuteResult;

// 校验输入的类型，只支持select 和 insert insert时将语句内容写入statement
PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement * statement);

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);

ExecuteResult execute_statement(Statement *statement, Table *table);

ExecuteResult execute_insert(Statement *statement, Table *table);

ExecuteResult execute_select(Statement *statement, Table * table);


#endif





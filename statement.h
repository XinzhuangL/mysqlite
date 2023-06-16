#ifndef STATEMENT_H
#define STATEMENT_H


#include "input.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0) -> Attribute)
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
// 表容纳的最大page数
#define TABLE_MAX_PAGES 100
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;





typedef struct {
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

void serialize_row(Row *source, void *destination);

void deserialize_row(void *source, Row *destination);

void *row_slot(Table *table, uint32_t row_num);

void print_row(Row *row);

void free_table(Table *table);

Table *new_table();

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
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

// 校验输入的类型，只支持select 和 insert insert时将语句内容写入statement
PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement * statement);

ExecuteResult execute_statement(Statement *statement, Table *table);

ExecuteResult execute_insert(Statement *statement, Table *table);

ExecuteResult execute_select(Statement *statement, Table * table);


#endif





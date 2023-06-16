#include "statement.h"
#include <string.h>
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
// page大小
const uint32_t PAGE_SIZE = 4096;
// page行数
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
// 表总行数
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement *statement) {
    if(strncmp(input_buffer -> buffer, "insert", 6) == 0) {
        statement -> type = STATEMENT_INSERT;
        int args_assigned = sscanf(
                input_buffer -> buffer,
                "insert %d %s %s",
                &(statement -> row_to_insert.id),
                statement -> row_to_insert.username,
                statement -> row_to_insert.email
                );
        if(args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if(strcmp(input_buffer -> buffer, "select") == 0) {
        statement -> type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
    switch (statement -> type) {
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(statement, table);
    }
}

ExecuteResult execute_insert(Statement *statement, Table *table) {
    if(table -> num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Row *row_to_insert = &(statement -> row_to_insert);
    serialize_row(row_to_insert, row_slot(table, table ->num_rows));
    table -> num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table * table) {
    Row row;
    for(uint32_t i = 0; i < table -> num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}



void serialize_row(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &(source -> id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source -> username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source -> email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination -> id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination -> username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination -> email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

// 计算插入位置
void *row_slot(Table *table, uint32_t row_num) {
    // 计算当前到那个page
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table -> pages[page_num];
    if(page == NULL) {
        // 新page分配内存
        page = table -> pages[page_num] = malloc(PAGE_SIZE);
    }
    // 计算到哪行
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    // 计算到那个bytes
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void print_row(Row *row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void free_table(Table *table) {
    for (int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}

Table *new_table() {
    Table *table = (Table *) malloc(sizeof(Table));
    table -> num_rows = 0;
    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table -> pages[i] = NULL;
    }
    return table;
}
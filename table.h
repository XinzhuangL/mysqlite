#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/errno.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0) -> Attribute)
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
// 表容纳的最大page数

#define TABLE_MAX_PAGES 100

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;


typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;


typedef struct {
    uint32_t num_rows;
//    void *pages[TABLE_MAX_PAGES];
    Pager* pager;
} Table;

extern const uint32_t ID_SIZE ;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET ;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;
// page大小
extern const uint32_t PAGE_SIZE;
// page行数
extern const uint32_t ROWS_PER_PAGE;
// 表总行数
extern const uint32_t TABLE_MAX_ROWS;

Table* db_open(const char* filename);
void db_close(Table* table);

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);

void* get_page(Pager* pager, uint32_t page_num);
Pager* pager_open(const char* filename);

void serialize_row(Row *source, void *destination);

void deserialize_row(void *source, Row *destination);

void *row_slot(Table *table, uint32_t row_num);

void print_row(Row *row);

void free_table(Table *table);

Table *new_table();


#endif
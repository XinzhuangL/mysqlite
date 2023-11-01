#ifndef TABLE_H
#define TABLE_H

#include "tree_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/errno.h>


typedef struct {
    uint32_t num_pages;
    int file_descriptor;
    uint32_t file_length;
    void *pages[TABLE_MAX_PAGES];
} Pager;


typedef struct {
//    uint32_t num_rows;
//    void *pages[TABLE_MAX_PAGES];
    Pager *pager;
    uint32_t root_page_num;  // 根节点在哪个page
} Table;


typedef struct {
    Table *table;
//    uint32_t row_num;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;  // Indicates a position one past the last element
} Cursor;


Table *db_open(const char *filename);

void db_close(Table *table);

//void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);
void pager_flush(Pager *pager, uint32_t page_num);

void *get_page(Pager *pager, uint32_t page_num);

Pager *pager_open(const char *filename);

void serialize_row(Row *source, void *destination);

void deserialize_row(void *source, Row *destination);

//void *row_slot(Table *table, uint32_t row_num);
// returns a pointer to the position described by thee cursor
void *cursor_value(Cursor *cursor);

void cursor_advance(Cursor *cursor);

void print_row(Row *row);

//void free_table(Table *table);
//
//Table *new_table();

Cursor *table_start(Table *table);

//Cursor *table_end(Table *table);

Cursor* table_find(Table* table, uint32_t key);




void leaf_node_insert(Cursor *cursor, uint32_t key, Row *row);

Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);

void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);

/*
 * Until we start recycling free pages, new pages will always
 * go onto the end of the database file
 */
uint32_t get_unused_page_num(Pager* pager);

void create_new_root(Table* table, uint32_t right_child_page_num);

// internal_node_find
Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key);


#endif
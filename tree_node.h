//
// Created by lixinzhuang1 on 2023/8/2.
//

#ifndef MYSQLITE_TREE_NODE_H
#define MYSQLITE_TREE_NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/errno.h>


typedef enum {
    NODE_INTERNAL,
    NODE_LEAF
} NodeType;

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0) -> Attribute)
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
// 表容纳的最大page数

#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

/**
 * Move from table.c
 */
extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;
// page大小
extern const uint32_t PAGE_SIZE;
// page行数
extern const uint32_t ROWS_PER_PAGE;
// 表总行数
extern const uint32_t TABLE_MAX_ROWS;

/*
 * Common Node Header Layout
 */

extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint8_t COMMON_NODE_HEADER_SIZE;


/**
* Leaf Node Header Layout
*/
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

/**
* Leaf Node Body Layout
*/
extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;

extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;


extern const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT;
extern const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT;

/*
 * Internal Node Header Layout
 * 最右边孩子的指针被存储在头结点
 */
extern const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE;

/**
 *
 *  Internal Node Body Layout
 */

extern const uint32_t INTERNAL_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_CELL_SIZE;


uint32_t *leaf_node_num_cells(void *node);

void *leaf_node_cell(void *node, uint32_t cell_num);

uint32_t *leaf_node_key(void *node, uint32_t cell_num);

void *leaf_node_value(void *node, uint32_t cell_num);

void initialize_leaf_node(void *node);

void initialize_internal_node(void* node);

NodeType get_node_type(void* node);

void set_node_type(void* node, NodeType type);

bool is_node_root(void* node);

void set_node_root(void* node, bool is_root);

uint32_t* internal_node_num_keys(void* node);

uint32_t* internal_node_right_child(void* node);

// 获取第cell_num 个cell_num cell = child + key
uint32_t* internal_node_cell(void* node, uint32_t cell_num);

// 获取第child_num个孩子
uint32_t* internal_node_child(void* node, uint32_t child_num);

// 最右边的key
uint32_t get_node_max_key(void* node);

// 获取第num个key
uint32_t* internal_node_key(void* node, uint32_t key_num);





#endif //MYSQLITE_TREE_NODE_H

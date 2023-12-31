


#include "table.h"


void serialize_row(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

Pager *pager_open(const char *filename) {
    int fd = open(filename,
                  O_RDWR | O_CREAT,  // 读写模式,创建文件如果不存在
                  S_IWUSR | S_IRUSR  // 用户读写权限
    );
    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }
    off_t file_length = lseek(fd, 0, SEEK_END);
    Pager *pager = malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;

    pager->num_pages = (file_length / PAGE_SIZE);

    if (file_length % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

Table *db_open(const char *filename) {
    Pager *pager = pager_open(filename);
//    uint32_t num_rows = pager->file_length / ROW_SIZE;

    Table *table = malloc(sizeof(Table));
    table->pager = pager;
//    table->num_rows = num_rows;
    table->root_page_num = 0;
    if (pager->num_pages == 0) {
        // New database file. Initialize page 0 as leaf node
        void *root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
    }
    return table;

}

void *get_page(Pager *pager, uint32_t page_num) {
    // todo page_num == TABLE_MAX_PAGES
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }
    // cache miss 从文件读取
    if (pager->pages[page_num] == NULL) {
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;
        // 不能整除则说明还有不能填满一个page的数据
        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }
        // 内存中不命中 从file中读取
        if (page_num < num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d \n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
        if (page_num >= pager->num_pages) {
            pager->num_pages = page_num + 1;
        }
    }
    return pager->pages[page_num];
}

void db_close(Table *table) {
    Pager *pager = table->pager;
//    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

//    for (uint32_t i = 0; i < num_full_pages; i++) {
    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }
    // 多出来的行
//    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
//    if (num_additional_rows > 0) {
//        uint32_t page_num = num_full_pages;
//        if (pager->pages[page_num] != NULL) {
//            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
//            free(pager->pages[page_num]);
//            pager->pages[page_num] = NULL;
//        }
//    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}
// 计算插入位置
//void *row_slot(Table *table, uint32_t row_num) {
//    // 计算当前到那个page
//    uint32_t page_num = row_num / ROWS_PER_PAGE;
////    void *page = table -> pages[page_num];
////    if(page == NULL) {
////        // 新page分配内存
////        page = table -> pages[page_num] = malloc(PAGE_SIZE);
////    }
//    void* page = get_page(table -> pager, page_num);
//    // 计算到哪行
//    uint32_t row_offset = row_num % ROWS_PER_PAGE;
//    // 计算到那个bytes
//    uint32_t byte_offset = row_offset * ROW_SIZE;
//    return page + byte_offset;
//}

void *cursor_value(Cursor *cursor) {
//    uint32_t row_num = cursor->row_num;
//    uint32_t page_num = row_num / ROWS_PER_PAGE;
    uint32_t page_num = cursor->page_num;
    void *page = get_page(cursor->table->pager, page_num);
//    uint32_t row_offset = row_num % ROWS_PER_PAGE;
//    uint32_t byte_offset = row_offset * ROW_SIZE;
//    return page + byte_offset;
    return leaf_node_value(page, cursor->cell_num);
}

void cursor_advance(Cursor *cursor) {
//    cursor->row_num += 1;
//    if (cursor->row_num >= cursor->table->num_rows) {
//        cursor->end_of_table = true;
//    }
    uint32_t page_num = cursor->page_num;
    void *node = get_page(cursor->table->pager, page_num);
    cursor->cell_num += 1;
    if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
        cursor->end_of_table = true;
    }

}

void print_row(Row *row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

//void free_table(Table *table) {
//    for (int i = 0; table->pages[i]; i++) {
//        free(table->pages[i]);
//    }
//    free(table);
//}
//
//Table *new_table() {
//    Table *table = (Table *) malloc(sizeof(Table));
//    table -> num_rows = 0;
//    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
//        table -> pages[i] = NULL;
//    }
//    return table;
//}

//void pager_flush(Pager *pager, uint32_t page_num, uint32_t size) {
void pager_flush(Pager *pager, uint32_t page_num) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }
    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }
//    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);

    if (bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}


Cursor *table_start(Table *table) {
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
//    cursor->row_num = 0;
//    cursor->end_of_table = (table->num_rows == 0);
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;
    void *root_node = get_page(table->pager, table->root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);
    cursor->end_of_table = (num_cells == 0);
    return cursor;
}

//Cursor *table_end(Table *table) {
//    Cursor *cursor = malloc(sizeof(Cursor));
//    cursor->table = table;
////    cursor->row_num = table->num_rows;
//    cursor->page_num = table->root_page_num;
//    void *root_node = get_page(table->pager, table->root_page_num);
//    uint32_t num_cells = *leaf_node_num_cells(root_node);
//    cursor->cell_num = num_cells;
//    cursor->end_of_table = true;
//    return cursor;
//}

/**
 * Return the position of the given key.
 * If the key is not present, return the position
 * where it should be inserted
 *
 *
 * @param table
 * @param key
 * @return
 */
Cursor* table_find(Table* table, uint32_t key) {
    uint32_t root_page_num = table -> root_page_num;
    void* root_node = get_page(table -> pager, root_page_num);
    if(get_node_type(root_node) == NODE_LEAF) {
        return leaf_node_find(table, root_page_num, key);
    }  else {
//        printf("Need to implement searching an internal node");
//        exit(EXIT_FAILURE);
        return internal_node_find(table, root_page_num, key);
    }

}

void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = get_page(cursor -> table -> pager, cursor -> page_num);
    // leaf nums
    uint32_t num_cells = *leaf_node_num_cells(node);
    if(num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node full
//        printf("Need to implement splitting a leaf node. \n");
//        exit(EXIT_FAILURE);
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }
    if(cursor -> cell_num < num_cells) {
        // Make room for new cell
        for(uint32_t i = num_cells; i > cursor -> cell_num; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }
    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor -> cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cursor -> cell_num));
}

Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key) {
    void* node = get_page(table -> pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor -> table = table;
    cursor -> page_num = page_num;

    // Binary search
    uint32_t min_index = 0;
    uint32_t  one_past_max_index = num_cells;
    while (one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if(key == key_at_index) {
            cursor -> cell_num = index;
            return cursor;
        }
        if(key < key_at_index) {
            one_past_max_index = index;
        } else {
            min_index = index + 1;
        }
    }
    cursor -> cell_num = min_index;
    return cursor;
}

// 返回未使用的下一个page，即未当前page的个数
uint32_t get_unused_page_num(Pager* pager) {
    return pager -> num_pages;
}

void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value) {
    /*
     * create a new node and move half the cells over.
     * Insert the new value in one of the two nodes
     * Update parent or create a new parent
     */
    void* old_node = get_page(cursor -> table -> pager, cursor -> page_num);
    uint32_t  new_page_num = get_unused_page_num(cursor -> table -> pager);
    void* new_node = get_page(cursor -> table -> pager, new_page_num);
    initialize_leaf_node(new_node);
    /**
     * All existing keys plus new key should be divided
     * evenly between old(left) and new (right) nodes
     * Starting from the right, move each key to correct position
     */
     // 从右向左
     for(int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
         void* destination_node;
         // 向右边分配
         if(i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
             destination_node = new_node;
         } else {
             destination_node = old_node;
         }

         uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
         // 获取指向cell的指针
         void* destination = leaf_node_cell(destination_node, index_within_node);

         // 正好为待插入的值
         // 没看懂
         if(i == cursor -> cell_num) {
             serialize_row(value, destination);
         } else if(i > cursor -> cell_num) {
             memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
         } else {
             memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
         }
     }

     *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
     *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

     // 判断节点是否是根节点，是根节点则创建新的父节点
     if(is_node_root(old_node)) {
         // 传入了右孩子的索引(在哪个page)
         return create_new_root(cursor -> table, new_page_num);
     } else {
         printf("Need to implement updating parent after split\n");
         exit(EXIT_FAILURE);
     }
}

void create_new_root(Table* table, uint32_t right_child_page_num) {
    /*
     * Handle splitting the root
     * Old root copied to new page, becomes left child.
     * Address of right child passed in.
     * Re-initialize root page to contain the new root node.
     * New root node points to two children.
     */
    void* root = get_page(table -> pager, table -> root_page_num);
    void* right_child = get_page(table -> pager, right_child_page_num);
    uint32_t  left_child_page_num = get_unused_page_num(table -> pager);
    void* left_child = get_page(table -> pager, left_child_page_num);
    // old root is copied to the left child so we can reuse the root page
    memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);
    /*
     * Root node is a new internal node with one key and two children
     */
    initialize_leaf_node(root);
    set_node_root(root, true);
    *internal_node_num_keys(root) = 1;
    // 设置指向左孩子
    *internal_node_child(root, 0) = left_child_page_num;
    // 获取左孩子最大的key 当做root的key
    uint32_t left_child_max_key = get_node_max_key(left_child);
    *internal_node_key(root, 0) = left_child_max_key;
    *internal_node_right_child(root) = right_child_page_num;
}


Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key) {
    void* node = get_page(table -> pager, page_num);
    uint32_t nums_keys = *internal_node_num_keys(node);
    // Binary search to find index of child to search
    uint32_t min_idx = 0;
    uint32_t max_idx = nums_keys; // one more child than key
    // min_idx = max_idx = key
    while (min_idx != max_idx) {
        uint32_t index = (min_idx + max_idx) / 2;
        uint32_t key_to_right = *internal_node_key(node, index);
        if(key_to_right >= key) {
            max_idx = index; // 向左找 或者相等 >=都算满足条件，代表有可能在孩子节点中包含
        } else {
            min_idx = index + 1; // 向右找
        }
    }

    uint32_t child_num = *internal_node_child(node, min_idx);
    void* child = get_page(table -> pager, child_num);
    switch (get_node_type(child)) {
        case NODE_LEAF:
            return leaf_node_find(table, child_num, key);
        case NODE_INTERNAL:
            return internal_node_find(table, child_num, key);
    }


}



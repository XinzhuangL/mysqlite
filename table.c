



#include "table.h"

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

Pager* pager_open(const char* filename) {
    int fd = open(filename,
                  O_RDWR | O_CREAT,  // 读写模式,创建文件如果不存在
                  S_IWUSR | S_IRUSR  // 用户读写权限
                  );
    if(fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }
    off_t file_length = lseek(fd, 0 ,SEEK_END);
    Pager* pager = malloc(sizeof(Pager));
    pager -> file_descriptor = fd;
    pager -> file_length = file_length;

    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

Table* db_open(const char* filename) {
    Pager* pager = pager_open(filename);
    uint32_t num_rows = pager -> file_length / ROW_SIZE;

    Table* table = malloc(sizeof(Table));
    table -> pager = pager;
    table -> num_rows = num_rows;
    return table;
}
void* get_page(Pager* pager, uint32_t page_num) {
    // todo page_num == TABLE_MAX_PAGES
    if(page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }
    // cache miss 从文件读取
    if(pager -> pages[page_num] == NULL) {
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager -> file_length / PAGE_SIZE;
        // 不能整除则说明还有不能填满一个page的数据
        if(pager -> file_length % PAGE_SIZE) {
            num_pages += 1;
        }
        // 内存中不命中 从file中读取
        if(page_num < num_pages) {
            lseek(pager -> file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager -> file_descriptor, page, PAGE_SIZE);
            if(bytes_read == -1) {
                printf("Error reading file: %d \n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager -> pages[page_num] = page;
    }
    return pager -> pages[page_num];
}

void db_close(Table* table) {
    Pager* pager = table -> pager;
    uint32_t  num_full_pages = table -> num_rows / ROWS_PER_PAGE;

    for(uint32_t i = 0; i < num_full_pages; i++) {
        if(pager -> pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i , PAGE_SIZE);
        free(pager -> pages[i]);
        pager -> pages[i] = NULL;
    }
    // 多出来的行
    uint32_t  num_additional_rows = table -> num_rows % ROWS_PER_PAGE;
    if(num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if(pager -> pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager -> pages[page_num]);
            pager -> pages[page_num] = NULL;
        }
    }

    int result = close(pager -> file_descriptor);
    if(result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager -> pages[i];
        if(page) {
            free(page);
            pager -> pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}
// 计算插入位置
void *row_slot(Table *table, uint32_t row_num) {
    // 计算当前到那个page
    uint32_t page_num = row_num / ROWS_PER_PAGE;
//    void *page = table -> pages[page_num];
//    if(page == NULL) {
//        // 新page分配内存
//        page = table -> pages[page_num] = malloc(PAGE_SIZE);
//    }
    void* page = get_page(table -> pager, page_num);
    // 计算到哪行
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    // 计算到那个bytes
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
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

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size) {
    if(pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }
    off_t offset = lseek(pager -> file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

    if(offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_written = write(pager -> file_descriptor, pager -> pages[page_num], size);

    if(bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

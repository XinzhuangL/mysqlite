#ifndef INPUT_H
#define INPUT_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char *buffer;
    size_t buffer_length;
    size_t input_length;
} InputBuffer;


// 初始化地址空间
InputBuffer *new_input_buffer();

// 打印提示信息
void print_prompt();

// 读取输入到 InputBuffer
void read_input(InputBuffer *input_buffer);

// 关闭输入
void close_input_buffer(InputBuffer *input_buffer);

#endif
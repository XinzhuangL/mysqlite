#include "input.h"
#include "statement.h"
#include "common.h"


int main() {
    Table *table = new_table();
    InputBuffer *input_buffer = new_input_buffer();
    while (true) {
        // 循环处理输入 直到退出
        // 打印提示
        print_prompt();
        // 读取输入
        read_input(input_buffer);
        // .开头的为系统命令
        if(input_buffer -> buffer[0] == '.') {
            switch(do_meta_command(input_buffer, table)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'\n", input_buffer -> buffer);
                    continue;
            }
        }
        // 处理非系统命令
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer -> buffer);
                continue;
        }
        switch (execute_statement(&statement, table)) {
            case EXECUTE_SUCCESS:
                printf("Executed.\n");
                break;
            case EXECUTE_TABLE_FULL:
                printf("Error: Table full.\n");
                break;
        }
    }
    return 0;
}
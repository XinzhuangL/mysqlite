//
// Created by lixinzhuang1 on 2023/5/31.
//

#include <stdio.h>
#include <string.h>


int main() {
    const char src[50] = "http:baidu.com";
    char dest[50];
    strcpy(dest, "Helooo!!");
    printf("Before memcpy dest = %s\n", dest);
    memcpy(dest, src, strlen(src) + 1);
    printf("After memcpy dest = %s\n", dest);
    return 0;
}

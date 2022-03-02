/*
 * @Author: h3n4l
 * @Date: 2022-03-01 13:27:38
 * @LastEditors: h3n4l
 * @LastEditTime: 2022-03-01 13:38:59
 * @FilePath: /xv6-labs-2021/user/sleep.c
 */
#include "../kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: sleep <time>\n");
        exit(1);
    }
    int times = 0;
    times = atoi(argv[1]);
    sleep(times);
    exit(0);
}
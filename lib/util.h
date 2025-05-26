#ifndef C_PROGRAMMING_UTIL_H
#define C_PROGRAMMING_UTIL_H

#define true 1
#define false 0

typedef struct {
  int x;
  int y;
} Pair;

// Windows에서 min, max 함수가 이미 정의되어 있어서 이름을 바꿈
int min_v(int, int);
int max_v(int, int);

#endif //C_PROGRAMMING_UTIL_H

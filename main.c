#include <stdio.h>
#include "lib/console.h"
#include "lib/util.h"

int main() {
  init_console();
  clear_console();
  hide_cursor();

  Pair size = get_console_size();
  if (size.x == 0 || size.y == 0) {
    printf("%s", FG_RED);
    printf("WARN : 콘솔 정보를 읽지 못했습니다.\n");
    printf("다른 터미널을 사용해보세요.\n");
    printf("%s", RESET);
    return 0;
  }

  printf("콘솔 크기: %d x %d\n", size.x, size.y);

  int width = 21, height = 21;
  move_cursor(3, 3);
  printf("너비 : %s%d%s, 높이: %s%d%s\n", FG_YELLOW, width, RESET, FG_YELLOW, height, RESET);
  move_cursor(3, 5);
  printf("A : 폭 감소, D : 폭 증가\n");
  move_cursor(3, 6);
  printf("W : 높이 증가, S : 높이 감소\n");
  move_cursor(3, 7);
  printf("Q : 종료, Enter : 시작\n");

  while (true) {
    int key = read_key();

    if (key == 'q' || key == 'Q') {
      clear_console();
      return 0;
    } else if (key == 'a' || key == 'A') {
      width = max(3, width - 2);
    } else if (key == 'd' || key == 'D') {
      width += 2;
    } else if (key == 'w' || key == 'W') {
      height += 2;
    } else if (key == 's' || key == 'S') {
      height = max(3, height - 2);
    } else if (key == '\n' || key == '\r') {
      clear_console();
      break;
    }

    if (width > size.x)
      width = size.x;
    if (height > size.y)
      height = size.y;

    if (width % 2 == 0)
      width--;
    if (height % 2 == 0)
      height--;

    move_cursor(3, 3);
    printf("너비 : %s%d%s, 높이: %s%d%s        \n",
        FG_YELLOW, width, RESET, FG_YELLOW, height, RESET);
  }

  show_cursor();
  return 0;
}

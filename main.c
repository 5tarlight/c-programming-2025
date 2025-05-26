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
    printf("WARN : Failed to fetch console information.\n");
    printf("Consider using other terminals.\n");
    printf("%s", RESET);
    return 0;
  }

  printf("Console size: %d x %d\n", size.x, size.y);

  int width = 21, height = 21;
  move_cursor(3, 3);
  printf("width : %s%d%s, height: %s%d%s\n", FG_YELLOW, width, RESET, FG_YELLOW, height, RESET);
  move_cursor(3, 5);
  printf("D, A : increase/decrease width\n");
  move_cursor(3, 6);
  printf("W, S : increase/decrease height\n");
  move_cursor(3, 7);
  printf("Q : quit, F : start\n");

  while (true) {
    int key = read_key();

    if (key == 'q' || key == 'Q') {
      clear_console();
      return 0;
    } else if (key == 'a' || key == 'A') {
      width = max_v(3, width - 2);
    } else if (key == 'd' || key == 'D') {
      width += 2;
    } else if (key == 'w' || key == 'W') {
      height += 2;
    } else if (key == 's' || key == 'S') {
      height = max_v(3, height - 2);
    } else if (key == 'f' || key == 'F') {
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
    printf("width : %s%d%s, height: %s%d%s        \n",
        FG_YELLOW, width, RESET, FG_YELLOW, height, RESET);
  }

  show_cursor();
  return 0;
}

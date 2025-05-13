#include <stdio.h>
#include "lib/console.h"

int main() {
  init_console();
  clear_console();
  hide_cursor();

  Pair size = get_console_size();
  if (size.x == 0 || size.y == 0) {
    printf("%s", FG_RED);
    printf("Error: Unable to get console size.\n");
    printf("%s", RESET);
    return 0;
  }

  // Print border
  for (int i = 0; i < size.x; i++) {
    move_cursor(i, 0);
    printf("#");
    move_cursor(i, size.y - 1);
    printf("#");
  }

  for (int i = 0; i < size.y; i++) {
    move_cursor(0, i);
    printf("#");
    move_cursor(size.x - 1, i);
    printf("#");
  }

  move_cursor(2, 2);
  printf("Console Size: %d x %d\n", size.x, size.y);

  while (1) {
    int key = read_key();
    if (key == 0) continue;
    move_cursor(2, 3);
    printf("Key Pressed: %d\n", key);

    if (key == 'q')
      break;
  }

  show_cursor();
  return 0;
}

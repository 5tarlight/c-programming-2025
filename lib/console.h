#ifndef C_CONSOLE_H
#define C_CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

/**
 * 운영체제마다 사용할 수 있는 헤더가 다르기 때문에
 * 각 운영체제에 맞는 헤더를 include 한다.
 */
#ifdef _WIN32
#include <windows.h>
/**
 * 주어진 시간(ms) 동안 대기한다.
 * Windows 운영체제에서는 Sleep 함수를 사용한다.
 * @param ms 대기할 시간(ms)
 */
#define SLEEP(ms) Sleep(ms)
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
/**
 * 주어진 시간(ms) 동안 대기한다.
 * UNIX 계열 운영체제에서는 `usleep` 함수를 사용한다.
 * `usleep`은 파라미터로 마이크로초를 받기 때문에 1000을 곱해준다.
 * @param ms 대기할 시간(ms)
 */
#define SLEEP(ms) usleep((ms) * 1000)
// 윗 줄에서 (ms) 로 감싸는 이유는 SLEEP(10 + 20) 과 같이 사용할 때
// 10 + 20 * 1000 = 20010 이 되는 것을 방지하기 위함이다.
#endif

// 각 운영체제마다 방향키를 나타내는 코드가 다르기 때문에
// 임의로 코드를 정의한다.

/** Up Arrow */
#define KEY_UP 1000
/** Down Arrow */
#define KEY_DOWN 1001
/** Left Arrow */
#define KEY_LEFT 1002
/** Right Arrow */
#define KEY_RIGHT 1003

// ANSI color escape code
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define INVERSE "\033[7m"
#define HIDDEN "\033[8m"
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

/**
 * 콘솔 화면을 지운다. 운영체제에 따라 다르게 동작한다.
 * Windows에서는 `cls` 명령어를 사용하고, UNIX 계열에서는 `clear` 명령어를 사용한다.
 *
 * WARNING : loop안에서 빠르게 사용하면 화면이 심하게 깜빡거릴 수 있음.
 *
 * @see https://en.wikipedia.org/wiki/Clear_(Unix)
 */
void clear_console();

/**
 * 키보드에서 눌린 키를 읽어온다.
 *
 * 정확히는 input buffer에서 키를 읽어오는데, 이 때 buffer가 비어있으면 0을 반환한다.
 * 버퍼에 있는 키를 읽고도 버퍼가 남아있으면 비운다(flush).
 *
 * @return 눌린 키의 ASCII 코드. 방향키의 경우 미리 정의된 값을 반환한다.
 * @see KEY_UP
 * @see KEY_DOWN
 * @see KEY_LEFT
 * @see KEY_RIGHT
 */
int read_key();

/**
 * 키보드 입력 버퍼를 비운다.
 *
 * 키보드 입력 버퍼에 남아있는 키들을 모두 읽어서 버린다.
 * `read_key` 함수에서 키를 하나 읽은 후 자동으로 실행한다.
 *
 * @see read_key
 */
void flush_input();

/**
 * 콘솔 화면의 크기를 구한다.
 */
Pair get_console_size();

/**
 * 콘솔 화면의 커서를 이동시킨다.
 */
void move_cursor(int x, int y);

/**
 * 출력될 문자열의 색을 변경한다.
 * reset하기 전까지 출력되는 모든 문자열에 영향을 준다.
 * ANSI가 지원되지 않는 환경에서는 동작하지 않을 수 있다.
 */
void set_console_color(const char* color);

/**
 * 콘솔 화면의 색을 원래대로 복구한다.
 */
void reset_console_color();

/**
 * 콘솔 제어를 위한 초기 설정을 한다.
 * Windows의 cmd.exe에서 ANSI escape code를 사용하기 위해
 * Virtual Terminal Processing을 활성화한다.
 *
 * @see https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 */
void init_console();

void hide_cursor();
void show_cursor();

Pair get_console_size();

#endif // C_CONSOLE_H
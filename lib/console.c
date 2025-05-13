#include "console.h"

void clear_console() {
  // 자기 운영체제에 맞는 명령어를 실행해서 콘솔 화면을 지운다.
  // 또는 ANSI Escape Code를 사용할 수도 있다.
  // https://en.wikipedia.org/wiki/ANSI_escape_code
#ifdef _WIN32
  system("cls");
#elif defined(__APPLE__) || defined(__linux__)
  system("clear");
#endif
}

/**
 * `read_key` 함수의 내부 구현.
 * 키를 읽고 -> 버퍼를 지운다
 * 형태로 로직을 분리하기 위해 만든 함수.
 */
int read_raw_key() {
#ifdef _WIN32
  // 버퍼에 남은 데이터가 있는지 확인. 없다면 0을 반환.
  if (_kbhit()) {
    int ch = _getch();

    if (ch == 224) { // Windows에서 방향키는 224 + 코드 형태의 2byte로 입력됨.
      ch = _getch();
      switch (ch) {
      case 72:
        return KEY_UP;
      case 80:
        return KEY_DOWN;
      case 75:
        return KEY_LEFT;
      case 77:
        return KEY_RIGHT;
      }
    }

    return ch;
  }

  return 0;
#else
  // `termios`는 UNIX 계열 운영체제에서 터미널을 제어하기 위한 구조체.
  // `oldt`에 현재 터미널 설정을 저장하고, `newt`에 새로운 설정을 저장한다.
  // 프로그램이 종료되면 `oldt`로 설정을 복구할 수 있다.
  struct termios oldt, newt;
  int ch;

  // 현재 터미널 설정을 `oldt`에 저장한다.
  // STDIN_FILENO : 표준 입력 파일 디스크립터
  // STDIN, STDOUT, STDERR으로 각각 0, 1, 2로 정의되어 있다.
  tcgetattr(STDIN_FILENO, &oldt);
  // `newt`에 `oldt`를 복사한다.
  // `termios` 구조체의 데이터는 전부 비트로 플래그를 관리하는 정수형 필드로
  // 되어 있어서 단순 대입으로 복사가 가능하다.
  newt = oldt;

  // ICANON : 정규(non-canonical) 모드. 입력을 한 줄 단위로 받는다.
  // ECHO : 입력한 문자를 화면에 출력한다.
  // 위 두 플래그를 해제하면 입력을 즉시 받을 수 있다.
  newt.c_lflag &= ~(ICANON | ECHO);
  // 새로운 설정을 적용한다.
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // F_GETFL : 파일 상태 플래그를 가져온다.
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  // O_NONBLOCK : 블록킹 모드를 해제한다.
  // F_SETFL : 파일 상태 플래그를 설정한다.
  // 마찬가지로 비트 연산을 통해 플래그를 설정한다.
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

  ch = getchar();

  int key = 0;

  // UNIX 계열에서는 방향키를 읽을 때 3바이트로 나눠서 읽는다.
  if (ch == 27) {
    if (getchar() == '[') {
      switch (getchar()) {
        case 'A': key = KEY_UP; break;
        case 'B': key = KEY_DOWN; break;
        case 'C': key = KEY_RIGHT; break;
        case 'D': key = KEY_LEFT; break;
      }
    }
  } else {
    key = ch == EOF ? 0 : ch;
  }

  // 터미널 설정을 원래대로 복구한다.
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return key;
#endif
}

int read_key() {
  // 키를 읽고 버퍼를 비운다.
  int ch = read_raw_key();
//  flush_input();
  return ch;
}

void flush_input() {
#ifdef _WIN32
  // 입력 버퍼에 데이터가 남아있으면 계속 읽어서 버퍼를 비운다.
  while (_kbhit())
    _getch();
#else
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_cc[VTIME] = 0; // 입력 대기 시간(timeout)을 비활성화
  newt.c_cc[VMIN] = 0; // 최소 입력 문자 수를 0으로 설정 (입력이 없어도 바로 return)
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  while (getchar() != EOF); // 입력이 없을 때까지 계속 읽기

  // 터미널 설정을 원래대로 복구한다.
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

Pair get_console_size() {
#ifdef _WIN32
  // Console Screen Buffer Info를 가져와서 현재 콘솔의 크기를 구한다.
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  // Pair 구조체에 너비와 높이를 저장해서 반환한다.
  // Pair는 typedef로 정의되어 있기 때문에 아래 코드는
  // `(struct ...) { ... }`와 같은 형태로 해석된다.
  return (Pair){ width, height };
#else
  struct winsize w;
  // TIOCGWINSZ : 윈도우 크기를 가져오는 ioctl 명령어
  // ioctl : 입출력 장치에 대해 제어 요청을 하는 시스템 콜
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return (Pair){ w.ws_col, w.ws_row };
#endif
}

void move_cursor(int x, int y) {
#ifdef _WIN32
  COORD pos = { x, y };
  // 콘솔 화면의 커서를 이동시킨다.
  // GetStdHandle : 표준 출력 장치의 핸들을 가져온다.
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
  // ANSI Escape Code를 사용해서 커서를 이동시킨다.
  printf("\033[%d;%dH", y + 1, x + 1);
  // fflush : 버퍼에 남아있는 데이터를 출력한다.
  fflush(stdout);
#endif
}

void init_console() {
#ifdef _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleMode(hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  tty.c_lflag &= ~(ECHO);  // 입력 에코 및 버퍼링 비활성화
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

void hide_cursor() {
  printf("\e[?25l");
  fflush(stdout);
}

void show_cursor() {
  printf("\e[?25h");
  fflush(stdout);
}

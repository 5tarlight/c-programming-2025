#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

//////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  int x;
  int y;
} Pair;

// Windows에서 min, max 함수가 이미 정의되어 있어서 이름을 바꿈
int min_v(int, int);
int max_v(int, int);

//////////////////////////////////////////////////////////////////////////////////////

/**
 * 운영체제마다 사용할 수 있는 헤더가 다르기 때문에
 * 각 운영체제에 맞는 헤더를 include 한다.
 */
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
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
//void set_console_color(const char* color);

/**
 * 콘솔 화면의 색을 원래대로 복구한다.
 */
//void reset_console_color();

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

//////////////////////////////////////////////////////////////////////////////////////

int *create_ufind(int width, int height);
int find(int *parent, int x);
void unite(int *parent, int x, int y);

///////////////////////////////////////////////////////////////////////////////////

typedef struct Node {
  int data;
  struct Node* next;
} Node;

typedef struct Queue {
  Node* front;
  Node* rear;
} Queue;

void initQueue(Queue* q);
int isEmpty(Queue* q);
void enqueue(Queue* q, int value);
int dequeue(Queue* q);
void freeQueue(Queue* q);

////////////////////////////////////////////////////////////////////////////////

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
  flush_input();
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
  fflush(stdout);
#else
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  tty.c_lflag &= ~(ECHO);  // 입력 에코 및 버퍼링 비활성화
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
  fflush(stdout);
#endif
}

void hide_cursor() {
  printf("\x1b[?25l");
  fflush(stdout);
}

void show_cursor() {
  printf("\x1b[?25h");
  fflush(stdout);
}

/////////////////////////////////////////////////////////////////////////////

int min_v(int a, int b) {
  return a < b ? a : b;
}

int max_v(int a, int b) {
  return a > b ? a : b;
}

//////////////////////////////////////////////////////////////////////////

int *create_ufind(int width, int height) {
  int *parent = (int *)malloc(sizeof(int) * width * height);
  if (!parent)
    return NULL;

  for (int i = 0; i < width * height; i++)
    parent[i] = i;

  return parent;
}

int find(int *parent, int x) {
  if (parent[x] != x)
    parent[x] = find(parent, parent[x]);
  return parent[x];
}

void unite(int *parent, int x, int y) {
  int rootX = find(parent, x);
  int rootY = find(parent, y);
  if (rootX != rootY)
    parent[rootY] = rootX;
}

///////////////////////////////////////////////////////////////////////

void initQueue(Queue* q) {
  q->front = q->rear = NULL;
}

int isEmpty(Queue* q) {
  return q->front == NULL;
}

void enqueue(Queue* q, int value) {
  Node* newNode = (Node*)malloc(sizeof(Node));
  newNode->data = value;
  newNode->next = NULL;

  if (isEmpty(q)) {
    q->front = q->rear = newNode;
  } else {
    q->rear->next = newNode;
    q->rear = newNode;
  }
}

int dequeue(Queue* q) {
  if (isEmpty(q)) {
    return -1;
  }

  Node* temp = q->front;
  int result = temp->data;
  q->front = q->front->next;

  if (q->front == NULL) {
    q->rear = NULL;
  }

  free(temp);
  return result;
}

void freeQueue(Queue* q) {
  while (!isEmpty(q)) {
    dequeue(q);
  }
}

////////////////////////////////////////////////////////////////////

int main() {
  init_console();
  clear_console();
  hide_cursor();

  Pair size = get_console_size();
  if (size.x == 0 || size.y == 0) {
    printf("%s", FG_RED);
    printf("WARN : 콘솔 크기를 읽지 못했습니다.\n");
    printf("다른 터미널을 사용하세요.\n");
    printf("%s", RESET);
    return 0;
  }

  printf("콘솔 크기 : %d x %d\n", size.x, size.y);

  int width = 21, height = 21;
  move_cursor(3, 3);
  printf("너비 : %s%d%s, 높이: %s%d%s\n", FG_YELLOW, width, RESET, FG_YELLOW, height, RESET);
  move_cursor(3, 5);
  printf("D, A : 너비 조절\n");
  move_cursor(3, 6);
  printf("W, S : 높이 조절\n");
  move_cursor(3, 7);
  printf("Q : 종료, F : 시작\n");

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
    printf("너비 : %s%d%s, 높이: %s%d%s        \n",
        FG_YELLOW, width, RESET, FG_YELLOW, height, RESET);
  }

  show_cursor();
  return 0;
}

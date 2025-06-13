#ifdef _WIN32
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define true 1
#define false 0

void play_bgm() {
#ifdef _WIN32
  PlaySound("bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
#elif defined(__APPLE__)
  system("afplay bgm.wav &"); // macOS
#elif defined(__linux__)
  system("aplay bgm.wav &"); // Linux
#endif
}

void play_escape() {
#ifdef _WIN32
  PlaySound("escape.wav", NULL, SND_FILENAME | SND_ASYNC);
#elif defined(__APPLE__)
  system("afplay escape.wav &"); // macOS
#elif defined(__linux__)
  system("aplay escape.wav &"); // Linux
#endif
}

void stop_bgm() {
#ifdef _WIN32
  PlaySound(NULL, 0, 0); // Stop sound
#elif defined(__APPLE__)
  system("killall afplay"); // Stop macOS playback
#elif defined(__linux__)
  system("pkill aplay"); // Stop Linux playback
#endif
}

//////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  int x;
  int y;
} Pair;

// Windows에서 min, max 함수가 이미 정의되어 있어서 이름을 바꿈
int min_v(int, int);
int max_v(int, int);

//////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
/**
 * 주어진 시간(ms) 동안 대기한다.
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
 * `usleep`은 파라미터로 마이크로초를 받기 때문에 1000을 곱해준다.
 * @param ms 대기할 시간(ms)
 */
#define SLEEP(ms) usleep((ms) * 1000)
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
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_CYAN "\033[36m"

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

typedef struct {
  int wall; // 1: 벽, 0: 길
} Cell;

typedef struct {
  int x1, y1;
  int x2, y2;
} Wall;

Cell **maze;

void init_maze(int width, int height);
void gen_maze(int width, int height);
void shuffle_maze(Wall *walls, int size);
void clean_maze(int width, int height);

////////////////////////////////////////////////////////////////////////////////

int is_end(int x, int y, int width, int height) {
  return (x == width - 2 && y == height - 1);
}

int can_move(int nx, int ny, int width, int height) {
  if (nx < 0 || nx >= width || ny < 0 || ny >= height)
    return 0;

  return maze[ny][nx].wall == 0;
}

int is_valid_input(int key) {
  return key == KEY_UP || key == KEY_DOWN ||
         key == KEY_LEFT || key == KEY_RIGHT ||
         key == 'w' || key == 'W' ||
         key == 'a' || key == 'A' ||
         key == 's' || key == 'S' ||
         key == 'd' || key == 'D';
}

Pair get_next_position(int x, int y, int key) {
  // 방향키 또는 WASD 키에 따라 다음 위치를 계산한다.
  switch (key) {
    case KEY_UP: case 'w': case 'W':
      return (Pair){x, y - 1};
    case KEY_DOWN: case 's': case 'S':
      return (Pair){x, y + 1};
    case KEY_LEFT: case 'a': case 'A':
      return (Pair){x - 1, y};
    case KEY_RIGHT: case 'd': case 'D':
      return (Pair){x + 1, y};
    default:
      return (Pair){x, y}; // 실행되지 않을 부분
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void init_maze(int width, int height) {
  // 미로를 동적으로 할당하고 초기화한다.
  maze = (Cell **)malloc(sizeof(Cell *) * height);
  for (int i = 0; i < height; i++) {
    maze[i] = (Cell *)malloc(sizeof(Cell) * width);
    for (int j = 0; j < width; j++) {
      maze[i][j].wall = 1; // 전체를 벽으로 초기화
    }
  }

  // 길 위치 초기화
  for (int y = 1; y < height; y += 2)
    for (int x = 1; x < width; x += 2)
      maze[y][x].wall = 0;
}

void shuffle_maze(Wall *walls, int size) {
  // Fisher-Yates Shuffle 알고리즘을 사용하여 벽을 섞는다.
  for (int i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    Wall temp = walls[i];
    walls[i] = walls[j];
    walls[j] = temp;
  }
}

void gen_maze(int width, int height) {
  Wall *walls = (Wall *)malloc(sizeof(Wall) * width * height);
  int wall_count = 0;

  // 길과 길 사이의 벽 목록 만들기
  for (int i = 1; i < height; i += 2) {
    for (int j = 1; j < width; j += 2) {
      if (i + 2 < height) // 아래
        walls[wall_count++] = (Wall){j, i, j, i + 2};
      if (j + 2 < width) // 오른쪽
        walls[wall_count++] = (Wall){j, i, j + 2, i};
    }
  }

  // 벽을 섞는다.
  shuffle_maze(walls, wall_count);

  // 유니온 파인드 초기화
  int *parent = create_ufind(width, height);

  // 벽을 제거하면서 길을 만든다.
  for (int i = 0; i < wall_count; i++) {
    Wall w = walls[i];
    int x1 = w.x1, y1 = w.y1;
    int x2 = w.x2, y2 = w.y2;

    // 현재 벽이 연결하는 두 셀의 루트 노드를 찾는다.
    int root1 = find(parent, y1 * width + x1);
    int root2 = find(parent, y2 * width + x2);

    // 두 셀이 서로 다른 루트 노드를 가지면 길을 만든다.
    if (root1 != root2) {
      unite(parent, y1 * width + x1, y2 * width + x2);
      maze[(y1 + y2) / 2][(x1 + x2) / 2].wall = 0;
    }
  }

  free(walls);

  maze[0][1].wall = 0; // 시작점
  maze[height - 1][width - 2].wall = 0; // 끝점
}

void clean_maze(int width, int height) {
  for (int i = 0; i < height; i++) {
    free(maze[i]);
  }
  free(maze);
}

////////////////////////////////////////////////////////////////////////////////

void clear_console() {
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
  struct termios oldt, newt;
  int ch;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  newt.c_lflag &= ~(ICANON | ECHO);
  // 새로운 설정을 적용한다.
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
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
  int ch = 0;
  while ((ch = read_raw_key()) == 0) {
    SLEEP(10); // Wait briefly to prevent CPU overuse
  }
  flush_input();
  return ch;
}

void flush_input() {
#ifdef _WIN32
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
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
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

int find_shortest_path(int width, int height) {
  Queue q;
  initQueue(&q);

  int x = 1, y = 0;
  enqueue(&q, y * width + x);

  int **visited = (int **)malloc(sizeof(int *) * height);
  for (int i = 0; i < height; i++) {
      visited[i] = (int *)malloc(sizeof(int) * width);
      for (int j = 0; j < width; j++)
          visited[i][j] = -1;
  }

  visited[y][x] = 0; // 시작 위치 방문 표시
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};

  while (!isEmpty(&q)) {
    int pos = dequeue(&q);
    y = pos / width;
    x = pos % width;

    // 도착 지점에 도달했는지 확인
    if (x == width - 2 && y == height - 1) {
      int result = visited[y][x];
      for (int i = 0; i < height; i++) {
          free(visited[i]);
      }
      free(visited);
      return result;
    }

    // 상하좌우로 이동
    for (int i = 0; i < 4; i++) {
      int nx = x + dx[i];
      int ny = y + dy[i];

      if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
          maze[ny][nx].wall == 0 && visited[ny][nx] == -1) {
        visited[ny][nx] = visited[y][x] + 1;
        enqueue(&q, ny * width + nx);
      }
    }
  }

  for (int i = 0; i < height; i++) {
      free(visited[i]);
  }
  free(visited);
  freeQueue(&q);
  return -1; // 도착 지점에 도달하지 못한 경우, 실행되지 않음
}

////////////////////////////////////////////////////////////////////

int play_count = 0;
int* scores;

void add_score(int score) {
  int *temp = (int *)malloc(sizeof(int) * (play_count + 1));
  for (int i = 0; i < play_count; i++)
    temp[i] = scores[i];
  temp[play_count] = score;
  play_count++;
  free(scores);
  scores = temp;
}

void sort_scores() {
  // Bubble Sort
  for (int i = 0; i < play_count - 1; i++) {
    for (int j = i + 1; j < play_count; j++) {
      if (scores[i] < scores[j]) {
        int temp = scores[i];
        scores[i] = scores[j];
        scores[j] = temp;
      }
    }
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

  while (true) {
    // Game menu
    clear_console();
    move_cursor(1, 1);
    printf("%sM%sA%sZ%sE %sRunner\n", FG_CYAN, RESET, FG_YELLOW, FG_GREEN, RESET);
    move_cursor(1, 3);
    printf("F : %s새 게임 시작%s\n", FG_GREEN, RESET);
    move_cursor(1, 4);
    printf("Q : %s종료%s\n", FG_RED, RESET);

    move_cursor(1, 6);
    printf("랭킹");
    sort_scores();
    for (int i = 0; i < play_count; i++) {
      move_cursor(1, 7 + i);
      printf("%d위 : %s%d%s\n", i + 1, FG_GREEN, scores[i], RESET);
    }

    int key;
    do {
      key = read_key();
    } while (key != 'f' && key != 'F' && key != 'q' && key != 'Q');

    if (key == 'q' || key == 'Q')
      break;

    // Start new game
    init_maze(width, height);
    gen_maze(width, height);

    clear_console();
    move_cursor(0, 0);
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        if (maze[i][j].wall) {
          printf("#");
        } else {
          printf(" ");
        }
      }
      printf("\n");
    }

    // Main Game Logic
    int x = 1, y = 0; // 시작 위치
    time_t start_time = time(NULL);
    int move_count = 0;
    play_bgm();
    while (!is_end(x, y, width, height)) {
      move_cursor(x, y);
      printf("%s@", FG_GREEN); // 현재 위치 표시

      int key;
      Pair next_pos;
      do {
        key = read_key();
        next_pos = get_next_position(x, y, key);
      } while (
          !is_valid_input(key) ||
          !can_move(next_pos.x, next_pos.y, width, height)
          );

      // 현재 위치를 비운다.
      move_cursor(x, y);
      printf("%s ", RESET); // 빈 공간으로 표시
      x = next_pos.x;
      y = next_pos.y;
      move_count++;
    }

    time_t end_time = time(NULL);
    stop_bgm();
    clear_console();
    int shortest = find_shortest_path(width, height);

    play_escape();
    move_cursor(1, 1);
    printf("%s미로를 탈출했습니다!%s\n", FG_CYAN, RESET);
    move_cursor(1, 2);
    time_t taken = end_time - start_time;
    printf("소요 시간: %ld초, 이동 횟수: %d(%d)\n", taken, move_count, shortest);

    double time_score = 1500.0 * (shortest / (double)taken);      // 시간 기준 점수
    double move_score = 1500.0 * (shortest / (double)move_count); // 이동 기준 점수

    int score = (int)(time_score + move_score);

    move_cursor(1, 3);
    printf("점수: %s%d%s\n", FG_GREEN, score, RESET);
    add_score(score);

    move_cursor(1, 5);
    printf("계속하려면 F 키를 누르세요.\n");

    do {
      key = read_key();
    } while (key != 'f' && key != 'F');

    clean_maze(width, height);
  }

  stop_bgm();
  show_cursor();
  return 0;
}

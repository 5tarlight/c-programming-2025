/*/////////////////////////////

 2025-04-29 김규산
 Random Maze Generation
 - Algorithm : Kruskal's Algorithm

 크루스칼 알고리즘은 Union-Find를 기반으로 함.
 전체 맵에 거대한 MST를 만드는 것으로 생각할 수 있음.
 다만 모든 점의 가중치가 1 이므로 PriorityQueue는 사용하지 않고
 진행할 수 있음.

 Union-Find는 어떤 지점과 어떤 지점이 연결되었는지를 O(1)에 확인할 수 있음.
 어떤 지점에서 2칸 떨어진 지점을 탐색하고 Union-Find를 통해서
 그 지점과 현재 지점이 연결되었는지를 확인할 수 있음.
 만약 연결되지 않았다면 사이 벽을 부수고 같은 지점으로 통합

/////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 21 // 미로 크기 (홀수)

typedef struct {
  int wall; // 1: 벽, 0: 길
} Cell;

typedef struct {
  int x1, y1;
  int x2, y2;
} Wall;

Cell maze[N][N];
int parent[N * N];

int find(int x) {
  if (parent[x] != x)
    parent[x] = find(parent[x]);
  return parent[x];
}

void unite(int x, int y) {
  x = find(x);
  y = find(y);
  if (x != y)
    parent[y] = x;
}

// 벽 섞기
void shuffle(Wall *walls, int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    Wall temp = walls[i];
    walls[i] = walls[j];
    walls[j] = temp;
  }
}

void init_maze() {
  // 전체를 벽으로 초기화
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      maze[i][j].wall = 1;

  // 길 위치 초기화
  for (int i = 1; i < N; i += 2)
    for (int j = 1; j < N; j += 2)
      maze[i][j].wall = 0;

  // 유니온 파인드 초기화
  for (int i = 0; i < N * N; i++)
    parent[i] = i;
}

void generate_maze() {
  Wall walls[N * N];
  int wall_count = 0;

  // 길과 길 사이의 벽 목록 만들기
  for (int i = 1; i < N; i += 2) {
    for (int j = 1; j < N; j += 2) {
      if (i + 2 < N) { // 아래
        walls[wall_count++] = (Wall){i, j, i + 2, j};
      }
      if (j + 2 < N) { // 오른쪽
        walls[wall_count++] = (Wall){i, j, i, j + 2};
      }
    }
  }

  shuffle(walls, wall_count);

  // 벽 부수기
  for (int i = 0; i < wall_count; i++) {
    int x1 = walls[i].x1, y1 = walls[i].y1;
    int x2 = walls[i].x2, y2 = walls[i].y2;

    int id1 = x1 * N + y1;
    int id2 = x2 * N + y2;

    if (find(id1) != find(id2)) {
      unite(id1, id2);
      maze[(x1 + x2) / 2][(y1 + y2) / 2].wall = 0; // 중간 벽 부수기
    }
  }

  // 시작/끝 벽 뚫기
  maze[0][1].wall = 0; // 위쪽 입구
  maze[N-1][N-2].wall = 0; // 아래쪽 출구
}

void print_maze() {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf(maze[i][j].wall ? "██" : "  ");
    }
    printf("\n");
  }
}

int main() {
  srand(time(0));

  init_maze();
  generate_maze();
  print_maze();

  return 0;
}

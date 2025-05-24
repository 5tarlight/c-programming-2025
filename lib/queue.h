#ifndef C_PROGRAMMING_QUEUE_H
#define C_PROGRAMMING_QUEUE_H

// 큐 노드 구조체
typedef struct Node {
  int data;
  struct Node* next;
} Node;

// 큐 구조체
typedef struct Queue {
  Node* front;
  Node* rear;
} Queue;

// 함수 선언
void initQueue(Queue* q);
int isEmpty(Queue* q);
void enqueue(Queue* q, int value);
int dequeue(Queue* q);
void freeQueue(Queue* q);

#endif // C_PROGRAMMING_QUEUE_H

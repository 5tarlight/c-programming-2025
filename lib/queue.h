#ifndef C_PROGRAMMING_QUEUE_H
#define C_PROGRAMMING_QUEUE_H

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

#endif // C_PROGRAMMING_QUEUE_H

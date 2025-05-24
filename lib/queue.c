#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

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

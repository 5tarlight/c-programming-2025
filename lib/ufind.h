#ifndef C_PROGRAMMING_UFIND_H
#define C_PROGRAMMING_UFIND_H

int *create_ufind(int width, int height);
int find(int *parent, int x);
void unite(int *parent, int x, int y);

#endif //C_PROGRAMMING_UFIND_H

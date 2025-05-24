#include "ufind.h"
#include <stdlib.h>

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

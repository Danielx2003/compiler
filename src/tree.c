#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum node_type {
  NODE_TYPE_A,
  NODE_TYPE_B,
  NODE_TYPE_C,
  NODE_TYPE_D,
};

struct NodeD {
  char last[32];
  struct Node *children;
};

struct NodeC {
  char name[32];
  struct Node *children;
};

struct NodeB {
  int day;
  struct Node *children;
};

struct NodeA {
  int age;
  struct Node *children;
};

struct Node {
  void *self;
  enum node_type type;
  struct Node *children;
  void (*print)(void *);
};

struct Tree {
  struct Node *nodes;
};


/*
void print_d(void *self)
{
  struct NodeD *d = (struct NodeD *)self;
  printf("NodeD: %s\n", d->last);
}

void print_c(void *self)
{
  struct NodeA *c = (struct NodeC *)self;
  printf("NodeC: %s\n", c->name);
}
*/

void print_b(void *self)
{
  struct NodeB *b = (struct NodeB *)self;
  printf("NodeB: %d\n", b->day);
}


void print_a(void *self)
{
  struct NodeA *a = (struct NodeA *)self;
  printf("NodeA: %d\n", a->age);
}

struct Node create_b(struct NodeB *b)
{
  return (struct Node){
    .self = b,
    .type = NODE_TYPE_B,
    .print = print_b
  };
}

struct Node create_a(struct NodeA *a)
{
  return (struct Node){
    .self = a,
    .type = NODE_TYPE_A,
    .print = print_a
  };
}

void print_tree(struct Tree *tree)
{
  for (int i = 0; i < 1; i++)
  {
    tree->nodes[i].print(tree->nodes[i].self);
    // ptr->print(ptr);
  }
}

int main()
{
  struct NodeA a2 = {
    .age = 22
  };

  struct NodeB b2 = {
    .day = 4
  };

  struct Node a = create_a(&a2);
  struct Node b = create_b(&b2);

  struct NodeA *temp = (struct NodeA *)a.self;

  /*
  struct Node b = {
    .b = {
      .day = 4
    },
    .print = print_b
  };

  struct Node c = {
    .self = {
      .name = "Daniel"
    },
    .print = print_c
  };

  struct Node d = {
    .d = {
      .last = "Spicer"
    },
    .print = print_d
  };

  struct Node d1 = {
    .d = {
      .last = "Surname"
    },
    .print = print_d
  };
  */

  struct Tree tree;
  tree.nodes = (struct Node *)malloc(sizeof(struct Node) * 1);
  memcpy(&tree.nodes[0], &a, sizeof(struct Node));
  // tree.nodes[0] = a;
  // a.children = &b;
  print_tree(&tree);
}

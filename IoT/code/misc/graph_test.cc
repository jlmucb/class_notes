#include "stdio.h"
#include "sys/types.h"
#include <vector>
#include "graph.h"

using std::vector;
using std::string;

int mark_count = 0;

void init_mark() {
  mark_count = 0;
}

void mark(node* n) {
  n->data_ = mark_count++;
  n->visited_ = true;
}

int main(int an, char** av) 
{
  node* root = test_graph();
  printf("\nInitial order:\n");
  print_graph(root, 0);

  clear_data(root);
  clear_visited(root);
  clear_wt(root);
  printf("\nTopological sort:\n");
  topsort(root);

  clear_data(root);
  clear_visited(root);
  clear_wt(root);
  init_mark();
  bfs(root, &mark);
  printf("\nbfs order:\n");
  print_graph(root, 0);

  clear_data(root);
  clear_visited(root);
  clear_wt(root);
  init_mark();
  dfs(root, &mark);
  printf("\ndfs order:\n");
  print_graph(root, 0);

  return 0;
}



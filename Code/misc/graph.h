#include "stdio.h"
#include "sys/types.h"
#include <string>
#include <vector>

#ifndef __GRAPH_H__
#define __GRAPH_H__

using std::vector;
using std::string;


class node {
public:
  int data_;
  int wt_;
  bool visited_;
  string label_;
  vector <node*> kids_;

  node() {data_ = 0; wt_ = 0; visited_=false;};
};

void bfs(node* n, void (* visit)(node*));
void dfs(node* n, void (* visit)(node*));
void print_node(node* n);
void print_graph(node* root, int level);
node* copy_tree(node* from);
node* test_graph();
void clear_data(node* n);
void clear_visited(node* n);
void clear_wt(node* n);
void topsort(node* root);
#endif


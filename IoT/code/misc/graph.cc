#include "stdio.h"
#include "sys/types.h"
#include <vector>
#include <string>
#include "graph.h"

using std::vector;
using std::string;

void collect_peers(node* kid, vector <node*>& peers) {
  for(node* k : kid->kids_) {
        peers.push_back(k);
  }
}

void bfs_current_level(vector <node*>& current_level, void (* visit)(node*)) {
  for(node* k : current_level) {
    visit(k);
  }

  vector <node*> all_kids;
  for(node* k : current_level) {
    collect_peers(k, all_kids);
  }
  if (all_kids.size() > 0)
    bfs_current_level(all_kids,  visit);
}

void bfs(node* root, void (* visit)(node*)) {
  vector <node*> current_level;
  current_level.push_back(root); 
  bfs_current_level(current_level, visit);
}

void clear_data(node* n) {
  n->data_ = 0;
  for(node* k : n->kids_) {
    clear_data(k);
  }
}

void clear_visited(node* n) {
  n->visited_ = false;
  for(node* k : n->kids_) {
    clear_visited(k);
  }
}

void clear_wt(node* n) {
  n->wt_ = 0;
  for(node* k : n->kids_) {
    clear_wt(k);
  }
}

void dfs(node* n, void(* visit)(node*)) {
  visit(n);
  for(node* k : n->kids_) {
    dfs(k, visit);
  }
}

node* test_graph() {
  int count = 0;

  node* root = new(node);
  root->data_ = count++;
  root->label_ = std::to_string(root->data_);

  node* child01 = new node;
  child01->data_ = count++;
  child01->label_ = std::to_string(child01->data_);
  node* child02 = new node;
  child02->data_ = count++;
  child02->label_ = std::to_string(child02->data_);
  node* child11 = new node;
  child11->data_ = count++;
  child11->label_ = std::to_string(child11->data_);
  node* child12 = new node;
  child12->data_ = count++;
  child12->label_ = std::to_string(child12->data_);
  node* child13 = new node;
  child13->data_ = count++;
  child13->label_ = std::to_string(child13->data_);
  node* child14 = new node;
  child14->data_ = count++;
  child14->label_ = std::to_string(child14->data_);
  node* child21 = new node;
  child21->data_ = count++;
  child21->label_ = std::to_string(child21->data_);
  node* child22 = new node;
  child22->data_ = count++;
  child22->label_ = std::to_string(child22->data_);
  node* child23 = new node;
  child23->data_ = count++;
  child23->label_ = std::to_string(child23->data_);
  node* child24 = new node;
  child24->data_ = count++;
  child24->label_ = std::to_string(child24->data_);

  root->kids_.push_back(child01);
  root->kids_.push_back(child02);
  child01->kids_.push_back(child11);
  child01->kids_.push_back(child12);
  child01->kids_.push_back(child13);
  child02->kids_.push_back(child14);
  child12->kids_.push_back(child21);
  child12->kids_.push_back(child22);
  child14->kids_.push_back(child23);
  child14->kids_.push_back(child24);

  return root;
}

void print_node(node* n) {
  printf("%s, ", n->label_.c_str());
  printf("data: %d, ", n->data_);
  if (n->visited_)
    printf("visited, ");
  else
    printf("not visited, ");
  printf("wt: %d\n", n->wt_);
}

void print_graph(node* root, int level) {
  for(int i = 0; i < level; i++)
    printf("   ");
  print_node(root);
  for(node* k : root->kids_) {
    print_graph(k, level + 1);
  }
}

node* copy_tree(node* from) {
  node* to = new(node);

  to->data_ = from->data_;
  to->wt_ = from->wt_;
  to->visited_ = from->visited_;
  to->label_= from->label_;
  for(node* k : from->kids_) {
    to->kids_.push_back(copy_tree(k));
  }
  return to;
}

// Topological sort.

void remove_leaves(node* n) {
  while (n->kids_.size() > 0) {
    node* k = n->kids_.back();
    n->kids_.pop_back();
    remove_leaves(k);
    }
  print_node(n);
}

void topsort(node* root) {
  node* t = copy_tree(root);

  remove_leaves(t);
}


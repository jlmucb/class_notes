#include<iostream.h>
   enum tower { A='A', B='B', C='C'};

   void TowersOfHanoi(int n, tower x,
                        tower y, tower z)
   // Move the top n disks from tower x to tower y.
   {   if (n) {
          TowersOfHanoi(n-1, x, z, y);
          cout << "move top disk from tower " << char(x)
          << " to top of tower " << char(y) << endl;
          TowersOfHanoi(n-1, z, y, x);
       }
   }

#include<iostream.h>

   void Perm(Type a[], int k, int n)
   {   if (k==n) { // Output permutation.
          for (int i=1; i<=n; i++) cout << a[i] << " ";
          cout << endl;
       }
       else // a[k:n] has more than one permutation.
            // Generate these recursively.
            for (int i=k; i<=n; i++) {
                Type t=a[k]; a[k]=a[i]; a[i]=t;
                Perm(a, k+1, n);
                // All permutations of a[k+1:n]
                t=a[k]; a[k]=a[i]; a[i]=t;
            }
   }

int Prime0(int n, int alpha)
   // Returns 1 if n is a prime and 0 otherwise;
   // alpha is the probability parameter.
   {
       int q = n-1, large; // Specify large.
       for (int i=1; i<=large; i++) {
          int m = q, y = 1;
          int a = random() % q + 1;
          // Choose a random number in the range [1, n-1].
          int z = a;
          // Compute a^{n-1}mod n.
          while (m > 0) {
             while (m%2 == 0) {
                z = (z*z) % n; m /= 2;
             }
             m--; y = (y*z) % n;
          }
          if (y != 1) return(0);
          // If a^{n-1}mod n is not 1, n is not a prime.
       }
       return(1);
   }

   int Prime(int n, int alpha)
   // Returns 1 if n is a prime and 0 otherwise.
   // alpha is the probability parameter.
   {
       int q = n-1;
       for (int i=1; i<=(alpha*log(n)); i++) {
          int m = q, y = 1;
          int a = random() % q + 1;
          // Choose a random number in the range [1, n-1].
          int z = a;
          // Compute a^{n-1}mod n.
          while (m > 0) {
             while (m%2 == 0) {
                int x = z; z = (z*z) % n;
                // If x is a nontrivial square
                // root of 1, n is not a prime.
                if ((z==1) && (x != 1) && (x != q))
                   return(0);
                m /= 2;
             }
             m--; y = (y*z) % n;
          }
          if (y != 1) return(0);
          // If a^{n-1}mod n is not 1, n is not a prime.
       }
       return(1);
   }

#include<iostream.h>

   template<class Type>
   class Stack
   {
       private:
          int top, MaxSize;
          Type *stack;

       public:
          Stack(int MSize): MaxSize(MSize)
          {   stack = new Type[MaxSize]; top=-1;}
          ~Stack()
          {   delete [] stack;}
          inline bool Add(const Type item)
             // Push an element onto the stack; return
             // true if successful else return false.
          {   if (StackFull()) {
                 cout << "Stack is full"; return false;
              }
              else {
                 stack[++top] = item; return true;
              }
          }
          inline bool Delete(Type& item)
             // Pop the top element from the stack; return
             // true if successful else return false.
          {   if (StackEmpty()) {
                 cout << "Stack is empty"; return false;
              }
              else {
                 item = stack[top--]; return true;
              }
          }
          inline bool StackEmpty()
             // Is the stack empty?
          {   if (top < 0) return true;
              else return false;   }
          inline bool StackFull()
             // Is the stack full?
          {   if (top >= (MaxSize-1)) return true;
              else return false;   }

   };


   #define NULL 0
   #include<iostream.h>

   class Stack
   {
       private:
          struct node {
             Type data; struct node *link;
          };
          struct node *top;
       public:
          Stack()
          {  top = NULL; }
          ~Stack()
          {   struct node *temp;
              while (top) {
                 temp=top; top=top->link;
                 delete temp;
              }
          }
          bool Add(const Type item);
          bool Delete(Type& item);
          inline bool StackEmpty()
             // Is the stack empty?
          {  if (top) return false;
              else return true;
          }
   };

bool Queue::AddQ(Type item)
   // Insert item in the circular queue stored in q[MaxSize].
   // rear points to the last item and front is one
   // position counterclockwise from the first item in q.
   {
      rear = (++rear) % MaxSize; // Advance rear clockwise.
      if (front == rear) {
         cout << "Queue is full" << endl;
         if (!front) rear = MaxSize-1; else rear--;
         // Move rear one position counterclockwise.
         return false;
      }
      else {
         q[rear] = item; // Insert new item.
         return true;
      }
   }

                       // (a) Addition of an element

   bool Queue::DeleteQ(Type& item)
   // Removes and returns the front element
   // of the queue q[MaxSize].
   {
      if (front == rear) {
         cout << "Queue is empty" << endl;
         return false;
      }
      else {
         front=(++front)%MaxSize; // Advance front clockwise.
         item = q[front]; // Return front of queue.
         return true;
      }
   }
                       // (b) Deletion of an element


   #define NULL 0
   #include<iostream.h>

   class Stack
   {
       private:
          struct node {   

bool Queue::AddQ(Type item)
   // Insert item in the circular queue stored in q[MaxSize].
   // rear points to the last item and front is one
   // position counterclockwise from the first item in q.
   {
      rear = (++rear) % MaxSize; // Advance rear clockwise.
      if (front == rear) {
         cout << "Queue is full" << endl;
         if (!front) rear = MaxSize-1; else rear--;
         // Move rear one position counterclockwise.
         return false;
      }
      else {
         q[rear] = item; // Insert new item.
         return true;
      }
   }


// (a) Addition of an element

   bool Queue::DeleteQ(Type& item)class Dictionary
   {
       private:
          // Implementation details
       public:
          Dictionary(int MSize);
          ~Dictionary();
          void Insert(Type item);
          void Delete(Type& item);
          bool Search(Type item);
   };


   // Removes and returns the front element
   // of the queue q[MaxSize].
   {
      if (front == rear) {
         cout << "Queue is empty" << endl;
         return false;
      }
      else {
         front=(++front)%MaxSize; // Advance front clockwise.
         item = q[front]; // Return front of queue.
         return true;
      }
   }
                       // (b) Deletion of an element




             Type data; struct node *link;
          };
          struct node *top;
       public:
          Stack()
          {  top = NULL; }
          ~Stack()
          {   struct node *temp;
              while (top) {
                 temp=top; top=top->link;
                 delete temp;
              }
          }
          bool Add(const Type item);
          bool Delete(Type& item);
          inline bool StackEmpty()
             // Is the stack empty?
          {  if (top) return false;
              else return true;
          }
   };

class TreeNode{
      friend class BSTree;
      private:
         TreeNode *lchild, *rchild;
         Type data;
   };

   class BSTree
   {
       private:
          TreeNode* tree;
          TreeNode* Search(TreeNode* t, Type x);
        public:
           BSTree() {tree=NULL;}
           ~BSTree() {delete tree;}
           TreeNode* Search(Type x);
                     // Recursively search for x.
           TreeNode* ISearch(Type x);
                     // Iteratively search for x.
           void Insert(Type x);
                // Insert x.
           void Delete(Type x);
                // Delete x.
   };



   #define NULL 0

   TreeNode* BSTree::Search(Type x)
   {   return Search(tree, x);   }

   TreeNode* BSTree::Search(TreeNode* t, Type x)
   {   if (t == NULL) return 0;
       else if (x == t->data) return t;
       else if (x<t->data) return (Search(t->lchild,x));
       else return (Search(t->rchild, x));
   }




 #define NULL 0

   TreeNode* BSTree::ISearch(Type x)
   {   bool notfound = true;
       TreeNode* t = tree;
       while ((t) && (notfound)) {
          if (x == t->data) notfound = false;
          else if (x<t->data) t = t->lchild;
          else t = t->rchild;
       }
       if (notfound) return NULL;
       else return t;
   }


#define NULL 0

   class TreeNode{
      friend class BSTree;
      private:
         TreeNode *lchild, *rchild;
         Type data; int leftsize;
   };
             // ...

   TreeNode* BSTree::Searchk(int k)
   {   bool notfound = 1; TreeNode* t = tree;
       while ((t) && (notfound)) {
          if (k == (t->leftsize)) notfound = false;
          else if (k<(t->leftsize)) t=t->lchild;
          else {
             k -= (t->leftsize);
             t = t->rchild;
          }
       }
       if (notfound) return NULL;
       else return t;
   }

#define NULL 0
   bool Heap::Insert(Type item)
   {   // Inserts item.
       int i = ++Nel;
       if (i==MaxSize) { cout << "heap size exceeded"
                         << endl; return false;
       }
       while ((i>1) && (array[i/2]<item)) {
          array[i] = array[i/2]; i /= 2;
       }
       array[i] = item; return true;
   }

   void BSTree::Insert(Type x)
   // Insert x into the binary search tree.
   {   bool notfound=true;
       TreeNode *p = tree, *q;

       // Search for x. q is parent of p.
       while ((p) && (notfound)) {
          q = p; // Save p.
          if (x == (p->data)) notfound = false;
          else if (x < (p->data)) p = (p->lchild);
          else p = (p->rchild);
       }

       // Perform insertion.
       if (notfound) {
          p = new TreeNode;
          p->lchild = NULL; p->rchild = NULL; p->data = x;
          if (tree) {
             if (x < (q->data)) q->lchild = p;
             else q->rchild = p;
          }
          else tree = p;
       }
   }


class Heap
   {
       private:
          Type *array;
          int MaxSize, Nel;
             // Max. size of the heap, no. of elements
          void Adjust(Type a[], int i, int n);
       public:
          Heap(int MSize): MaxSize(MSize)
          { array = new Type[MaxSize+1]; Nel=0; }
          ~Heap() { delete []array; }
          bool Insert(Type item);
             // Insert item.
          bool DelMax(Type& item);
             // Delete the maximum.
   };



   bool Heap::Insert(Type item)
   {   // Inserts item.
       int i = ++Nel;
       if (i==MaxSize) { cout << "heap size exceeded"
                         << endl; return false;
       }
       while ((i>1) && (array[i/2]<item)) {
          array[i] = array[i/2]; i /= 2;
       }
       array[i] = item; return true;
   }


 void Heap::Adjust(Type a[], int i, int n)
   // The complete binary trees with roots 2*i and 2*i+1 are
   // combined with node i to form a heap rooted at i. No
   // node has an address greater than n or less than 1.
   {
       int j = 2*i, item = a[i];
       while (j <= n) {
          if ((j<n) && (a[j]<a[j+1])) j++;
             // Compare left and right child
             // and let j be the larger child.
          if (item >= a[j]) break;
             // A position for item is found.
          a[j/2] = a[j]; j *= 2;
       }
       a[j/2] = item;
   }

   bool Heap::DelMax(Type& item)
   {   if (!Nel) { cout << "heap is empty"
                   << endl; return false;
       }
       item=array[1]; array[1]=array[Nel--];
       Adjust(array, 1, Nel); return true;
   }


  void Heapify(Type a[], int n)
   // Readjust the elements in a[1:n] to form a heap.
   {
       for (int i=n/2; i; i--) AdjustH(a, i, n);
   }
   void HeapSort(Type a[], int n)
   // a[1:n] contains n elements to be sorted. HeapSort
   // rearranges them in-place into nondecreasing order.
   {
       Heapify(a, n); // Transform the array into a heap.
       // Interchange the new maximum with the element
       // at the end of the array.
       for (int i=n; i>=2; i--) {
          Type t = a[i]; a[i] = a[1]; a[1]=t;
          AdjustH(a, 1, i-1);
       }
   }


class Sets
   {
       private:
          int *p, n;
       public:
          Sets(int Size): n(Size)
          {   p = new int[n+1];
              for (int i=0;i<=n;i++)
                 p[i]=-1;
          }
          ~Sets() {delete []p;}
          void SimpleUnion(int i, int j);
          int SimpleFind(int i);
   };

   void Sets::SimpleUnion(int i, int j)
   {   p[i] = j;   }

   int Sets::SimpleFind(int i)
   {   while (p[i]>=0) i = p[i];
       return i;
   }


typedef struct list_item
{
  pq_object *content;
  struct list_item *next;
} list_item;

typedef list_item pq;


pq *create_pq(void)
{
  pq *p;
  
  p = (pq *) malloc(sizeof(pq));
  if(p==NULL)    
    {
      printf("OUT OF MEMORY!\n");
      exit(1);
    }

  p->next=NULL;
  return p;
}

boolean pq_is_empty(pq *p)
{
  return( (p->next==NULL) ? TRUE : FALSE ); 
}

pq_object *find_min(pq *p)
{
  list_item *winner, *candidate;
  
  if(pq_is_empty(p)==TRUE)
    {
      printf("TRIED TO FIND A MIN-OBJECT IN AN EMPTY PRIORITY QUEUE!\n");
      exit(1);
    }

  winner=p->next;
  for(candidate=p->next->next; candidate; candidate=candidate->next)
    {
      if(candidate->content->pq_cost < winner->content->pq_cost)
        winner=candidate;
    }
  return winner->content;
}

void pq_insert(pq *p, pq_object *object, cost object_cost)
{
  list_item *new_item;
  
  new_item = (list_item *) malloc(sizeof(list_item));
  if(new_item==NULL)
    {
      printf("OUT OF MEMORY!\n");
      exit(1);
    }

  object->pq_cost=object_cost;
  new_item->content=object;

  new_item->next=p->next;
  p->next=new_item;
}

void delete_min(pq *p)
{
  list_item *winner, *candidate, *previous;
  
  if(pq_is_empty(p)==TRUE)
    {
      printf("TRIED TO FIND A MIN-OBJECT IN AN EMPTY PRIORITY QUEUE!\n");
      exit(1);
    }

  winner=p->next;
  for(candidate=p->next->next; candidate; candidate=candidate->next)
    {
      if(candidate->content->pq_cost < winner->content->pq_cost)
  winner=candidate;
    }
  for(previous=p; previous->next != winner; previous=previous->next);

  previous->next=winner->next;
  free(winner);
}

void reduce_cost(pq *p, pq_object *object, cost smaller_cost)
{
  object->pq_cost=smaller_cost;
}





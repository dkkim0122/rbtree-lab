#include "rbtree.h"
#include <stdio.h>

#include <stdlib.h>
/*
typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;
*/


/*
  new_rbtree : 트리를 새로 선언한다.
  - rbtree struct 하나의 메모리를 동적할당하고, 그 메모리를 가리키는 포인터 변수 p를 선언.
  - p->nil의 메모리를 동적할당하여야 한다
  -> rbtree struct 안에는 nil을 가리키는 포인터 변수만 있지, nil 자체의 메모리가 할당되어 있지는 않다!
*/
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));  // rbtree 1개 만큼의 메모리 사이즈 동적할당
  // TODO: initialize struct if needed

  // nil의 데이터를 할당해주어야 한다.
  p->nil = calloc(1, sizeof(node_t));
  
  // nil
  p->nil->color = RBTREE_BLACK;

  // root
  p->root = p->nil;

  return p;
}


/*
  left_rotate : 왼쪽 회전
  1. y를 설정한다.
  2. y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮긴다.
  3. x의 부모를 y의 부모로 연결한다.
  4. x를 y의 왼쪽 서브트리로 만든다.
*/
void left_rotate(rbtree* t, node_t* x){
  // 1.
  node_t* y = x->right; 
  
  // 2.
  x->right = y->left; 
  if (y->left != t->nil)
    y->left->parent = x;
  
  // 3.
  y->parent = x->parent;  
  if (x->parent == t->nil)
    t->root = y;
  else if (x->parent->left == x)
    x->parent->left = y;
  else
    x->parent->right = y;

  // 4.
  y->left = x;
  x->parent = y;
}

/*
  right_rotate : 오른쪽 회전
  1. x를 정의한다.
  2. x의 오른쪽 서브트리를 y의 왼쪽 서브트리로 옮긴다.
  3. y의 부모를 x의 부모로 옮긴다.
  4. y를 x의 오른쪽 서브트리로 만든다.
*/
void right_rotate(rbtree* t, node_t* x){
  // 1.
  node_t* y = x->left;

  // 2.
  x->left = y->right;
  if (y->right != t->nil)
    y->right->parent = x;
  
  // 3.
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x->parent->left == x)
    x->parent->left = y;
  else
    x->parent->right = y;
  
  // 4.
  y->right = x;
  x->parent = y;
}


/*
  delete_rbtree && delete_rbtree_node: 트리의 모든 메모리 삭제
  - 모든 노드들을 순회해야 한다
  - postorder로 순회해야 자식들이 다 지워지고 나서 부모가 지워진다.

  - NIL이 아닌 모든 노드들을 지운 후 NIL을 없앤다.
*/
void delete_rbtree_node(rbtree* t, node_t* n){
  if (n == t->nil)
    return;

  delete_rbtree_node(t, n->left);
  delete_rbtree_node(t, n->right);
  free(n);  
  n = NULL;  // free 후 포인터 값을 NULL로 만들어주는 것이 후에 이 주소값에 access할 때 안전하다.
 }

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory

  // 모든 노드 삭제
  delete_rbtree_node(t, t->root);
  
  free(t->nil); // NIL의 메모리 삭제
  t->nil = NULL;  // free 후 

  free(t); // tree 자체의 메모리
  t = NULL;
}

/*
  fixup : 위반된 RBT 노드를 수정한다.
  1. 새 노드 z의 색을 RED로 칠했으므로, z의 부모 색이 RED일 시 실행한다.
  2. z의 삼촌도 RED이면 Recolor를 진행한다
  - z의 부모와 삼촌은 BLACK으로 칠한다.
  - 노드의 black height를 맞추기 위해 조부모를 RED로 칠한다.
  - 조부모가 RED가 되었으므로, 새로운 z로 설정하고 규칙에 위반될 시 다시 fixup을 수행한다.
  3. z의 삼촌이 BLACK이면 크게 두 가지 경우로 나눈다.
  1) z와 부모, 조부모가 triangle 형태 : 부모를 rotation하고 새 z를 부모로 설정한다.
  2) z와 부모, 조부모가 line 형태 : 조부모를 rotation하고, 부모와 조부모의 색깔을 바꾼다.
  1)을 거치면 무조건적으로 2)가 나오고, 2)를 거치면 무조건 균형 이진 트리가 나온다.
  4. 루트는 항상 BLACK이어야 한다.
*/
void rbtree_fixup(rbtree* t, node_t* z){
  // 1. 부모의 색이 RED일 때 실행
  while(z->parent->color == RBTREE_RED){
    // 부모가 조부모의 왼쪽 노드일때부터 시작한다.
    if (z->parent == z->parent->parent->left){ 
      node_t* uncle = z->parent->parent->right;  // 삼촌
      
      // 2. Recolor
      if (uncle->color == RBTREE_RED){
        z->parent->parent->color = RBTREE_RED;
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;

        z = z->parent->parent;  // z의 조부모가 규칙에 어긋나면 fixup 다시 실행(while문)
      }

      // 3. Restructure
      else{
        // triangle
        if (z == z->parent->right){
          z = z->parent;
          left_rotate(t, z);
        }

        // line
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }

    // 부모가 조부모의 오른쪽 노드일 때
    else{
      node_t* uncle = z->parent->parent->left;

      // Recolor
      if (uncle->color == RBTREE_RED){
        z->parent->parent->color = RBTREE_RED;
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;

        z = z->parent->parent;
      }

      // Reconstruct
      else{
        if (z == z->parent->left){
          z = z->parent;
          right_rotate(t, z);
        }

        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }

    }
  }
  
  // 4. 만약 새 노드가 ROOT라면.
  t->root->color = RBTREE_BLACK;
}


/*
  insert(key) : key 값을 가진 노드를 트리에 삽입한다.
  1. 이진탐색트리와 동일하게 새 노드를 트리에 삽입한다.
  2. 새 노드의 색을 RED로 칠한다.
  3. RBT의 규칙에 어긋날 시, 규칙을 지키기 위해 트리를 fixup한다.
*/
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t* newNode = malloc(sizeof(node_t));
  newNode->key = key;

  // 새 노드의 부모 노드를 찾는다.
  node_t* parent = t->nil;
  node_t* curr = t->root;

  while (curr != t->nil){
    parent = curr;
    if (key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;
  }
  newNode->parent = parent;

  // 새 노드의 자리를 찾아 삽입한다(원래 있던 자릴 대체하진 않는다).
  if (parent == t->nil)
    t->root = newNode;
  else if (key < parent->key)
    parent->left = newNode;
  else
    parent->right = newNode;
  
  newNode->left = t->nil;
  newNode->right = t->nil;
  
  // 새 노드의 색을 RED로 칠하고 트리를 fixup한다.
  newNode->color = RBTREE_RED;
  rbtree_fixup(t, newNode);  // 인자가 이게 맞나?

  return t->root;
}



node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find

  node_t* n = t->root;

  while (n != t->nil && n->key != key){
      if (key < n->key)
        n = n->left;
      else
        n = n->right;
    }

  if (n == t->nil)
    return NULL;
  else
    return n;
}


/*
  min, max : 키 값이 최소/최대인 노드를 찾아주는 함수.
  - RBT는 BST의 일종이므로, 트리에서 가장 왼쪽에 있는 노드가 최소, 오른쪽에 있는 노드가 최대이다.
*/
node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find

  node_t* parent = t->nil;
  node_t* curr = t->root;

  while (curr != t->nil){
    parent = curr;
    curr = curr->left;
  }

  return parent;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find

  node_t* parent = t->nil;
  node_t* curr = t->root;

  while (curr != t->nil){
    parent = curr;
    curr = curr->right;
  }
    
  return parent;
}

node_t* find_min_successor(const rbtree *t, const node_t* node){
  node_t* curr= node->right;

  while (curr->left != t->nil)
    curr = curr->left;

  return curr;
}

/*
  transplant : 이식, 기존 노드 u의 자리를 다른 노드 v가 대체하는 함수.
  1. u의 부모를 v에 연결시킨다(부모->자식).
  2. v를 새 부모에 연결시킨다(자식->부모).
  => 기존 노드의 부모와만 연결시키지, 그 노드의 자식들과는 따로 연결해야 한다.
*/
void rbtree_transplant(rbtree* t, node_t* u, node_t* v){
  
  // 1. 
  if (u->parent == t->nil)
    t->root = v;
  else if(u == u->parent->left)
    u->parent->left = v;
  else 
    u->parent->right = v;

  // 2.
  v->parent = u->parent;  
}

/*
  erase_fixup : 만약 지워진 노드의 색깔이 BLACK일 경우, 조건5를 만족시켜주기 위해 조정해야 한다.
  1. 대체 노드 x.color==RED이다(적/흑 노드) : 그냥 x.color=BLACK 하면 된다.
  2. t->root==x인 경우 : 그냥 x.color==BLACK하면 된다.
  3. x.color==BLACK인 경우 : 회전 및 색칠을 적절히 수행해야 한다.
  CASE1 : x의 sibling.color == RED
  - 조정을 통해 case 2, 3, 4(sibling.color==BLACK)으로 바꿔줄 것이다.
*/
void rbtree_erase_fixup(rbtree* t, node_t* x){
  node_t* w;

  // x의 색깔이 BLACK일 때가 문제가 된다.
  while (x!=t->root && x->color == RBTREE_BLACK){
    // 먼저 x가 왼쪽 노드일 때부터 본다.
    if (x == x->parent->left){ 
      w = x->parent->right;  //sibling w의 정의

      // CASE1 w.color == RED : CASE 2,3,4 중 하나로 바꾼다.
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right; // rotate후 다시 w 설정
      }
      // w.color == BLACK인 경우들
      // CASE2 : while문을 탈출하거나 새로운 x로 while문을 계속 돌거나.
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;  // x와 w에서 BLACK을 하나씩 없애기. 밑에서 이 B를 x.parent로 올린다.
        x = x->parent;  // x.parent를 새 x로 두고 while문 돌기
        // x의 부모가 BLACK이거나 root일 때 while문이 종료된다.
      }
      // CASE3 : CASE4로 바꾼다.
      else {
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;  // w와 w.left 색 바꾸기
          right_rotate(t, w);
          w = x->parent->right; // rotate 후 다시 w 설정
        }
        // CASE4 : w.color==B, w.right.color==R : 조정 후 끝.
        w->color = w->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);

        x = t->root;  // while문 탈출조건 및 root 처리 -> while문 종료!

      }

    }

    // x가 오른쪽 노드일 때
    else{
      w = x->parent->left;
      // CASE1
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // CASE2
      if (w->right->color==RBTREE_BLACK && w->left->color==RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      // CASE3
      else {
        if(w->left->color==RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
      }
      // CASE4
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(t, x->parent);

      x = t->root;
      }
    }
  }

  /*
    애초에
    - x가 R + 여분의 B일 때 : R 대신 B로 덮어씌우면 된다.
    - x가 root일 때 여분의 B 제거 가능
    x가 Double B일 때
    - while문 종료 후 CASE2에서 x.parent(새로운 x, 색깔 R)에 B 덮기
  */
  x->color = RBTREE_BLACK;
}

/*
  erase : tree의 node p를 지우는 함수.
  - p:지워질 노드 / y:지워질 노드(case1,2), 대체할 노드(case3) / x:y의 자리를 대체할 노드 
  - y의 색깔을 먼저 저장해 둔다.
  case1, 2 : 노드의 한 자식이 nil인 경우(두개 다 nil인 경우도 포함)
  1. 나머지 자식을 x로 두고 y와 대체한다.
  case 3 : 노드의 두 자식 모두 nil이 아닌 경우
  - y는 p의 successor가 된다. 이 때 y가 바꼈으므로 새로운 y의 색을 저장한다.
  - x는 y의 right child가 된다. y의 자리에 x가 들어오면서 RBT 규칙에 어긋나는 경우가 생길 수 있다.
  1. y의 자리에 x를 교체한다. y는 이제 동떨어진 노드가 됐다.
  2. 만약 y가 p의 직접적인 자식이라면, transplant(y, x)를 하지 말기(x.p = y.p).
  3. y.p는 없어지고 그 자리를 y가 차지할 예정이므로 그냥 x.p를 y로 두어야 한다.
  4. 그게 아니라면 transplant하여 y와 x를 바꿔준 이후 
  5. 동떨어진 y와 p의 right child를 연결시켜준다(if의 경우에는 그냥 y가 p.right이므로 필요없음).
  6. 그 후 y와 p를 transplant하고 y를 p의 left child를 연결시켜줌으로써 p를 떨어뜨려 놓는다.
  7. 이 때 y의 색깔을 원래 p의 색깔로 덮는다. 이제 p와 y에 대해서는 RBT 규칙을 기억할 필요가 없다.
  8. 대신 대체된(삭제된) y의 색깔 때문에 RBT 규칙이 어긋날 수 있다. 
  - y가 RED면 상관이 없다. 모든 노드에 대해 black height는 동일하다.
  - y가 BLACK이면 black height가 문제가 된다. 이를 위해서 fixup 함수를 실행한다.
  9. p의 메모리를 free한다.
*/
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t* y = p;
  node_t* x;
  color_t y_original_color = y->color;  

  // case 1 : p의 왼쪽 자식과 nil -> 오른쪽 자식과 교체한다.
  if (p->left == t->nil){
    x = p->right;
    rbtree_transplant(t, p, p->right);
  }
  // case 2 : p의 오른쪽 자식과 nil -> 왼쪽 자식과 교체한다.
  else if(p->right == t->nil){
    x = p->left;
    rbtree_transplant(t, p, p->left);
  }
  // case 3 : p의 왼쪽, 오른쪽 자식이 둘 다 nil이 아님
  else{
    y = find_min_successor(t, p);
    y_original_color = y->color;
    x = y->right;

    // y와 x를 교체한다. y를 떨어뜨려 놓는다. else의 경우 y와 p.right을 미리 연결시킨다.
    if (y->parent == p)
      x->parent = y;
    else{
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }

    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  if (y_original_color == RBTREE_BLACK)
    rbtree_erase_fixup(t, x);

  // p의 메모리 공간을 free한다.
  free(p);

  return 0;
}


/*
  rbtree_to_array && recur_to_array : 트리 구조를 배열로 만드는 함수
  1. 트리 내의 노드들을 키 값 오름차순으로 나타내기로 하자 -> 중위 순회를 하여야 한다.
  2. 중위 순회를 위해 재귀를 사용한다.
  3. 재귀 시, 배열의 인덱스를 파라미터로 집어넣는다.
  4. 그 인덱스 값 i를 리턴하여야 i의 값이 재귀 시마다 갱신될 수 있다.
  5. left child로 넘어갈 시 i는 증가하지 않아야 한다. right child로 넘어갈 시에도 증가하지 않는다.
  6. 해당 노드에 도착했을 때 그 노드의 key 값을 배열에 저장하고, 그 다음 i의 값을 증가시켜야 한다.
*/
key_t* recur_for_array(const rbtree *t, key_t *arr, node_t* x){

  if (x == t->nil)
    return arr;

  arr = recur_for_array(t, arr, x->left);
  *arr = x->key;
  arr += 1;
  arr = recur_for_array(t, arr, x->right);

  return arr;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array

  recur_for_array(t, arr, t->root);

  return 0;
}
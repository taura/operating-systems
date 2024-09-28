/* 
 * openmp_queue.c
 *  gcc -fopenmp openmp_queue.c
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <omp.h>

/* FIFO キューのノード */
typedef struct queue_node {
  struct queue_node * next;	/* 次のノードへのポインタ */
  int val;			/* 格納された値 */
} * queue_node_t;

/* FIFO キュー全体の構造(先頭ノードと終端ノードへのポインタ) */
typedef struct queue {
  queue_node_t head;		/* 先頭 */
  queue_node_t tail;		/* 終端 */
} * queue_t;

void die(char * s) {
  perror(s); exit(1);
}

/* 新しい空のキューを作る */
queue_t new_queue() {
  queue_t q = (queue_t)malloc(sizeof(struct queue));
  if (q == NULL) die("malloc");
  q->head = q->tail = NULL;
  return q;
}

/* キューq の末尾に要素 xを挿入 */
void enq(queue_t q, int x) {
  /* ノードを作る */
  queue_node_t n = (queue_node_t)malloc(sizeof(struct queue_node));
  if (n == NULL) die("malloc");
  n->next = NULL;
  n->val = x;
  if (q->tail) {
    /* もともと空じゃなければ現在の終端ノード -> 新ノード */
    q->tail->next = n;
  } else {
    /* もともと空ならば, head -> 新ノード */
    q->head = n;
  }
  q->tail = n;
}

/* キューから要素を削除.
   ただしここでは空だったら -1 を返す */
int deq(queue_t q) {
  queue_node_t h = q->head;
  if (h) {
    q->head = h->next;
    if (h->next == NULL) q->tail = NULL;
    return h->val;
  } else {
    /* 空 ----> -1 を返す */
    return -1;
  }
}
/* FIFO キュー終わり */



void * thread_func1(int idx, queue_t q, char * a,
		    int n_items, int n_enq_threads, int n_deq_threads) {
  if (idx < n_enq_threads) {
    /* 挿入するスレッド. n_enq_threads スレッドで分担して, 
       0, 1, 2, ..., n_items-1 を一度ずつ挿入する */
    int i;
    /* [my_items_beg,my_items_end) = 自分が挿入する範囲 */
    int my_items_beg = (n_items * idx)       / n_enq_threads;
    int my_items_end = (n_items * (idx + 1)) / n_enq_threads;
    /* 挿入(enq) */
    for (i = my_items_beg; i < my_items_end; i++) {
      enq(q, i);
    }
  } else {
    /* 削除(deq) */
    int deq_idx = idx - n_enq_threads;
    /* n_deq_threadsスレッドが分担して合計, n_items 回 deq を発効する.
       挿入した全要素が一度ずつとり出されるはずである */
    int my_items_beg = (n_items * deq_idx)       / n_deq_threads;
    int my_items_end = (n_items * (deq_idx + 1)) / n_deq_threads;
    int i;
    for (i = my_items_beg; i < my_items_end; i++) {
      int x = deq(q);
      assert(x != -1);	      /* (i) 空であってはいけない */
      assert(a[x] == 0); /* (ii) 同じ要素が2度とり出されてはいけない */
      a[x] = 1;	      /* xが取り出されたと記録 */
    }
  }
  return 0;
}


double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

int main(int argc, char ** argv)
{
  int n_items       = (argc > 1 ? atoi(argv[1]) : 100000); /* 挿入/削除される要素数 */
  int n_enq_threads = (argc > 2 ? atoi(argv[2]) : 1);	   /* 挿入役のスレッド数 */
  int n_threads     = omp_get_max_threads();
  assert(n_enq_threads < n_threads);
  int n_deq_threads = n_threads - n_enq_threads;
  queue_t q = new_queue();
  char * a = calloc(1, n_items);
  int i;
  double t0 = cur_time();
  

#pragma omp parallel
  thread_func1(omp_get_thread_num(),   // omp_get_thread_num() が現在のスレッドの番号を与える
	       q, a, n_items, n_enq_threads, n_deq_threads);

  /* すべての要素が取り出されているかチェック */
  for (i = 0; i < n_items; i++) {
    assert(a[i] == 1);
  }
  double t1 = cur_time();
  printf("OK: elapsed time: %f\n", t1 - t0);
  printf("Performance: %f items/sec\n", n_items / (t1 - t0));
  return 0;
}


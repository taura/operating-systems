#define _GNU_SOURCE
#include <pthread.h>
#include <omp.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* 有限バッファ: API仕様

   (a) bounded_buffer * bb = mk_bounded_buffer();
       空の有限バッファを作る

   (b) int r = bb_enq(bb, long x);  
       要素xをbbに格納. x>=0 とする(制限の理由は後述)
       必ず 0 を返す(そもそもなぜ値を返す必要があるのかは後述)

   (c) long x = bb_deq(bb);
       1要素取り出す

注:
   (1) 要素数に上限があり，満杯の時にbb_enqすると,
       誰かがbb_deqするまで待つ(リターンしない)
   (2) 空の時に bb_deqすると，誰かがbb_enqするまで待つ
       (リターンしない)

これを，複数スレッドが bb_enq, bb_deq しても大丈夫なように，
正しく作るのが目標
*/

/* 
   以下は，不完全なバージョンで，

   (2') 要素数に上限があり，満杯の時にbb_enqすると,
        直ちに -1 を返す 
        ((b)により，満杯でなければ0, 満杯なら-1が返る)
   (3') 空の時にbb_dnqすると, 直ちに -1 を返す
        ((b)で，要素は >= 0を仮定しているので -1 iff 空)

 */


/* 有限バッファの1要素 (long) */
typedef struct {
  long val;
} item;

/* C = 有限バッファの容量 */
#define C 8


/* 有限バッファのデータ構造

               h%C            t%C
                |              |
         +----+----+----+----+----+----+----+----+
   items |    | x  | x  | x  |    |    |    |    |
         +----+----+----+----+----+----+----+----+
              |<-- 要素あり->|


   t : これまでにenqされた要素の合計数(単調増加)
   h : これまでにdeqされた要素の合計数(単調増加)

   i番目にenqされた要素は items[i % C] に入る．
   つまりitemsを循環しながら用いる(循環バッファ)

   t % C : 次のenqが要素を入れる場所
   h % C : 次のdeqが要素を取り出す場所

  現在の要素数 = t - h
   特に，
     空 <==> h == t
   満杯 <==> h + C == t

 */
typedef struct {
  volatile long h; /* the slot to consume the next item from */
  volatile long t; /* the slot to insert the next item to */
  item items[C];
} bounded_buffer;

/* 空の有限バッファを作る */
bounded_buffer * mk_bounded_buffer() {
  bounded_buffer * bb= malloc(sizeof(bounded_buffer));
  long i;
  bb->h = bb->t = 0;
  for (i = 0; i < C; i++) {
    bb->items[i].val = 0; /* 不要だが一応初期化 */
  }
  return bb;
}
  
enum { 
  BB_FULL = -1,
  BB_EMPTY = -1
};

/* 要素を挿入．だいたい， 
   bb->items[bb->t % C] = val; 
   bb->t++; */
int bb_enq(bounded_buffer * bb, long val) {
  long t = bb->t++;
  if (!(t - bb->h < C)) return BB_FULL;
  bb->items[t % C].val = val;
  return 0;
}

/* 要素を挿入．だいたい， 
   val = bb->items[bb->h % C];
   bb->h++; */
long bb_deq(bounded_buffer * bb) {
  long h = bb->h++;
  if (!(h < bb->t)) return BB_EMPTY;
  long val = bb->items[h % C].val;
  return val;
}

/* begin ... end - 1 までを bb に挿入 */
void * enq_items(bounded_buffer * bb, long begin, long end) {
  long x;
  for (x = begin; x < end; x++) {
    int r = bb_enq(bb, x);
    assert(r == 0);
  }
  return 0;
}

/* n_to_dequeue 回, bb から取り出し */
void * deq_items(bounded_buffer * bb, int n_to_dequeue,
		 char * dequeued, int n_items) {
  int i;
  for (i = 0; i < n_to_dequeue; i++) {
    long val = bb_deq(bb);
    assert(val >= 0);
    assert(val < n_items);
    /* 同じ値が2回とり出されていないことをチェック */
    assert(dequeued[val] == 0);
    dequeued[val] = 1;
  }
  return 0;
}

void * thread_func(bounded_buffer * bb, 
		   char * dequeued, long n_items,
		   int n_enq_threads, int n_deq_threads) {
  int my_idx = omp_get_thread_num();
  /* my_idxに応じて，enq する人か deq する人になる */
  if (my_idx < n_enq_threads) {
    /* enq する */
    int idx = my_idx;
    long begin = (n_items *  idx     ) / n_enq_threads;
    long end   = (n_items * (idx + 1)) / n_enq_threads;
    return enq_items(bb, begin, end);
  } else {
    /* deq する */
    int idx = my_idx - n_enq_threads;
    long begin = (n_items *  idx     ) / n_deq_threads;
    long end   = (n_items * (idx + 1)) / n_deq_threads;
    return deq_items(bb, end - begin, dequeued, n_items);
  }
}

/* 全部の値が取り出されていることをチェック */
void check_dequeued(char * dequeued, int n_items) {
  int i;
  for (i = 0; i < n_items; i++) {
    assert(dequeued[i]);
  }
}

double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, 0);
  return tp->tv_sec + 1.0e-6 * tp->tv_usec;
}

int main(int argc, char ** argv) {
  if (argc <= 3) {
    fprintf(stderr, 
	    "usage: %s n_items n_enq_threads n_deq_threads\n",
	    argv[0]);
    fprintf(stderr, "example:\n   %s 1000000 8 8\n",
	    argv[0]);
    exit(1);
  }
  /* 挿入される要素数 */
  long n_items      = (argc > 1 ? atol(argv[1]) : 10000000);
  /* enq スレッドの数 */
  int n_enq_threads = (argc > 2 ? atoi(argv[2]) : 1);
  assert(n_enq_threads > 0);
  /* deq スレッドの数 */
  int n_deq_threads = (argc > 3 ? atoi(argv[3]) : 1);
  assert(n_deq_threads > 0);

  /* チェック用配列 */
  char * dequeued = (char*)calloc(sizeof(char), n_items);

  bounded_buffer * bb = mk_bounded_buffer();
  printf("%d threads to enqueue, %d threads to dequeue\n",
	 n_enq_threads, n_deq_threads);

  double t0 = cur_time();
#pragma omp parallel num_threads(n_enq_threads + n_deq_threads)
  {
    thread_func(bb, dequeued, n_items, n_enq_threads, n_deq_threads);
  }
  double t1 = cur_time();
  double dt = t1 - t0;
  check_dequeued(dequeued, n_items);
  printf("OK\n");
  printf("%ld items enqueued/dequeued in %f sec\n", 
	 n_items, dt);
  printf("%f items/sec\n", 
	 (double)n_items/(double)dt);
  return 0;
}

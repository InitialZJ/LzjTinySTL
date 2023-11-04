#ifndef MYTINYSTL_QUEUE_TEST_H_
#define MYTINYSTL_QUEUE_TEST_H_

// queue test : 测试 queue, priority_queue 的接口和它们 push 的性能

#include <queue>

#include "../MyTinySTL/queue.h"
#include "test.h"

namespace mystl {
namespace test {
namespace queue_test {

void queue_print(mystl::Queue<int> q) {
  while (!q.empty()) {
    std::cout << " " << q.front();
    q.pop();
  }
  std::cout << std::endl;
}

void p_queue_print(mystl::PriorityQueue<int> q) {
  while (!q.empty()) {
    std::cout << " " << q.top();
    q.pop();
  }
  std::cout << std::endl;
}

//  queue 的遍历输出
#define QUEUE_COUT(q)                   \
  do {                                  \
    std::string q_name = #q;            \
    std::cout << " " << q_name << " :"; \
    queue_print(q);                     \
  } while (0)

// priority_queue 的遍历输出
#define P_QUEUE_COUT(p)                 \
  do {                                  \
    std::string p_name = #p;            \
    std::cout << " " << p_name << " :"; \
    p_queue_print(p);                   \
  } while (0)

#define QUEUE_FUN_AFTER(con, fun)                 \
  do {                                            \
    std::string fun_name = #fun;                  \
    std::cout << " After " << fun_name << " :\n"; \
    fun;                                          \
    QUEUE_COUT(con);                              \
  } while (0)

#define P_QUEUE_FUN_AFTER(con, fun)               \
  do {                                            \
    std::string fun_name = #fun;                  \
    std::cout << " After " << fun_name << " :\n"; \
    fun;                                          \
    P_QUEUE_COUT(con);                            \
  } while (0)

void queue_test() {
  std::cout << "[===============================================================]" << std::endl;
  std::cout << "[----------------- Run container test : queue ------------------]" << std::endl;
  std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
  int a[] = {1, 2, 3, 4, 5};
  mystl::Deque<int> d1(5);
  mystl::Queue<int> q1;
  mystl::Queue<int> q2(5);
  mystl::Queue<int> q3(5, 1);
  mystl::Queue<int> q4(a, a + 5);
  mystl::Queue<int> q5(d1);
  mystl::Queue<int> q6(std::move(d1));
  mystl::Queue<int> q7(q2);
  mystl::Queue<int> q8(std::move(q2));
  mystl::Queue<int> q9;
  q9 = q3;
  mystl::Queue<int> q10;
  q10 = std::move(q3);
  mystl::Queue<int> q11{1, 2, 3, 4, 5};
  mystl::Queue<int> q12;
  q12 = {1, 2, 3, 4, 5};

  QUEUE_FUN_AFTER(q1, q1.push(1));
  QUEUE_FUN_AFTER(q1, q1.push(2));
  QUEUE_FUN_AFTER(q1, q1.push(3));
  QUEUE_FUN_AFTER(q1, q1.pop());
  QUEUE_FUN_AFTER(q1, q1.emplace(4));
  QUEUE_FUN_AFTER(q1, q1.emplace(5));
  std::cout << std::boolalpha;
  FUN_VALUE(q1.empty());
  std::cout << std::noboolalpha;
  FUN_VALUE(q1.size());
  FUN_VALUE(q1.front());
  FUN_VALUE(q1.back());
  while (!q1.empty()) {
    QUEUE_FUN_AFTER(q1, q1.pop());
  }
  QUEUE_FUN_AFTER(q1, q1.swap(q4));
  QUEUE_FUN_AFTER(q1, q1.clear());
  PASSED;
#if PERFORMANCE_TEST_ON
  std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  std::cout << "|         push        |";
#if LARGER_TEST_DATA_ON
  CON_TEST_P1(queue<int>, Queue<int>, push, rand(), SCALE_LL(LEN1), SCALE_LL(LEN2), SCALE_LL(LEN3));
#else
  CON_TEST_P1(queue<int>, Queue<int>, push, rand(), SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
#endif
  std::cout << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  PASSED;
#endif
  std::cout << "[----------------- End container test : queue ------------------]" << std::endl;
}

void priority_test() {
  std::cout << "[===============================================================]" << std::endl;
  std::cout << "[------------- Run container test : priority_queue -------------]" << std::endl;
  std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
  int a[] = {1, 2, 3, 4, 5};
  mystl::Vector<int> v1(5);
  mystl::PriorityQueue<int> p1;
  mystl::PriorityQueue<int> p2(5);
  mystl::PriorityQueue<int> p3(5, 1);
  mystl::PriorityQueue<int> p4(a, a + 5);
  mystl::PriorityQueue<int> p5(v1);
  mystl::PriorityQueue<int> p6(std::move(v1));
  mystl::PriorityQueue<int> p7(p2);
  mystl::PriorityQueue<int> p8(std::move(p2));
  mystl::PriorityQueue<int> p9;
  p9 = p3;
  mystl::PriorityQueue<int> p10;
  p10 = std::move(p3);
  mystl::PriorityQueue<int> p11{1, 2, 3, 4, 5};
  mystl::PriorityQueue<int> p12;
  p12 = {1, 2, 3, 4, 5};

  P_QUEUE_FUN_AFTER(p1, p1.push(1));
  P_QUEUE_FUN_AFTER(p1, p1.push(5));
  P_QUEUE_FUN_AFTER(p1, p1.push(3));
  P_QUEUE_FUN_AFTER(p1, p1.pop());
  P_QUEUE_FUN_AFTER(p1, p1.emplace(7));
  P_QUEUE_FUN_AFTER(p1, p1.emplace(2));
  P_QUEUE_FUN_AFTER(p1, p1.emplace(8));
  std::cout << std::boolalpha;
  FUN_VALUE(p1.empty());
  std::cout << std::noboolalpha;
  FUN_VALUE(p1.size());
  FUN_VALUE(p1.top());
  while (!p1.empty()) {
    P_QUEUE_FUN_AFTER(p1, p1.pop());
  }
  P_QUEUE_FUN_AFTER(p1, p1.swap(p4));
  P_QUEUE_FUN_AFTER(p1, p1.clear());
  PASSED;
#if PERFORMANCE_TEST_ON
  std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  std::cout << "|         push        |";
#if LARGER_TEST_DATA_ON
  CON_TEST_P1(
      priority_queue<int>,
      PriorityQueue<int>,
      push,
      rand(),
      SCALE_LL(LEN1),
      SCALE_LL(LEN2),
      SCALE_LL(LEN3));
#else
  CON_TEST_P1(
      priority_queue<int>,
      PriorityQueue<int>,
      push,
      rand(),
      SCALE_L(LEN1),
      SCALE_L(LEN2),
      SCALE_L(LEN3));
#endif
  std::cout << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  PASSED;
#endif
  std::cout << "[------------- End container test : priority_queue -------------]" << std::endl;
}

}  // namespace queue_test
}  // namespace test
}  // namespace mystl

#endif  // ! MYTINYSTL_QUEUE_TEST_H_

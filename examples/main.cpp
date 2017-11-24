#include "pubsub.h"

struct St_Test {
  int i;
  int j;
};

void handle_int(int x) { cout << "hi int:" << x << endl; }
void handle_str(string x) { cout << "hi str:" << x << endl; }
void handle_struct(St_Test x) {
  cout << "hi struct, i:" << x.i << " j:" << x.j << endl;
}

int main(int argc, char **argv) {
  subscribe("topic_int", handle_int);
  // subscribe("topic_str", handle_str);
  // subscribe("topic_struct", handle_struct);

  vector<thread> arrThread;
  arrThread.push_back(thread([]() {
    while (true) {
      this_thread::sleep_for(chrono::milliseconds(200));
      publish("topic_int", 1);
    }
  }));

  // arrThread.push_back(thread([]() {
  //   while (true) {
  //     this_thread::sleep_for(chrono::milliseconds(100));
  //     publish("topic_str", "string");
  //   }
  // }));
  // arrThread.push_back(thread([]() {
  //   while (true) {
  //     St_Test st;
  //     st.i = 10;
  //     st.j = 20;
  //     publish("topic_struct", st);
  //   }
  // }));

  for (auto &t : arrThread) {
    t.join();
  }

  getchar();
}

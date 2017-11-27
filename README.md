## lightweight pub sub framework between thread (ros like)
 lps is a very little and fast C++ thread-safe implementation of publish and subscribe system which is based on modern C++11(boost) code

## Dependencies
1. c++11
2. boost signals2(head only), while any signal slots system is ok!!
3. [Concurrent Queue](https://github.com/cameron314/concurrentqueue)


## How to build

```bash
  mkdir build
  cd build
  cmake ..
  make
```
## Example

* subscribe one function
```
void handle_int(int x) { cout << "hi int:" << x << endl; }

int main(int argc, char **argv) {
  subscribe("topic_int", handle_int);

  vector<thread> arrThread;
  arrThread.push_back(thread([]() {
    while (true) {
      this_thread::sleep_for(chrono::milliseconds(200));
      publish("topic_int", 1);
    }
  }));
}
```

* subscribe many function
```
#include "pubsub.h"

struct Struct4Test {
  int i;
  int j;
};

// handle type integer
void handle_int(int x) { cout << "hi int:" << x << endl; }

// handle type string
void handle_str(string x) { cout << "hi str:" << x << endl; }

// handle type struct
void handle_struct(Struct4Test x) {
  cout << "hi struct, i:" << x.i << " j:" << x.j << endl;
}

int main(int argc, char **argv) {

  vector<thread> arrThread;
  arrThread.push_back(thread([]() {
    subscribe("topic_str", handle_str);
    while (true) {
      this_thread::sleep_for(chrono::milliseconds(200));
      publish("topic_int", 1);
    }
  }));

  arrThread.push_back(thread([]() {
    subscribe("topic_struct", handle_struct);
    while (true) {
      this_thread::sleep_for(chrono::milliseconds(100));
      publish("topic_str", string("string"));
    }
  }));

  arrThread.push_back(thread([]() {
    subscribe("topic_int", handle_int);
    while (true) {
      Struct4Test st;
      st.i = 10;
      st.j = 20;
      publish("topic_struct", st);
      this_thread::sleep_for(chrono::milliseconds(100));
    }
  }));

  for (auto &t : arrThread) {
    t.join();
  }

  getchar();
}
```
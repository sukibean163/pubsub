# tiny pub sub framework (ros like)

## Depends
1. c++11
2. boost signals2(head only) any signal slots system is ok!
3. [Concurrent Queue](https://github.com/cameron314/concurrentqueue)


## How to build


```bash
  mkdir build
  cd build
  cmake ..
  make
```
## Example

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
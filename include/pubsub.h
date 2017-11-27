#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
using namespace std;

#include <boost/signals2.hpp>

#include "concurrentqueue.h"
using namespace moodycamel;

// #include "signals.hpp"

class TopicInterface {
public:
  string topic;
  virtual ~TopicInterface() {}
};

typedef shared_ptr<TopicInterface> TopicInterfacePtr;
typedef vector<TopicInterfacePtr> V_TopicInterfacePtr;

template <class T> class TopicAdapter : public TopicInterface {
public:
  shared_ptr<boost::signals2::signal<void(T)>> sigTopic;
  TopicAdapter(string str) {
    sigTopic = make_shared<boost::signals2::signal<void(T)>>();
    topic = str;
  }
};

class ThreadArray {
public:
  ThreadArray() {}
  ThreadArray &operator=(ThreadArray &) = delete;

  template <class T>
  shared_ptr<boost::signals2::signal<void(T)>> Get(string strTopic) {
    std::lock_guard<std::mutex> lg(mut);
    TopicInterfacePtr topicAdapter;

    const bool is_in = mapTopic.find(strTopic) != mapTopic.end();
    if (is_in) {
      topicAdapter = mapTopic[strTopic];
    } else {
      topicAdapter = shared_ptr<TopicInterface>(new TopicAdapter<T>(strTopic));
      mapTopic[strTopic] = topicAdapter;
    }

    auto ret = static_cast<TopicAdapter<T> *>(topicAdapter.get());

    return ret->sigTopic;
  }

private:
  std::mutex mut;
  map<string, TopicInterfacePtr> mapTopic;
};

extern ThreadArray g_ta;

extern vector<thread> arrThread;

template <class T> void subscribe(string topic, std::function<void(T)> func) {
  typedef ConcurrentQueue<T> Queue;

  shared_ptr<boost::signals2::signal<void(T)>> sig = g_ta.Get<T>(topic);

  auto q_new = make_shared<Queue>(100);
  sig->connect([q_new](T t) {
    // todos: if queue full, remove top items
    q_new->try_enqueue(t);
  });
  arrThread.push_back(thread([q_new, func]() {
    while (true) {
      T t;
      bool ret = q_new->try_dequeue(t);
      if (ret) {
        func(t);
      } else {
        this_thread::sleep_for(chrono::milliseconds(1));
      }
    }
  }));
}

template <class T> void subscribe(string topic, void (*fp)(T)) {
  std::function<void(T)> f = fp;
  subscribe<T>(topic, f);
}

template <class T> void publish(string topic, T t) {
  shared_ptr<boost::signals2::signal<void(T)>> sig = g_ta.Get<T>(topic);
  if (sig) {
    (*sig)(t);
  }
}

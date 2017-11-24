#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
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
  boost::signals2::signal<void(T)> *sigTopic;
  TopicAdapter(string str) {
    sigTopic = new boost::signals2::signal<void(T)>();
    topic = str;
  }
};

extern vector<TopicInterfacePtr> g_arrTopic;

// new topic
template <class T> void Insert(string topic) {
  TopicInterfacePtr topicAdapter =
      shared_ptr<TopicInterface>(new TopicAdapter<T>(topic));
  g_arrTopic.push_back(topicAdapter);
}


template <class T> boost::signals2::signal<void(T)> *Get(string strTopic) {
  for (auto &topic : g_arrTopic) {
    auto ret = static_cast<TopicAdapter<T> *>(topic.get());
    if (ret && ret->topic == strTopic) {
      return ret->sigTopic;
    }
  }
  return nullptr;
}

extern vector<thread> arrThread;

template <class T> void subscribe(string topic, std::function<void(T)> func) {
  typedef ConcurrentQueue<T> Queue;

  boost::signals2::signal<void(T)> *sig = Get<T>(topic);

  if (sig == nullptr) {
    Insert<T>(topic);
    sig = Get<T>(topic);
    assert(sig != nullptr);
  }

  auto q_new = make_shared<Queue>(100);
  sig->connect([q_new](T t) {
    //todos: if queue full, remove top items
    q_new->try_enqueue(t);
  });
  arrThread.push_back(thread([q_new, func]() {
    while (true) {
      T t;
      bool ret = q_new->try_dequeue(t);
      if (ret) {
        func(t);
      } else {
        this_thread::sleep_for(chrono::microseconds(1));
      }
    }
  }));
}

template <class T> void subscribe(string topic, void (*fp)(T)) {
  std::function<void(T)> f = fp;
  subscribe<T>(topic, f);
}

template <class T> void publish(string topic, T t) {
  boost::signals2::signal<void(T)> *sig = Get<T>(topic);
  (*sig)(t);
}

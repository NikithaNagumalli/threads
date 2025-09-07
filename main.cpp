#include <iostream>
#include <tuple>
#include <bits/stdc++.h>
#include <ctime>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>

using std::cout;
using std::endl;
using std::tuple;
using std::make_tuple;
using std::queue;
using std::atomic;
using std::mutex;
using std::condition_variable;
using std::vector;

class ThreadPool {
    private:
        vector<std::thread> _threads;
        queue<tuple<int, time_t>> tasks;
        atomic<int> count = 0; // TODO: does this need to be atomic if it's only being updated in mutex locked section?
        mutex mtx;
        condition_variable cv;
        bool _stop = false;

        void process_task(int thread_num) {
            while (true) {
                tuple<int, time_t> task;
                {    
                    std::unique_lock<mutex> lock(mtx);
                    cv.wait(lock, [this] {return !tasks.empty() || _stop;});
                    if (_stop && tasks.empty()) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::thread::id this_id = std::this_thread::get_id();
                cout << "Task Count: " << std::get<0>(task) << " at time: " << std::get<1>(task) << " processed by " << this_id << endl;
            }
        }

    public:
        ThreadPool(int threads_num) {
            for (int i = 0; i < threads_num; i++) {
                _threads.emplace_back([this, i] {
                    process_task(i);
                });
            }
        }

        ~ThreadPool() {
            mtx.lock();
            _stop = true;
            mtx.unlock();
            cv.notify_all();
            for (int i = 0; i < _threads.size(); i++) {
                _threads[i].join();
            }
        };

        void create_task() {
            time_t timestamp;
            time(&timestamp);
        
            mtx.lock();
            tasks.push(make_tuple(++count, timestamp));
            mtx.unlock();
            // cout << "Pushed task number: " << count << endl;
            cv.notify_one();
            return;
        }
};


int main() {
    ThreadPool pool(100);

    int tasks_num = 1000;
    for (int i = 0; i < tasks_num; i++) {
        pool.create_task();
    }

}
#include <iostream>
#include <tuple>
#include <bits/stdc++.h>
#include <ctime>
#include <queue>
#include <mutex>
// #include <thread>
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
        queue<tuple<int, time_t>> tasks; // TODO: why can't I make this atomic?
        atomic<int> count = 0; // TODO: does this need to be atomic if it's only being updated in mutex locked section?
        mutex mtx;
        condition_variable cv;

        void process_task() {
            cout << "here" << endl;
            while (true) {
                mtx.lock();
                std::unique_lock<mutex> lock(mtx);
                cv.wait(lock, [this] {return !tasks.empty();});
                if (tasks.empty()) {
                    return;
                }
                tuple<int, time_t> task = std::move(tasks.front());
                tasks.pop();
                mtx.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::thread::id this_id = std::this_thread::get_id();
                cout << "Task Count: " << std::get<0>(task) << " at time: " << std::get<1>(task) << " processed by " << this_id << endl;
            }
        }

    public:
        ThreadPool(int threads_num) {
            for (int i = 0; i < threads_num; i++) {
                std::thread t([this] {process_task();});
                // _threads.emplace_back(t);
            }
        }

        ~ThreadPool() {
            // for (int i = 0; i < _threads.size(); i++) {
            //     std::thread t = _threads[0];
            //     t.join();
            // }
        };

        void create_task() {
            time_t timestamp;
            time(&timestamp);
        
            mtx.lock();
            tasks.push(make_tuple(++count, timestamp));
            mtx.unlock();
            cout << "Pushed task number: " << count << endl;
            cv.notify_one();
            return;
        }
};





// cout << "thread id is " << t.get_id() << endl;
int main() {
    ThreadPool pool(5);

    int tasks_num = 5;
    for (int i = 0; i < tasks_num; i++) {
        pool.create_task();
    }

}
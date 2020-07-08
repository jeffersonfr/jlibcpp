/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jthread/jthreadpool.h"

namespace jthread {

ThreadPool::ThreadPool(size_t threads):
  stop(false)
{
  for(size_t i = 0;i<threads;++i)
    workers.emplace_back(
        [this]{
          for(;this->stop == false;) {
            std::function<void()> task;

          
            {
              std::unique_lock<std::mutex> lock(this->queue_mutex);

              this->condition.wait(lock,
                  [this]{
                    return this->stop || !this->tasks.empty(); 
                  });
              
              if(this->stop && this->tasks.empty()) {
                return;
              }

              task = std::move(this->tasks.front());
              
              this->tasks.pop();
            }

            task();
          }
        });
}

ThreadPool::~ThreadPool()
{
  stop = true;

  condition.notify_all();

  for(std::thread &worker: workers) {
    worker.join();
  }
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();

  {
    std::unique_lock<std::mutex> lock(queue_mutex);

    if(stop) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    tasks.emplace([task](){ (*task)(); });
  }

  condition.notify_one();

  return res;
}

}

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
#ifndef J_THREADPOOL_H
#define J_THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace jthread {

class ThreadPool {
  private:
    /* \brief */
    std::vector< std::thread > workers;
    /* \brief */
    std::queue< std::function<void()> > tasks;
    /* \brief */
    std::mutex queue_mutex;
    /* \brief */
    std::condition_variable condition;
    /* \brief */
    bool stop;

  public:
    /**
     * \brief
     *
     */
    ThreadPool(size_t);
    
    /**
     * \brief
     *
     */
    virtual ~ThreadPool();
    
    /**
     * \brief
     *
     */
    template<class F, class... Args> 
      auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

}

#endif

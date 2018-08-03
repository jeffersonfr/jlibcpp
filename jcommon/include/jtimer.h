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
#ifndef J_TIMER_H
#define J_TIMER_H

#include "jcommon/jobject.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <memory>
#include <algorithm>
#include <functional>

namespace jcommon {

/**
 * \brief Thread.
 *
 * @author Jeff Ferr
 */
class TimerTask : public virtual jcommon::Object {

  private:
    TimerTask()
    {
    }

  public:
    virtual ~TimerTask()
    {
    }

    template <class callable, class... arguments> TimerTask(int after, bool async, callable&& f, arguments&&... args) 
    {
      std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

      if (async) {
        std::thread([after, task]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }).detach();
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(after));

        task();
      }
    }

};

}

#endif

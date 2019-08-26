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
#ifndef J_THREADLIB_H
#define J_THREADLIB_H

#include <functional>
#include <utility>

#define CONCAT_NAMES(a, b) a ## b
#define UNIQUE_DEFER_VARIABLE(a, b) CONCAT_NAMES(a, b)

#define defer(f) ScopeGuard UNIQUE_DEFER_VARIABLE(__defer__, __LINE__) = [&]() { f; }

class ScopeGuard {

  private:
    std::function<void()> _function;

  public:
    template<typename Callable> ScopeGuard(Callable &&fn):
      _function(std::forward<Callable>(fn))
    {
    }

    ScopeGuard(ScopeGuard &&other):
      _function(std::move(other._function))
    {
    }

    virtual ~ScopeGuard()
    {
      if (_function) {
        _function();
      }
    }

    ScopeGuard(const ScopeGuard &) = delete;

    void operator=(const ScopeGuard &) = delete;
};

#endif

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
#ifndef J_CHANNEL_H
#define J_CHANNEL_H

#include "jcommon/jobject.h"

#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <optional>

namespace jthread {

/**
 * \brief Semaphore.
 * 
 * \author Jeff Ferr
 */
template<typename T> class Channel : public virtual jcommon::Object {
	
	private:
    /** \brief */
		std::queue<T> _list;
    /** \brief */
		std::mutex _mutex;
    /** \brief */
		std::condition_variable _send_condition;
    /** \brief */
		std::condition_variable _recv_condition;
    /** \brief */
		size_t _buffer_size;
    /** \brief */
		bool _is_closed;

	public:
    /**
     * \brief 
     *
     */
		Channel(size_t size = 20):
      jcommon::Object()
    {
      jcommon::Object::SetClassName("jthread::Channel");

      _buffer_size = size;
      _is_closed = false;
    }
    
    /**
     * \brief 
     *
     */
		virtual ~Channel()
    {
    }

    /**
     * \brief 
     *
     */
  	bool Send(T item)
    {
      std::unique_lock<std::mutex> l(_mutex);

      if (_is_closed == true) {
        return false;
      }

      _list.push(item);
      _recv_condition.notify_all();

      _send_condition.wait(l, 
        [&] { 
          return _list.size() <= _buffer_size or _is_closed; 
        });

      return true;
    }
    
    /**
     * \brief 
     *
     */
		std::optional<T> TryRecv()
    {
      std::unique_lock<std::mutex> l(_mutex);

      _recv_condition.wait(l, 
        [&] { 
          return _list.size() > 0 or _is_closed; 
        });

      if (_list.empty() == true) {
        return std::nullopt;
      }

      T entry = _list.front();

      _list.pop();
      _send_condition.notify_all();

      return std::make_optional(entry);
    }
    
    /**
     * \brief 
     *
     */
		void Close()
    {
      std::lock_guard<std::mutex> l(_mutex);

      _is_closed = true;
      _recv_condition.notify_all();
    }
    
    /**
     * \brief 
     *
     */
		bool IsClosed()
    {
      std::lock_guard<std::mutex> l(_mutex);

      return _is_closed;
    }
    
    /**
     * \brief 
     *
     */
		size_t Length()
    {
      std::lock_guard<std::mutex> l(_mutex);

      return _list.size();
    }

    /**
     * \brief 
     *
     */
    void operator<<(const T &param)
    {
      Send(param);
    }

    /**
     * \brief 
     *
     */
    void operator>>(T &param)
    {
      std::optional<T> value = TryRecv();

      if (value) {
        param = value.value();
      }
    }

		class my_iterator: public std::iterator<std::input_iterator_tag, T, T, T*, T&> {
			private:
				Channel<T>* _channels;

			public:
				explicit my_iterator(Channel<T> *channel):
					_channels(channel)
			{
			}

			my_iterator & operator++() 
			{
				return *this;
			}

			my_iterator operator++(int) 
			{
				return *this;
			}

			bool operator==(my_iterator other) const 
			{
				return _channels->IsClosed() and _channels->Length() == 0;
			}

			bool operator!=(my_iterator other) const 
			{
				return !_channels->IsClosed() or _channels->Length() > 0;
			}

			std::optional<T> operator*() const 
			{
				return _channels->TryRecv();
			}
		};

		my_iterator begin() 
		{
			return my_iterator(this);
		}

		my_iterator end() 
		{
			return my_iterator(this);
		}

};

}

#endif

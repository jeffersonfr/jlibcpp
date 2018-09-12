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
#ifndef J_EVENTMANAGER_H
#define J_EVENTMANAGER_H

#include "jevent/jeventobject.h"

#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace jgui {

class Window;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class EventManager : public jcommon::Object {

  friend class jgui::Window;

	private:
    /** \brief */
    std::map<std::string, int> _event_count;
    /** \brief */
    std::vector<jevent::EventObject *> _events;
    /** \brief */
    std::thread _thread;
    /** \brief */
    std::mutex _mutex;
    /** \brief */
    std::condition_variable _condition;
    /** \brief */
    jgui::Window *_window;
    /** \brief */
    size_t _click_delay;
    /** \brief */
    bool _alive;

	protected:
		/**
		 * \brief
		 *
		 */
		EventManager(jgui::Window *window);
		
		/**
		 * \brief
		 *
		 */
    virtual void ProcessEvents();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~EventManager();

		/**
		 * \brief Limits the number of events from a type in the queue.
		 *
		 */
		virtual void SetEventCountLimit(jevent::EventObject &event, int count);
		
		/**
		 * \brief Limits the number of events from a type in the queue.
		 *
		 */
		virtual int GetEventCountLimit(jevent::EventObject &event);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetClickDelay(size_t ms);
		
		/**
		 * \brief
		 *
		 */
		virtual size_t GetClickDelay();
		
		/**
		 * \brief
		 *
		 */
		virtual void PostEvent(jevent::EventObject *event);

		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::EventObject *> & GetEvents();

};

}

#endif 

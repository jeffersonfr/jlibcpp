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
#include "jgui/jeventmanager.h"
#include "jgui/jwindow.h"

namespace jgui {

EventManager::EventManager(jgui::Window *window):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::EventManager");

  _click_delay = 200; // milliseconds
  _alive = true;
  _window = window;

  _thread = std::thread(&EventManager::ProcessEvents, this);
}

EventManager::~EventManager()
{
  std::unique_lock<std::mutex> lock(_mutex);

  _alive = false;

  _condition.notify_one();

  lock.unlock();

  _thread.join();
}

void EventManager::SetEventCountLimit(jevent::EventObject &event, int count)
{
  _event_count[event.GetClassName()] = count;
}

int EventManager::GetEventCountLimit(jevent::EventObject &event)
{
  return _event_count[event.GetClassName()];
}

void EventManager::SetClickDelay(size_t ms)
{
  _click_delay = ms;
}

size_t EventManager::GetClickDelay()
{
  return _click_delay;
}

void EventManager::PostEvent(jevent::EventObject *event)
{
  std::unique_lock<std::mutex> lock(_mutex);

  _events.push_back(event);

  _condition.notify_one();
}

bool EventManager::IsKeyDown(jevent::jkeyevent_symbol_t key)
{
  return _key_button_map[key];
}

bool EventManager::IsButtonDown(jevent::jmouseevent_button_t button)
{
  return _mouse_button_map[button];
}

const std::vector<jevent::EventObject *> & EventManager::GetEvents()
{
  return _events;
}

void EventManager::ProcessEvents()
{
  do {
    std::unique_lock<std::mutex> lock(_mutex);

    while (_events.size() == 0 && _alive == true) {
      _condition.wait(lock);
    }

    lock.unlock();

    if (_alive == false) {
      break;
    }

    jevent::EventObject *unknown = nullptr;

    _mutex.lock();

    unknown = _events.front();

    _events.erase(_events.begin());

    _mutex.unlock();
  
    if (dynamic_cast<jevent::KeyEvent *>(unknown) != nullptr) {
      jevent::KeyListener *listener = dynamic_cast<jevent::KeyListener *>(_window);

      if (listener != nullptr) {
        jevent::KeyEvent *event = dynamic_cast<jevent::KeyEvent *>(unknown);

        if (event->GetType() == jevent::JKT_PRESSED) {
          _key_button_map[event->GetSymbol()] = true;

          listener->KeyPressed(event);
        } else if (event->GetType() == jevent::JKT_RELEASED) {
          _key_button_map[event->GetSymbol()] = false;

          listener->KeyReleased(event);
        } else if (event->GetType() == jevent::JKT_TYPED) {
          listener->KeyTyped(event);
        }
      }
    } else if (dynamic_cast<jevent::MouseEvent *>(unknown) != nullptr) {
      jevent::MouseListener *listener = dynamic_cast<jevent::MouseListener *>(_window);

      if (listener != nullptr) {
        jevent::MouseEvent *event = dynamic_cast<jevent::MouseEvent *>(unknown);

        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        int clicks = 1;

        if (event->GetType() == jevent::JMT_PRESSED) {
          static std::map<jevent::jmouseevent_button_t, std::chrono::time_point<std::chrono::steady_clock>> button_timestamp;

          _mouse_button_map[event->GetButton()] = true;

          if ((event->GetTimestamp() - button_timestamp[event->GetButton()]) < std::chrono::milliseconds(200)) {
            clicks = clicks + 1;
          }

          button_timestamp[event->GetButton()] = event->GetTimestamp();
        } else if (event->GetType() == jevent::JMT_RELEASED) {
          _mouse_button_map[event->GetButton()] = false;
        }
        
        if (_mouse_button_map[jevent::JMB_BUTTON1] == true) {
          buttons = (jevent::jmouseevent_button_t)(buttons | jevent::JMB_BUTTON1);
        }

        if (_mouse_button_map[jevent::JMB_BUTTON2] == true) {
          buttons = (jevent::jmouseevent_button_t)(buttons | jevent::JMB_BUTTON2);
        }

        if (_mouse_button_map[jevent::JMB_BUTTON3] == true) {
          buttons = (jevent::jmouseevent_button_t)(buttons | jevent::JMB_BUTTON3);
        }

        jevent::MouseEvent local(event->GetSource(), event->GetType(), event->GetButton(), buttons, event->GetLocation(), clicks);

        if (event->GetType() == jevent::JMT_PRESSED) {
          listener->MousePressed(&local);
        } else if (event->GetType() == jevent::JMT_RELEASED) {
          listener->MouseReleased(&local);
        } else if (event->GetType() == jevent::JMT_MOVED) {
          listener->MouseMoved(&local);
        } else if (event->GetType() == jevent::JMT_ROTATED) {
          listener->MouseWheel(&local);
        }
      }
    }
  
    delete unknown;
  } while (_alive == true);
}

}


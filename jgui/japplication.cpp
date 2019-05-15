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
#include "jgui/japplication.h"
#include "jexception/jnullpointerexception.h"

#ifdef JGUI_UI
#include "binds/include/nativeapplication.h"
#endif

#include <mutex>

namespace jgui {

static Application *_instance = nullptr;
static std::mutex g_application_mutex;
static std::mutex g_application_event_mutex;

Application::Application():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Application");
}

Application::~Application()
{
  delete _instance;
  _instance = nullptr;
}

void Application::InternalInit(int argc, char **argv)
{
}

void Application::InternalLoop()
{
}

void Application::InternalQuit()
{
}

void Application::Init(int argc, char **argv)
{
  std::lock_guard<std::mutex> guard(g_application_mutex);

  if (_instance == nullptr) {
    try {
#ifdef JGUI_UI
      _instance = new NativeApplication();
#endif

      if (_instance != nullptr) {
        _instance->InternalInit(argc, argv);
      }
    } catch (jexception::NullPointerException &e) {
      throw jexception::RuntimeException("Application cannot be initialized: " + e.What());
    }
  }
        
  if (_instance == nullptr) {
    throw jexception::NullPointerException("No graphic engine avaiable !");
  }
}

void Application::Loop()
{
  g_application_mutex.lock();

  if (_instance == nullptr) {
    throw jexception::RuntimeException("Application was not initialized");
  }

  g_application_mutex.unlock();

  _instance->InternalLoop();
}

void Application::Quit()
{
  g_application_mutex.lock();

  if (_instance == nullptr) {
    throw jexception::RuntimeException("Application was not initialized");
  }

  g_application_mutex.unlock();

  _instance->InternalQuit();
}

jsize_t<int> Application::GetScreenSize()
{
  g_application_mutex.lock();

  if (_instance == nullptr) {
    throw jexception::RuntimeException("Application was not initialized");
  }

  g_application_mutex.unlock();

  return _instance->GetScreenSize();
}

void Application::SetVerticalSyncEnabled(bool enabled)
{
  g_application_mutex.lock();

  if (_instance == nullptr) {
    throw jexception::RuntimeException("Application was not initialized");
  }

  g_application_mutex.unlock();

  _instance->SetVerticalSyncEnabled(enabled);
}

bool Application::IsVerticalSyncEnabled()
{
  g_application_mutex.lock();

  if (_instance == nullptr) {
    throw jexception::RuntimeException("Application was not initialized");
  }

  g_application_mutex.unlock();

  return _instance->IsVerticalSyncEnabled();
}

}

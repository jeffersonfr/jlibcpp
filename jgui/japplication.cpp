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

#if defined(SDL2_UI)
#include "sdl2/include/sdl2application.h"
#elif defined(SFML2_UI)
#include "sfml2/include/sfml2application.h"
#elif defined(X11_UI)
#include "x11/include/x11application.h"
#elif defined(GTK3_UI)
#include "gtk3/include/gtk3application.h"
#elif defined(ALLEGRO5_UI)
#include "allegro5/include/allegro5application.h"
#endif

#include <mutex>

namespace jgui {

static Application *_instance = NULL;
static std::vector<jevent::EventObject *> _events;
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
  _instance = NULL;
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

	if (_instance == NULL) {
	  try {
#if defined(SDL2_UI)
			_instance = new SDL2Application();
#elif defined(SFML2_UI)
			_instance = new SFML2Application();
#elif defined(X11_UI)
			_instance = new X11Application();
#elif defined(GTK3_UI)
			_instance = new GTK3Application();
#elif defined(ALLEGRO5_UI)
			_instance = new Allegro5Application();
#endif

      if (_instance != NULL) {
        _instance->InternalInit(argc, argv);
      }
		} catch (jexception::NullPointerException &e) {
    	throw jexception::RuntimeException("Application cannot be initialized: " + e.what());
		}
	}
        
  if (_instance == NULL) {
    throw jexception::NullPointerException("No graphic engine avaiable !");
  }
}

void Application::Loop()
{
  g_application_mutex.lock();

	if (_instance == NULL) {
    throw jexception::RuntimeException("Application was not initialized");
	}

  g_application_mutex.unlock();

	_instance->InternalLoop();
}

void Application::Quit()
{
  g_application_mutex.lock();

	if (_instance == NULL) {
    throw jexception::RuntimeException("Application was not initialized");
	}

  g_application_mutex.unlock();

	_instance->InternalQuit();
}

void Application::PushEvent(jevent::EventObject *event)
{
  if ((void *)event == NULL) {
    return;
  }

  std::lock_guard<std::mutex> guard(g_application_event_mutex);

  _events.push_back(event);
}

std::vector<jevent::EventObject *> & Application::GrabEvents()
{
  // g_application_event_mutex.lock();

  return _events;
}

void Application::ReleaseEvents()
{
  // g_application_event_mutex.unlock();
}

jsize_t Application::GetScreenSize()
{
  g_application_mutex.lock();

	if (_instance == NULL) {
    throw jexception::RuntimeException("Application was not initialized");
	}

  g_application_mutex.unlock();

	return _instance->GetScreenSize();
}

void Application::SetVerticalSyncEnabled(bool enabled)
{
  g_application_mutex.lock();

	if (_instance == NULL) {
    throw jexception::RuntimeException("Application was not initialized");
	}

  g_application_mutex.unlock();

	_instance->SetVerticalSyncEnabled(enabled);
}

bool Application::IsVerticalSyncEnabled()
{
  g_application_mutex.lock();

	if (_instance == NULL) {
    throw jexception::RuntimeException("Application was not initialized");
	}

  g_application_mutex.unlock();

	return _instance->IsVerticalSyncEnabled();
}

}

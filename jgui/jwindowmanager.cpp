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
#include "jwindowmanager.h"
#include "jwindow.h"
#include "jautolock.h"
#include "jgraphics.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

WindowManager * WindowManager::_instance = NULL;

WindowManager::WindowManager():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::WindowManager");
}

WindowManager::~WindowManager()
{
}

WindowManager * WindowManager::GetInstance()
{
	if (_instance == NULL) {
		_instance = new WindowManager();
	}

	return _instance;
}

std::vector<Window *> & WindowManager::GetWindows()
{
	return windows;
}

Window * WindowManager::GetWindowInFocus()
{
	if (windows.size() > 0) {
		return *windows.begin();
	}

	return NULL;
}

void WindowManager::Add(Window *w)
{
	windows.insert(windows.begin(), w);
}

void WindowManager::Remove(Window *w)
{
	for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); ++i) {
		if ((*i) == w) {
			windows.erase(i);

			break;
		}
	}
}

void WindowManager::Restore()
{
#ifdef DIRECTFB_UI
	for (std::vector<Window *>::const_iterator i=windows.begin(); i!=windows.end(); ++i) {
		Window *w = (*i);

		((Window *)w)->Show(false);
	} 
#endif
}

void WindowManager::Release()
{
#ifdef DIRECTFB_UI
	for (std::vector<Window *>::const_iterator i=windows.begin(); i!=windows.end(); ++i) {
		Window *w = (*i);

		w->ReleaseWindow();
	}
#endif
}

void WindowManager::RaiseToTop(Window *c)
{
	if (c == NULL) {
		return;
	}

	bool b = false;

	for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); ++i) {
		if (c == (*i)) {
			windows.erase(i);

			b = true;

			break;
		}
	}
	
	if (b == true) {
		c->RaiseToTop();
		windows.insert(windows.begin(), c);
	}
}

void WindowManager::LowerToBottom(Window *c)
{
	if (c == NULL) {
		return;
	}

	bool b = false;

	for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); ++i) {
		if (c == (*i)) {
			windows.erase(i);

			b = true;

			break;
		}
	}
	
	if (b == true) {
		windows.push_back(c);
	}
}

void WindowManager::PutWindowATop(Window *c, Window *c1)
{
	if (c == NULL || c1 == NULL) {
		return;
	}

	std::vector<Window *>::iterator i;

	i = std::find(windows.begin(), windows.end(), c1);

	if (i == windows.end()) {
		return;
	}

	windows.insert(i, c);
}

void WindowManager::PutWindowBelow(Window *c, Window *c1)
{
	if (c == NULL || c1 == NULL) {
		return;
	}

	std::vector<Window *>::iterator i;

	i = std::find(windows.begin(), windows.end(), c1);

	if (i == windows.end()) {
		return;
	}

	windows.insert(i+1, c);
}

}


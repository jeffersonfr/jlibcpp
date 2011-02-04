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
#include "Stdafx.h"
#include "jthememanager.h"

namespace jgui {

ThemeManager *ThemeManager::_instance = NULL;

ThemeManager::ThemeManager():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::ThemeManager");

	_default_theme = new Theme();

	_theme = _default_theme;
}

ThemeManager::~ThemeManager()
{
	if (_default_theme != NULL) {
		delete _default_theme;
	}

	if (_instance != NULL) {
		delete _instance;
	}
}

ThemeManager * ThemeManager::GetInstance()
{
	if (_instance == NULL) {
		_instance = new ThemeManager();
	}

	return _instance;
}

Theme * ThemeManager::GetDefaultTheme()
{
	return _default_theme;
}

void ThemeManager::SetTheme(Theme *theme)
{
	if (_theme == theme) {
		return;
	}

	_theme = theme;

	DispatchThemeEvent(new ThemeEvent(this, _theme));
}

Theme * ThemeManager::GetTheme()
{
	if (_theme != NULL) {
		return _theme;
	}

	return _default_theme;
}

void ThemeManager::RegisterThemeListener(ThemeListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_theme_listeners.begin(), _theme_listeners.end(), listener) == _theme_listeners.end()) {
		_theme_listeners.push_back(listener);

		if (_theme != NULL) {
			ThemeEvent *event = new ThemeEvent(this, _theme);

			listener->ThemeChanged(event);

			delete event;
		}
	}
}

void ThemeManager::RemoveThemeListener(ThemeListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ThemeListener *>::iterator i = std::find(_theme_listeners.begin(), _theme_listeners.end(), listener);
	
	if (i != _theme_listeners.end()) {
		_theme_listeners.erase(i);
	}
}

void ThemeManager::DispatchThemeEvent(ThemeEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_theme_listeners.size();

	while (k++ < (int)_theme_listeners.size()) {
		ThemeListener *listener = _theme_listeners[k-1];

		listener->ThemeChanged(event);

		if (size != (int)_theme_listeners.size()) {
			size = (int)_theme_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<ThemeListener *>::iterator i=_theme_listeners.begin(); i!=_theme_listeners.end(); i++) {
		listener->ChangedTheme(event);
	}
	*/

	delete event;
}

std::vector<ThemeListener *> & ThemeManager::GetThemeListeners()
{
	return _theme_listeners;
}

}


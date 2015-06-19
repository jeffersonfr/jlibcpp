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
#include "jdialogbox.h"

namespace jgui {

DialogBox::DialogBox(std::string title, int x, int y, int width, int height):
 	jgui::Frame(title, x, y, width, height)
{
	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	int w = width;
	int h = height;

	if (width < 0) {
		w = (int)(screen.width*0.6);
	}
	
	if (h < 0) {
		h = (int)(screen.height*0.4);
	}
	
	if (x < 0) {
		x = (screen.width-w)/2;
	}
	
	if (y < 0) {
		y = (int)(screen.height*0.2);
	}

	SetBounds(x, y, w, h);
}

DialogBox::~DialogBox() 
{
}

jcommon::ParamMapper * DialogBox::GetParams()
{
	return &_params;
}

void DialogBox::RegisterDataListener(jcommon::DataListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_data_listeners.begin(), _data_listeners.end(), listener) == _data_listeners.end()) {
		_data_listeners.push_back(listener);
	}
}

void DialogBox::RemoveDataListener(jcommon::DataListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<jcommon::DataListener *>::iterator i = std::find(_data_listeners.begin(), _data_listeners.end(), listener);

	if (i != _data_listeners.end()) {
		_data_listeners.erase(i);
	}
}

void DialogBox::DispatchDataEvent(jcommon::ParamMapper *params)
{
	if (params == NULL) {
		return;
	}

	int k = 0,
			size = (int)_data_listeners.size();

	while (k++ < (int)_data_listeners.size()) {
		jcommon::DataListener *listener = _data_listeners[k-1];

		listener->DataChanged(params);

		if (size != (int)_data_listeners.size()) {
			size = (int)_data_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<DataListener *>::iterator i=_data_listeners.begin(); i!=_data_listeners.end(); i++) {
		if (event->GetType() == JWET_CLOSING) {
			(*i)->DataClosing(event);
		} else if (event->GetType() == JWET_CLOSED) {
			(*i)->DataClosed(event);
		} else if (event->GetType() == JWET_OPENED) {
			(*i)->DataOpened(event);
		} else if (event->GetType() == JWET_RESIZED) {
			(*i)->DataResized(event);
		} else if (event->GetType() == JWET_MOVED) {
			(*i)->DataMoved(event);
		}
	}
	*/
}

std::vector<jcommon::DataListener *> & DialogBox::GetDataListeners()
{
	return _data_listeners;
}

}

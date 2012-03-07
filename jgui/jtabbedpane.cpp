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
#include "jtabbedpane.h"

namespace jgui {

TabbedPane::TabbedPane(int x, int y, int width, int height):
   	jgui::Container(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::TabbedPane");

	SetFocusable(true);
}

TabbedPane::~TabbedPane()
{
}

int TabbedPane::GetCurrentTab()
{
	return 0;
}

void TabbedPane::AddTab(std::string title, jgui::Image *image, jgui::Component *component, int index)
{
}

void TabbedPane::RemoveTab(int index)
{
}

jgui::Component * TabbedPane::GetTabComponentAt(int index)
{
	return NULL;
}

void TabbedPane::SetTabTitle(int index, std::string title)
{
}

std::string TabbedPane::GetTabTitle(int index)
{
	return "";
}

int TabbedPane::GetTabCount()
{
	return 0;
}

void TabbedPane::SetPaddind(int left, int top, int right, int bottom)
{
}

int TabbedPane::IndexOfComponent(jgui::Component *cmp)
{
	return 0;
}

void TabbedPane::RegisterTabsListener(SelectListener *listener)
{
}

void TabbedPane::RemoveTabsListener(SelectListener *listener)
{
}

void TabbedPane::DispatchTabsEvent(SelectEvent *event)
{
}

std::vector<SelectListener *> & TabbedPane::GetTabsListeners()
{
	return _select_listeners;
}


}

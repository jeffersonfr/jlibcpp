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
#include "jguilib.h"

namespace jgui {

Animation::Animation(int x, int y, int width, int height):
 	Component(x, y, width, height),
	jthread::Thread()
{
	SetClassName("jgui::Animation");

	_index = 0;
	_running = true;

	SetInterval(500);
}

Animation::~Animation()
{
	SetVisible(false);
	
	WaitThread();

	while (_images.size() > 0) {
		OffScreenImage *prefetch = (*_images.begin());

		_images.erase(_images.begin());

		delete prefetch;
	}
}

void Animation::Release()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_running = false;
	}
}

void Animation::SetVisible(bool b)
{
	jthread::AutoLock lock(&_component_mutex);

	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		if (IsRunning() == false) {
			_running = true;

			Start();
		}
	} else {
		_running = false;

		Release();
		WaitThread();
		Repaint(true);
	}
}

void Animation::RemoveAll()
{
	jthread::AutoLock lock(&_component_mutex);

	_images.clear();
}

void Animation::SetInterval(int i)
{
	jthread::AutoLock lock(&_component_mutex);

	_interval = i;
}

void Animation::AddImage(std::string file)
{
	jthread::AutoLock lock(&_component_mutex);

	OffScreenImage *prefetch = new OffScreenImage(_size.width, _size.height);

	prefetch->GetGraphics()->DrawImage(file, 0, 0, _size.width, _size.height);

	_images.push_back(prefetch);
}

void Animation::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y,
			gapx = 0,
			gapy = 0;
	int px = x+gapx,
			py = y+gapy,
			pw = w-gapx,
			ph = h-gapy;

	if (_images.size() != 0) {
		OffScreenImage *image = _images[_index];

		g->DrawImage(image, px, py, pw, ph);
	}
}

void Animation::Run()
{
	while (_running == true) {
		Repaint();

		if (_running == false) {
			return;
		}

		_index++;

		if (_index >= (int)_images.size()) {
			_index = 0;
		}

		jthread::Thread::MSleep(_interval);
	}
}

}

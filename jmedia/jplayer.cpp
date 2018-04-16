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
#include "jplayer.h"

#include <algorithm>

#include <stdio.h>

namespace jmedia {

Player::Player():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmedia::Player");

	_media_info.title = "";
	_media_info.author = "";
	_media_info.album = "";
	_media_info.genre = "";
	_media_info.comments = "";
	_media_info.date = "";
}

Player::~Player()
{
	while (_controls.size() > 0) {
		Control *control = (*_controls.begin());

		_controls.erase(_controls.begin());

		delete control;
	}
}

jmedia_info_t Player::GetMediaInfo()
{
	return _media_info;
}

void Player::Play()
{
}

void Player::Pause()
{
}

void Player::Stop()
{
}

void Player::Resume()
{
}

void Player::Close()
{
}

void Player::SetCurrentTime(uint64_t i)
{
}

uint64_t Player::GetCurrentTime()
{
	return -1LL;
}

uint64_t Player::GetMediaTime()
{
	return -1LL;
}

void Player::SetLoop(bool b)
{
}

bool Player::IsLoop()
{
	return false;
}

double Player::GetDecodeRate()
{
	return 0.0;
}

void Player::SetDecodeRate(double rate)
{
}

std::vector<Control *> & Player::GetControls()
{
	return _controls;
}

jgui::Component * Player::GetVisualComponent()
{
	return NULL;
}

Control * Player::GetControl(std::string id)
{
	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		if (control->GetID() == id) {
			return control;
		}
	}

	return NULL;
}

void Player::RegisterPlayerListener(PlayerListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_player_listeners.begin(), _player_listeners.end(), listener) == _player_listeners.end()) {
		_player_listeners.push_back(listener);
	}
}

void Player::RemovePlayerListener(PlayerListener *listener)
{
	if (listener == NULL) {
		return;
	}

  _player_listeners.erase(std::remove(_player_listeners.begin(), _player_listeners.end(), listener), _player_listeners.end());
}

std::vector<PlayerListener *> & Player::GetPlayerListeners()
{
	return _player_listeners;
}

void Player::DispatchPlayerEvent(PlayerEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_player_listeners.size();

	while (k++ < (int)_player_listeners.size()) {
		PlayerListener *listener = _player_listeners[k-1];

		if (event->GetType() == JPE_STARTED) {
			listener->MediaStarted(event);
		} else if (event->GetType() == JPE_PAUSED) {
			listener->MediaPaused(event);
		} else if (event->GetType() == JPE_RESUMED) {
			listener->MediaResumed(event);
		} else if (event->GetType() == JPE_STOPPED) {
			listener->MediaStopped(event);
		} else if (event->GetType() == JPE_FINISHED) {
			listener->MediaFinished(event);
		}

		if (size != (int)_player_listeners.size()) {
			size = (int)_player_listeners.size();

			k--;
		}
	}

	delete event;
}

void Player::RegisterFrameGrabberListener(FrameGrabberListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_frame_listeners.begin(), _frame_listeners.end(), listener) == _frame_listeners.end()) {
		_frame_listeners.push_back(listener);
	}
}

void Player::RemoveFrameGrabberListener(FrameGrabberListener *listener)
{
	if (listener == NULL) {
		return;
	}

  _frame_listeners.erase(std::remove(_frame_listeners.begin(), _frame_listeners.end(), listener), _frame_listeners.end());
}

std::vector<FrameGrabberListener *> & Player::GetFrameGrabberListeners()
{
	return _frame_listeners;
}

void Player::DispatchFrameGrabberEvent(FrameGrabberEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_frame_listeners.size();

	while (k++ < (int)_frame_listeners.size()) {
		FrameGrabberListener *listener = _frame_listeners[k-1];

		if (event->GetType() == JFE_GRABBED) {
			listener->FrameGrabbed(event);
		}

		if (size != (int)_frame_listeners.size()) {
			size = (int)_frame_listeners.size();

			k--;
		}
	}

	delete event;
}


}

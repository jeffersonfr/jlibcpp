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
#include "jframe.h"
#include "jplayermanager.h"
#include "jplayerlistener.h"
#include "jframelistener.h"
#include "jautolock.h"

#include <stdio.h>
#include <unistd.h>

#define BOX_NUMS	4
#define BOX_STEP	32
#define BOX_SIZE	240

class MediaStart : public jthread::Thread {

	public:
		jmedia::Player *_player;
		int _dx;
		int _dy;

	public:
		MediaStart(jmedia::Player *player, int dx, int dy)
		{
			_player = player;
			_dx = dx;
			_dy = dy;
		}

		virtual ~MediaStart()
		{
			delete _player;
		}

		virtual void Release()
		{
			_player->Stop();
				
			WaitThread();
		}

		virtual void Run()
		{
			_player->Play();
		}

};

class PlayerTest : public jgui::Frame, public jmedia::PlayerListener, public jmedia::FrameListener {

	private:
		std::vector<MediaStart *> _players;
		jthread::Mutex _mutex;
		bool _is_playing;

	public:
		PlayerTest(std::string file):
			jgui::Frame("Player Test", 0, 0, 1920, 1080)
		{
			jgui::jsize_t size = GetSize();

			for (int i=0; i<BOX_NUMS; i++) {
				jmedia::Player *player = jmedia::PlayerManager::CreatePlayer(file);

				player->RegisterPlayerListener(this);
				player->RegisterFrameListener(this);

				jgui::Component *cmp = player->GetVisualComponent();

				cmp->SetBounds(random()%(size.width-BOX_SIZE), random()%(size.height-BOX_SIZE), BOX_SIZE, BOX_SIZE);

				Add(cmp);

				_players.push_back(new MediaStart(player, ((random()%2) == 0)?-1:1, ((random()%2) == 0)?-1:1));
			}

			_is_playing = false;
		}

		virtual ~PlayerTest()
		{
			jthread::AutoLock lock(&_mutex);

			for (int i=0; i<(int)_players.size(); i++) {
				MediaStart *media = _players[i];

				delete media;
			}
		}

		virtual void StartMedia()
		{
			jthread::AutoLock lock(&_mutex);

			for (std::vector<MediaStart *>::iterator i=_players.begin(); i!=_players.end(); i++) {
				(*i)->Start();
			}
			
			_is_playing = true;
		}

		virtual void StopMedia()
		{
			jthread::AutoLock lock(&_mutex);

			for (std::vector<MediaStart *>::iterator i=_players.begin(); i!=_players.end(); i++) {
				(*i)->Release();
			}

			RemoveAll();

			_is_playing = false;
		}

		virtual void Run()
		{
			jthread::AutoLock lock(&_mutex);

			if (_is_playing == false) {
				return;
			}

			jgui::jsize_t size = GetSize();

			for (std::vector<MediaStart *>::iterator i=_players.begin(); i!=_players.end(); i++) {
				MediaStart *media = (*i);
	
				jgui::Component *cmp = media->_player->GetVisualComponent();
				jgui::jpoint_t location = cmp->GetLocation();

				location.x = location.x+media->_dx*BOX_STEP;
				location.y = location.y+media->_dy*BOX_STEP;

				if (location.x < 0) {
					location.x = 0;
					media->_dx = 1;
				}

				if ((location.x+BOX_SIZE) > size.width) {
					location.x = size.width-BOX_SIZE;
					media->_dx = -1;
				}

				if (location.y < 0) {
					location.y = 0;
					media->_dy = 1;
				}

				if ((location.y+BOX_SIZE) > size.height) {
					location.y = size.height-BOX_SIZE;
					media->_dy = -1;
				}

				cmp->SetLocation(location);
			}
		}

		virtual void MediaStarted(jmedia::PlayerEvent *event)
		{
			std::cout << "Media Started" << std::endl;
		}

		virtual void MediaResumed(jmedia::PlayerEvent *event)
		{
			std::cout << "Media Resumed" << std::endl;
		}

		virtual void MediaPaused(jmedia::PlayerEvent *event)
		{
			std::cout << "Media Paused" << std::endl;
		}

		virtual void MediaStopped(jmedia::PlayerEvent *event)
		{
			std::cout << "Media Stopped" << std::endl;
		}

		virtual void MediaFinished(jmedia::PlayerEvent *event)
		{
			std::cout << "Media Finished" << std::endl;
		}

		virtual void FrameGrabbed(jmedia::FrameEvent *event)
		{
			jgui::Image *image = event->GetFrame();
			jgui::Graphics *g = image->GetGraphics();

			g->SetColor(jgui::Color::Blue);
			g->FillRectangle(4, 4, 12, 12);
		}

};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "use:: " << argv[0] << " <media>" << std::endl;

		return -1;
	}

	srand(time(NULL));

	PlayerTest test(argv[1]);

	test.Show();

	test.StartMedia();

	int k = 100;

	while (k-- > 0) {
		test.Run();

		usleep(100000);
	}

	test.StopMedia();

	sleep(2);

	return 0;
}


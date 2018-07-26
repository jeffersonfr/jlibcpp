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
#include "jgui/jwindow.h"
#include "jmedia/jplayermanager.h"
#include "jmedia/jplayerlistener.h"
#include "jmedia/jframegrabberlistener.h"

#include <thread> 

#include <stdio.h>
#include <unistd.h>

#define BOX_NUMS	4
#define BOX_STEP	32
#define BOX_SIZE	240

class MediaStart {

	public:
		jmedia::Player *_player;
    std::thread _thread;
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

    virtual void Start()
    {
      _thread = std::thread(&MediaStart::Run, this);
    }

		virtual void Stop()
		{
			_player->Stop();

      _thread.join();
		}

		virtual void Run()
		{
			_player->Play();
		}

};

class PlayerTest : public jgui::Widget, public jmedia::PlayerListener, public jmedia::FrameGrabberListener {

	private:
		std::vector<MediaStart *> _players;
    std::mutex _mutex;
		bool _is_playing;

	public:
		PlayerTest(std::string file):
			jgui::Widget("Player Test")
		{
			jgui::jsize_t size = GetSize();

			for (int i=0; i<BOX_NUMS; i++) {
				jmedia::Player *player = jmedia::PlayerManager::CreatePlayer(file);

				player->RegisterPlayerListener(this);
				player->RegisterFrameGrabberListener(this);

				jgui::Component *cmp = player->GetVisualComponent();

				cmp->SetBounds(random()%(size.width-BOX_SIZE), random()%(size.height-BOX_SIZE), BOX_SIZE, BOX_SIZE);

				Add(cmp);

				_players.push_back(new MediaStart(player, ((random()%2) == 0)?-1:1, ((random()%2) == 0)?-1:1));
			}

			_is_playing = false;
		}

		virtual ~PlayerTest()
		{
      _mutex.lock();

			for (int i=0; i<(int)_players.size(); i++) {
				MediaStart *media = _players[i];

				delete media;
			}
      
      _mutex.unlock();
		}

		virtual void StartMedia()
		{
      _mutex.lock();

			for (std::vector<MediaStart *>::iterator i=_players.begin(); i!=_players.end(); i++) {
				(*i)->Start();
			}
			
			_is_playing = true;
      
      _mutex.unlock();
		}

		virtual void StopMedia()
		{
      _mutex.lock();

			for (std::vector<MediaStart *>::iterator i=_players.begin(); i!=_players.end(); i++) {
				(*i)->Stop();
			}

			RemoveAll();

			_is_playing = false;
      
      _mutex.unlock();
		}

		virtual void Render()
		{
      _mutex.lock();

			if (_is_playing == false) {
        _mutex.unlock();

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
        
      _mutex.unlock();
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

		virtual void FrameGrabbed(jmedia::FrameGrabberEvent *event)
		{
			jgui::Image *image = event->GetFrame();
			jgui::Graphics *g = image->GetGraphics();

			g->SetColor(jgui::Color::Blue);
			g->FillRectangle(4, 4, 12, 12);
		}

};

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: " << argv[0] << " <media>" << std::endl;

		return -1;
	}

	jgui::Application::Init(argc, argv);

	srand(time(NULL));

	PlayerTest app(argv[1]);

	app.SetTitle("Video Player");
	// main->SetSize(app.GetWidth(), app.GetHeight());
	app.SetVisible(true);
	app.StartMedia();
	
	int k = 100;

	while (k-- > 0) {
		app.Render();

		usleep(100000);
	}

	app.StopMedia();

	jgui::Application::Loop();

	return 0;
}


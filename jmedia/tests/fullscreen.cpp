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
#include "jmedia/jplayermanager.h"
#include "jmedia/jvideosizecontrol.h"
#include "jgui/jwindow.h"
#include "jevent/jplayerlistener.h"

#include <iostream>

class PlayerTest : public jgui::Window, public jevent::PlayerListener, public jevent::FrameGrabberListener {

	private:
		jmedia::Player *_player;
		jgui::GridLayout *_fullscreen_layout;

	public:
		PlayerTest(std::string file):
			jgui::Window(720, 480)
		{
			_player = jmedia::PlayerManager::CreatePlayer(file);

			jgui::Component *cmp = _player->GetVisualComponent();

      if (cmp != nullptr) {
  			cmp->SetSize(720, 480);
	  		cmp->SetVisible(true);

		  	Add(cmp);
      }

			_fullscreen_layout = new jgui::GridLayout(1, 1);

			SetLayout(_fullscreen_layout);

			_player->RegisterPlayerListener(this);
			_player->RegisterFrameGrabberListener(this);
		}

		virtual ~PlayerTest()
		{
			_player->Stop();

			delete _player;
			_player = nullptr;

			delete _fullscreen_layout;
			_fullscreen_layout = nullptr;
		}

		virtual void StartMedia()
		{
			_player->Play();
		}

		virtual void StopMedia()
		{
			_player->Stop();
		}

		virtual void FrameGrabbed(jevent::FrameGrabberEvent *event)
		{
			jgui::Image *image = (jgui::Image *)event->GetSource();
			jgui::Graphics *g = image->GetGraphics();

			g->SetColor(jgui::Color::Blue);
			g->FillRectangle(8, 8, 64, 64);
		}

		virtual void MediaStarted(jevent::PlayerEvent *event)
		{
			std::cout << "Media Started" << std::endl;
		}

		virtual void MediaResumed(jevent::PlayerEvent *event)
		{
			std::cout << "Media Resumed" << std::endl;
		}

		virtual void MediaPaused(jevent::PlayerEvent *event)
		{
			std::cout << "Media Paused" << std::endl;
		}

		virtual void MediaStopped(jevent::PlayerEvent *event)
		{
			std::cout << "Media Stopped" << std::endl;
		}

		virtual void MediaFinished(jevent::PlayerEvent *event)
		{
			std::cout << "Media Finished" << std::endl;
		}

};

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: " << argv[0] << " <media>" << std::endl;

		return -1;
	}

	jgui::Application::Init(argc, argv);

	srand(time(nullptr));

	PlayerTest app(argv[1]);

	app.SetTitle("Video Player");
	app.SetSize(720, 480);
	app.SetVisible(true);
	
  app.StartMedia();
	
	jgui::Application::Loop();

	app.StopMedia();

	return 0;
}


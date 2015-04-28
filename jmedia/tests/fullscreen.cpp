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

class PlayerTest : public jgui::Window, public jmedia::PlayerListener {

	private:
		jmedia::Player *_player;

	public:
		PlayerTest(std::string file):
			jgui::Window(0, 0, 1920, 1080)
		{
			_player = jmedia::PlayerManager::CreatePlayer(file);

			_player->RegisterPlayerListener(this);

			jgui::Component *cmp = _player->GetVisualComponent();

			cmp->SetBounds(0, 0, 1920, 1080);

			Add(cmp);
		}

		virtual ~PlayerTest()
		{
			_player->Stop();

			delete _player;
		}

		virtual void StartMedia()
		{
			_player->Play();
		}

		virtual void StopMedia()
		{
			_player->Stop();
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

	sleep(30);

	test.StopMedia();

	sleep(2);

	return 0;
}


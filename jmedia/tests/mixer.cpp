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
#include "jmedia/jaudiomixercontrol.h"

#include <iostream>
#include <thread>

int main(int argc, char **argv)
{
  jmedia::Player *player = jmedia::PlayerManager::CreatePlayer("mixer://");

  if (player == nullptr) {
    std::cout << "Audio mixer is not avaiable !" << std::endl;

    return -1;
  }

  jmedia::AudioMixerControl *control = dynamic_cast<jmedia::AudioMixerControl *>(player->GetControl("audio.mixer"));

  if (control == nullptr) {
    std::cout << "Audio mixer control is not avaiable !" << std::endl;

    return -1;
  }

  jmedia::Audio *music = control->CreateAudio("sounds/highlands.wav");

  music->SetLoopEnabled(true);

  jmedia::Audio *sound1 = control->CreateAudio("sounds/door3.wav");
  jmedia::Audio *sound2 = control->CreateAudio("sounds/door4.wav");

  sound1->SetVolume(1.0);
  sound2->SetVolume(0.5);

  control->StartSound(music);

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  int count = 100;

  while (count-- > 0) {
    control->StartSound(sound1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    control->StartSound(sound2);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  player->Stop();
  player->Close();

	return 0;
}


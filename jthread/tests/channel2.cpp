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
#include "jthread/jchannel.h"
#include "jthread/jlatch.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <optional>

int main() 
{
  jthread::Channel<std::string> chan;
  jthread::Latch wg;

  wg.Add(2);

  std::thread first(
    [&]() {
      for (int i = 0; i < 8; i++) {
        // Sending the message to the channel:
        chan << "from thread first: hello";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        chan.Send("from thread first: world");
      }
      wg.Arrive();
    });

  std::thread second(
    [&]() {
      for (int i = 0; i < 5; i++) {
        chan.Send("from thread second: hello");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        chan.Send("from thread second: world");
      }
      wg.Arrive();
    });

  // checker waits for all threads to finish their work and closes the channel.
  std::thread checker(
    [&]() {
      wg.Wait();
      chan.Close();
    });

  while (true) {
    auto item = chan.TryRecv();

    if (!item) {
      break;
    }

    std::cout << *item << std::endl;
  }

  std::cout << "All work has been done\n";

  first.join();
  second.join();
  checker.join();

  return 0;
}

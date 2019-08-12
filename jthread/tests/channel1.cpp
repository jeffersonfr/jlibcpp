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
#include <vector>

struct Result {
  int result;
  int id; // of the worker who provided the result
};

// Worker receives a number from the jobs/input channel and calculates +1, then it sends the calculated result to the results channel
void Worker(int id, jthread::Channel<int> *jobs, jthread::Channel<Result> *results, jthread::Latch *wg) 
{
  for (auto job : *jobs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    auto val = job.value();

    results->Send(Result {val + 1, id});
  }

  wg->Arrive();
}

int main() 
{
  std::vector<std::thread> workers;
  jthread::Channel<int> jobs;
  jthread::Channel<Result> results;
  jthread::Latch wg;

  // INFO:: spawning 20 Workers. 
  for (int i = 1; i <= 20; i++) {
    wg.Add(1);

    std::thread(
      [&, i]() {
        Worker(i, &jobs, &results, &wg);
      }).detach();
  }

  // Waiting for all threads to finish its work and then close the results channel.
  std::thread(
    [&]() {
      wg.Wait();
      results.Close();
    }).detach();

  std::thread(
    [&]() {
      // Sending work to the Worker:
      for (int i = 1; i <= 5000; i++) {
        jobs.Send(i);
      }
      jobs.Close();
    }).detach();

  while (true) {
    auto result = results.TryRecv();

    if (!result) {
      break;
    }

    auto resp = result;

    std::cout << "Result: " << resp->result << " from #" << resp->id << "\n";
  }

  return 0;
}


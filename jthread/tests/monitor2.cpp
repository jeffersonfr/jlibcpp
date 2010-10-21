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
#include "jmonitor.h"
#include "jthread.h"

#include <stdio.h>
#include <assert.h>

enum GAME_STATE {
  START_GAME,
  PLAYER_A,     // Player A playes the ball
  PLAYER_B,     // Player B playes the ball
  GAME_OVER,
  ONE_PLAYER_GONE,
  BOTH_PLAYERS_GONE
};

enum GAME_STATE XGameState;

jthread::Monitor XGameStateChange;

class XPlayerA : public jthread::Thread {

	public:
		virtual void Run()
		{
			// For access to game state variable
			XGameStateChange.Enter();

			// Play loop
			while ( XGameState < GAME_OVER ) {
				// Play the ball
				fprintf(stdout, "\nPLAYER-A\n");
				//cout << endl << "PLAYER-A" << endl;

				// Now its PLAYER-B's turn
				XGameState = PLAYER_B;

				// Signal to PLAYER-B that now it is his turn
				XGameStateChange.Notify();

				// Wait until PLAYER-B finishes playing the ball
				do {

					XGameStateChange.Wait();

					if ( PLAYER_B == XGameState ) {
						fprintf(stdout, "\n----PLAYER-A: SPURIOUS WAKEUP!!!\n");
						//cout << endl << "----PLAYER-A: SPURIOUS WAKEUP!!!" << endl;
						assert(0);
					}

				} while ( PLAYER_B == XGameState );
			}

			// PLAYER-A gone
			XGameState = (GAME_STATE)(XGameState+1);
			fprintf(stdout, "\nPLAYER-A GONE\n");
			//cout << endl << "PLAYER-A GONE" << endl;

			// No more access to state variable needed
			XGameStateChange.Exit();

			// Signal PLAYER-A gone event
			XGameStateChange.NotifyAll();
		}

};

class XPlayerB : public jthread::Thread {
	
	public:
		virtual void Run()
		{
			// For access to game state variable
			XGameStateChange.Enter();

			// Play loop
			while ( XGameState < GAME_OVER ) {
				// Play the ball
				fprintf(stdout, "\nPLAYER-B\n");
				//cout << endl << "PLAYER-B" << endl;

				// Now its PLAYER-A's turn
				XGameState = PLAYER_A;

				// Signal to PLAYER-A that now it is his turn
				XGameStateChange.Notify();

				// Wait until PLAYER-A finishes playing the ball
				do {

					XGameStateChange.Wait();

					if ( PLAYER_A == XGameState ) {
						fprintf(stdout, "\n----PLAYER-B: SPURIOUS WAKEUP!!!\n");
						//cout << endl << "----PLAYER-B: SPURIOUS WAKEUP!!!" << endl;
						assert(0);
					}

				} while ( PLAYER_A == XGameState );

			}

			// PLAYER-B gone
			XGameState = (GAME_STATE)(XGameState+1);
			fprintf(stdout, "\nPLAYER-B GONE\n");
			//cout << endl << "PLAYER-B GONE" << endl;

			// No more access to state variable needed
			XGameStateChange.Exit();

			// Signal PLAYER-B gone event
			XGameStateChange.NotifyAll();
		}
};

int main()
{
	// Set initial state
	XGameState = START_GAME;

	// Create players
	jthread::Thread *playerA = new XPlayerA(),
		*playerB = new XPlayerB();

	playerA->Start();
	playerB->Start();

	// Give them 5 sec. to play
	jthread::Thread::Sleep(5);

	// Set game over state
	XGameStateChange.Enter();
	XGameState = GAME_OVER;

	// Let them know
	XGameStateChange.NotifyAll();

	// Wait for players to stop
	do {
		XGameStateChange.Wait();
	} while ( XGameState < BOTH_PLAYERS_GONE );

	// Cleanup
	fprintf(stdout, "\nGAME OVER\n");
	//cout << endl << "GAME OVER" << endl;
	XGameStateChange.Exit();

	playerA->WaitThread();
	playerB->WaitThread();

	return 0;
}


#include "jmonitor.h"

#include <stdio.h>

enum GAME_STATE {
	START_GAME,
	PLAYER_A,     // Player A playes the ball
	PLAYER_B,     // Player B playes the ball
	GAME_OVER,
	ONE_PLAYER_GONE,
	BOTH_PLAYERS_GONE

};

enum GAME_STATE YGameState;

jthread::Monitor YGameStateChange;

class YPlayerA : public jthread::Thread {

	public:
		virtual void Run()
		{
			// For access to game state variable
			YGameStateChange.Enter();

			// Play loop
			while ( YGameState < GAME_OVER ) {
				// Play the ball
				fprintf(stdout, "\nPLAYER-A\n");
				//cout << endl << "PLAYER-A" << endl;

				// Now its PLAYER-B's turn
				YGameState = PLAYER_B;

				// Signal to PLAYER-B that now it is his turn
				YGameStateChange.NotifyAll();

				// Wait until PLAYER-B finishes playing the ball
				do {

					YGameStateChange.Wait();

					if ( PLAYER_B == YGameState )
						fprintf(stdout, "\n----PLAYER-A: SPURIOUS WAKEUP!!!\n");
				} while ( PLAYER_B == YGameState );

			}

			// PLAYER-A gone
			YGameState = (GAME_STATE)(YGameState+1);
			//cout << endl << "PLAYER-A GONE" << endl;
			fprintf(stdout, "\nPLAYER-A GONE\n");

			// No more access to state variable needed
			YGameStateChange.Exit();

			// Signal PLAYER-A gone event
			YGameStateChange.NotifyAll();
		}

};

class YPlayerB : public jthread::Thread {

	public:
		virtual void Run()
		{
			// For access to game state variable
			YGameStateChange.Enter();

			// Play loop
			while ( YGameState < GAME_OVER ) {
				// Play the ball
				//cout << endl << "PLAYER-B" << endl;
				fprintf(stdout, "\nPLAYER-B\n");

				// Now its PLAYER-A's turn
				YGameState = PLAYER_A;

				// Signal to PLAYER-A that now it is his turn
				YGameStateChange.NotifyAll();

				// Wait until PLAYER-A finishes playing the ball
				do {

					YGameStateChange.Wait();

					if ( PLAYER_A == YGameState )
						//cout << endl << "----PLAYER-B: SPURIOUS WAKEUP!!!" << endl;
						fprintf(stdout, "\n----PLAYER-B: SPURIOUS WAKEUP!!!\n");
				} while ( PLAYER_A == YGameState );
			}

			// PLAYER-B gone
			YGameState = (GAME_STATE)(YGameState+1);
			//cout << endl << "PLAYER-B GONE" << endl;
			fprintf(stdout, "\nPLAYER-B GONE\n");

			// No more access to state variable needed
			YGameStateChange.Exit();

			// Signal PLAYER-B gone event
			YGameStateChange.NotifyAll();
		}

};

int main()
{
	// Set initial state
	YGameState = START_GAME;

	// Create players
	// Create players
	jthread::Thread *playerA = new YPlayerA(),
		*playerB = new YPlayerB();

	playerA->Start();
	playerB->Start();

	// Give them 5 sec. to play
	jthread::Thread::Sleep(5);

	// Make some noise
	YGameStateChange.Enter();
	//cout << endl << "---Noise ON..." << endl;
	fprintf(stdout, "\n---Noise ON...\n");
	YGameStateChange.Exit();
	for ( int i = 0; i < 5000; i++ ) {
		YGameStateChange.NotifyAll();
	}
	//cout << endl << "---Noise OFF" << endl;
	fprintf(stdout, "\n---Noise OFF\n");

	// Set game over state
	YGameStateChange.Enter();
	YGameState = GAME_OVER;

	// Let them know
	YGameStateChange.NotifyAll();

	// Wait for players to stop
	do {
		YGameStateChange.Wait();
	} while ( YGameState < BOTH_PLAYERS_GONE );

	// Cleanup
	//cout << endl << "GAME OVER" << endl;
	fprintf(stdout, "\nGAME OVER\n");
	YGameStateChange.Exit();

	playerA->WaitThread();
	playerB->WaitThread();

	return 0;
}



#include"Boards.h"


int main() {

	//seed to generate random number
	srand(time(0));

	//print interface

	cout << "\n\n***** Welcome to the Ultimate TIC-TAC-TOE *****\n" << endl
		<< "Player one is X" << endl
		<< "Player two is O\n" << endl
		<< "Rules: " << endl
		<< "1) each turn choose the little board you want to play with by choosing a number from 1 to 9" << endl
		<< "2) choose the square you want to place in X/O by also choosing a number from 1 to 9" << endl;

	// set the game object
	Ultimate U;

	// run the two rounds
	for (int round = 0; round < 2; round++) {

		cout << "\nRound:" << round+1 <<'\n' << endl;
		U.print_game(round);

		// if the board staus still empty (no one won or tied)
		while (U.get_board(round).get_winB() == '-') {
			int board;

		
			cout << "Player one Turn" << endl;
			U.play_game(round, 'X');
			U.print_game(round);

			// if X won break the loop
			if (U.get_board(round).get_winB() == 'X')
				break;

			cout << "Player two Turn" << endl;
			U.play_game(round, 'O');
			U.print_game(round);

		}
	}

	// will play the third round if no one has already won
	U.decide_winner();

	return 0;
}




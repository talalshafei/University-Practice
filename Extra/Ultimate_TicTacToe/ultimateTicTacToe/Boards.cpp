


#include "Boards.h"


// helper functions

void choose(int& c) {
	//function to choose enteger from 0->9
	cin >> c;
	while (c < 1 || c >9) {
		cout << "invalid, please pick one from 0 - > 9: ";
		cin >> c;
	}
}

void put_dimensions(int a, int& r, int& c) {
	//function to  convert to an integer from (0,9) to array dimensions 
	//a = 3*r + c +1

	r = (a-1) / 3 ;
	c = (a-1) % 3;
}



//Small_board class' methods//////////////////////
Small_board::Small_board() {
	
	winS = '-';
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			small[r][c] = '-';

}

void Small_board::check_filled(int &square) {
	int r, c;
	put_dimensions(square, r, c);
	while (small[r][c] != '-') {
		cout << "Position already played, please choose a new one: ";
		choose(square);
		put_dimensions(square, r, c);
	}
}

void Small_board::update_small_status() {

	//Horizontal lines
	for (int r = 0; r < 3; r++) {
		if ((small[r][0] == 'X') && (small[r][1] == 'X') && (small[r][2] == 'X'))
			winS = 'X';
		else if ((small[r][0] == 'O') && (small[r][1] == 'O') && (small[r][2] == 'O'))
			winS = 'O';
	}
	//Vertical lines
	for (int c = 0; c < 3; c++) {
		if ((small[0][c] == 'X') && (small[1][c] == 'X') && (small[2][c] == 'X'))
			winS = 'X';
		else if ((small[0][c] == 'O') && (small[1][c] == 'O') && (small[2][c] == 'O'))
			winS = 'O';
	}
	
	// Diagonals
	if ((small[0][0] == 'X') && (small[1][1] == 'X') && (small[2][2] == 'X'))
		winS = 'X';

	else if ((small[0][0] == 'O') && (small[1][1] == 'O') && (small[2][2] == 'O'))
		winS = 'O';
	
	//Anti Diagonals

	if ((small[0][2] == 'X') && (small[1][1] == 'X') && (small[2][0] == 'X'))
		winS = 'X';

	else if ((small[0][2] == 'O') && (small[1][1] == 'O') && (small[2][0] == 'O'))
		winS = 'O';

	//if no one won check if board is not empty
	bool empty = false;
	if (winS != 'X' && winS != 'O') {
		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++)
				if (small[r][c] == '-') {
					empty = true;
					break;
				}
			if (empty)
				break;
		}
		//calculate the winner
		if (!empty)
			winS = !(*this) ? 'X' : 'O';
	}
	

}

int Small_board::operator!() {
	// when it's a tie in a small board calculate the winner using the method in the manual
	int X = 0, O = 0;
	for(int r=0;r<2;r++)
		for (int c = 0; c < 2; c++) {
			if (small[r][c] == 'X')
				X += 3 * r + c + 1;
			else
				O += 3 * r + c + 1;
		}
	if (X > O)
		return 1;
	else
		return 0;
}
void Small_board::play_small(const char& played) {
	
	int square;
	cout <<"Enter number of the square:";

	choose(square);
	check_filled(square);

	int r, c;

	put_dimensions(square, r, c);

	small[r][c] = played;

	// to see if the board full or someone won or we still playable
	update_small_status();

}

bool Small_board::small_finished() {
	if (winS == '-')
		return false;

	return true;
}


//Big_board class' methods //////////////////////////////////////
Big_board::Big_board() {
	winB = '-';
	
}

char Big_board::get_winB()const {
	return winB;
}

void Big_board::print_big_board() {
	cout << endl;
	for (int r1 = 0; r1 < 3; r1++) {
		for (int r2 = 0; r2 < 3; r2++) {
			for (int c1 = 0; c1 < 3; c1++) {
				for (int c2 = 0; c2 < 3; c2++) {
					cout << little_boards[r1][c1].small[r2][c2] << "   ";
				}
				cout << "    ";
			}
			cout << endl;
		}
		cout <<'\n' << endl;
	}
}

void Big_board:: play_big(const char& played) {

	cout<< "Enter the number of the board: ";
	int board;
	choose(board);
	int r, c;
	put_dimensions(board, r, c);
	
	while(little_boards[r][c].small_finished()) {
		cout << "Board has finished, please choose another board: ";
		choose(board);
		put_dimensions(board, r, c);
	}

	little_boards[r][c].play_small(played);
	// to see if someone won or the board is full or is it a tie
	update_big_status();

}

void Big_board::update_big_status() {
	//Horizontal lines
	for (int r = 0; r < 3; r++) {
		if (little_boards[r][0].winS == 'X' && little_boards[r][1].winS == 'X' && little_boards[r][2].winS == 'X')
			winB = 'X';
		else if (little_boards[r][0].winS == 'O' && little_boards[r][1].winS == 'O' && little_boards[r][2].winS == 'O')
			winB = 'O';
	}
	//Vertical lines
	for (int c = 0; c < 3; c++) {
		if (little_boards[0][c].winS == 'X' && little_boards[1][c].winS == 'X' && little_boards[2][c].winS == 'X')
			winB = 'X';
		else if (little_boards[0][c].winS == 'O' && little_boards[1][c].winS == 'O' && little_boards[2][c].winS == 'O')
			winB = 'O';
	}

	// Diagonals
	if (little_boards[0][0].winS == 'X' && little_boards[1][1].winS == 'X' && little_boards[2][2].winS == 'X')
		winB = 'X';

	else if (little_boards[0][0].winS == 'O' && little_boards[1][1].winS == 'O' && little_boards[2][2].winS == 'O')
		winB = 'O';

	//Anti Diagonals

	if (little_boards[0][2].winS == 'X' && little_boards[1][1].winS == 'X' && little_boards[2][0].winS == 'X')
		winB = 'X';

	else if (little_boards[0][2].winS == 'O' && little_boards[1][1].winS == 'O' && little_boards[2][0].winS == 'O')
		winB = 'O';

	//if no one won check if board is not empty
	bool empty = false;
	if (winB != 'X' && winB != 'O') {
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++)
				if (little_boards[r][c].winS == '-') {
					empty = true;
					break;
				}
		// the big board is full but no winner 
		if (!empty)
			winB = 'F';
	}
	
}


//Ultimate class' methods //////////////////////////////////////

Ultimate::Ultimate() {
	winU = '-';
}

void Ultimate::print_game(int round) {
	game_boards[round].print_big_board();
}

char Ultimate::get_winU()const {
	return winU;
}

//will be used in the main as a condition in the loop
Big_board Ultimate::get_board(int round) const {
	return game_boards[round];
}

// start the game for the round
void Ultimate::play_game(int round, const char& played) {
	game_boards[round].play_big(played);
}


// after playing two rounds this function will announce the winner 
// if no winner exist the function will do the randomness effect as explained in the pdf  with probability 50%
void Ultimate::decide_winner() {
	char winner_flag;
	//check if someone won the game
	if ((game_boards[0].get_winB() == game_boards[1].get_winB()) && (game_boards[0].get_winB() != 'F'))
		winner_flag = game_boards[0].get_winB();

	else {
		cout << "since no one won two consecutive rounds we will play the third round!" << endl;
		// generate a random number with probability 50% to be even or odd

		int random = int(rand()) % 2;
		// if one of them or both is not F then one of them must be X or O so we can play + and -
		if (game_boards[0].get_winB() != 'F' || game_boards[1].get_winB() != 'F') {
			cout << "\nRound: 3" << endl;
			if (random) {
				cout << "we added the Boards '+' !" << endl;
				winner_flag = game_boards[0] + game_boards[1];
			}
			else {
				cout << "we subtracted the Boards '-' !" << endl;
				winner_flag = game_boards[0] - game_boards[1];
			}
		}
		else {
			cout << "Since boards are Full but no winner in any of them, we will flip a coin to determine the winner!" << endl;
			if (random)
				winner_flag = 'X';
			else
				winner_flag = 'O';
		}

	}


	if (winner_flag == 'X')
		cout << "\n***** Player One won the game *****\n" << endl;

	if (winner_flag == 'O')
		cout << "\n***** Player Two won the game *****\n" << endl;
}

char Big_board:: operator+(const Big_board& B) {
	//implemented as explained in the pdf
	if (winB == 'X' || B.winB == 'X')
		return 'X';
	else if (winB == 'O' || B.winB == 'O')
		return 'O';
	else 
		return 'F';

	
}
char Big_board :: operator-(const Big_board& B) {
	if (winB == 'O' || B.winB == 'O')
		return 'O';
	else if (winB == 'X' || B.winB == 'X')
		return 'X';
	else
		return '-';
}
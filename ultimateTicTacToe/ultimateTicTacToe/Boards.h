
#pragma once
#define _CRT_SEURE_NO_WARNINGS
#include<iostream>
#include <cstdlib>
#include<ctime>

using namespace std;


class Small_board {
private:
	char small[3][3];
	char winS;
public:
	Small_board();
	void play_small(const char&);
	void check_filled(int&);
	void update_small_status();

	bool small_finished();
	int operator!();

	friend class Big_board;

	
};

class Big_board{
private:
	Small_board little_boards[3][3];
	char winB;
public:
	Big_board();
	char get_winB() const;
	void print_big_board();
	void play_big(const char&);
	void update_big_status();

	char operator+(const Big_board&);
	char operator-(const Big_board&);


};

class Ultimate {
private:
	Big_board game_boards[2];
	char winU;
public:
	Ultimate();
	void print_game(int);
	Big_board get_board(int)const;
	char get_winU() const;
	void play_game(int, const char&);



	void decide_winner();

};

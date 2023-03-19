# Overview

Ultimate Tic-Tac-Toe is a new version of the old game Tic-Tac-Toe

Game rules: 

* To win you need to win at least two rounds
* Each turn, you mark one of the small squares.
* When you get three in a row (horizontal, vertical or diagonal) on a small board, you
win that board.
* To win the round, you need to win three small boards in a row (horizontal, vertical or 
diagonal).
* The order of the small board games should be as 1, 2, 3, 4, 5, 6, 7, 8, and 9
* If a small board ends up as a tie, the "!" operator is employed to decide who won that position. The "!" operator counts the total 
position score for X and O individually. Considering the cells numbered from 1 to 9, the total values for positions will be computed. In turn, 
the operator will return 0 if O wins and 1 if X wins.
* Unlike the small boards, If the large board ends in a tie, the round will prompt the users that it is a 
draw.
* The third round is different since we include some additional operators. At the 
end of round two, the big boards of the first two rounds should be stored. Following 
this, these boards will be added (overload "+" operator) or subtracted (overload "-
"operator). The operator ("+" or "-") is determined randomly with a 50% probability for 
each. the "+" gives X the win if he won one of the previous games, and "-" do the same for "O"
*  At the end of addition or subtraction, if we do not have a situation where one side wins 
and the other loses, we generate another random number to decide who wins with a 50% 
chance for each player.

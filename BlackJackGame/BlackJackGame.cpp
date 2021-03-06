// BlackJackGame.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <limits>

//We need this to print out unicode characters.
#include <fcntl.h>
#include <io.h>



//Convert a string to all lower case characters
void lowerCaseFunction(std::string& word);


class Card {
private:
	int val;
	int suit;

public:
	Card(int theValue, int theSuit) {
		this->val = theValue;
		//For the Suit, 1->Heart, 2->Diamond, 3->Spade, 4->Club
		this->suit = theSuit;
	}

	int getIntegerValue();
	char getValue();
	int getSuit();
};

int Card::getIntegerValue() {
	return this->val;
}


char Card::getValue() {
	
	//Return the proper character
	if (this->val == 1) {
		return 'A';
	}
	else if (this->val == 11)
	{
		return 'J';
	}
	else if (this->val == 12)
	{
		return 'Q';
	}
	else if (this->val == 13)
	{
		return 'K';
	}
	else {
		return (char)(this->val + 48);
	}
}

int Card::getSuit() {
	return this->suit;
}





class BlackJack {

private:

	std::vector<Card*> yourHand;
	std::vector<Card*> houseHand;

	int yourHandSum;
	int yourHandSumTwo;
	int houseHandSum;
	int houseHandSumTwo;

	std::unordered_map<int, int > myMap;

public:
	
	//Bet size
	static int betSize;
	//This will static variable will hold all the money that the user has.
	static double yourHandMoney;

	//Constructor
	BlackJack();
	//Destructor
	~BlackJack();

	//If we get a zero, display yourHand and also get the sum.
	//If we get a one, display houseHand and also get the sum.
	void getSumAndDisplayHand(int i);

	//Get another card
	Card* hit();

	//Helper function for the hit function.
	//Do we have a valid values and suit?
	bool isValid(int val, int suit);

	//It is the players turn
	int playersTurn();

	//It is the houses turn
	int houseTurn();

	//Getters for the hand sum of the player
	int getYourHandSum();
	int getYourHandSumTwo();
	
	//Getters for the hand sum of the house
	int getHouseHandSum();
	int getHouseHandSumTwo();

	//Add a card to the player
	void updateYourHand(Card* theCard);
	
	//Add a card to the dealer
	void updateHouseHand(Card* theCard);



	//Modify our sum if we are dealing with card of value: 'A'
	void modifyYourSumIfCharacterA();

	//Modify our sum if we are dealing with card of value: 'A'
	//This is for the case that we the hand has card: 'A'
	void modifyHouseSumIfCharacterA();
};

//We need to tell the compiler that we are using a static variables
int BlackJack::betSize = 0;
double BlackJack::yourHandMoney = 1000.00;



//Define our Constructor
BlackJack::BlackJack() {

	//We need to start the game
	int betting;
	std::wcout << "How much do you want to bet: $";
	std::cin >> betting;

	//Validate that we have a number
	while (1)
	{
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::wcout << "Try Again, please enter a bet number: $";
			std::cin >> betting;
		}
		if (!std::cin.fail())
			break;
	}	



	while ((BlackJack::yourHandMoney - betting) < 0 || betting == 0)
	{
		std::wcout << "\nTry Again.";
		std::wcout << "Bet any amount between [1 to " << BlackJack::yourHandMoney << "]\n";
		std::wcout << "How much do you want to bet: $";
		std::cin >> betting;

		//Validate that we have a number
		while (1)
		{
			if (std::cin.fail())
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::wcout << "Try Again, please enter a bet number: $";
				std::cin >> betting;
			}
			if (!std::cin.fail())
				break;
		}

	}

	std::cin.ignore();
	//BetSize is valid
	BlackJack::betSize = betting;



	Card* myCardOne = hit();
	Card* myCardTwo = hit();
	Card* myCardThree = hit();

	yourHand.push_back(myCardOne);
	houseHand.push_back(myCardTwo);
	yourHand.push_back(myCardThree);

	//Display the cards of the dealer
	getSumAndDisplayHand(1);
	std::wcout << "\n";
	//Display the cards of the player
	getSumAndDisplayHand(0);
}

//Define our destructor
BlackJack::~BlackJack() {
	for (unsigned int i = 0; i < BlackJack::yourHand.size(); i++) {
		delete BlackJack::yourHand[i];
	}
	for (unsigned int i = 0; i < BlackJack::houseHand.size(); i++) {
		delete BlackJack::houseHand[i];
	}
}



//Get another card
Card* BlackJack::hit()
{
	//Get a valid card	
	//For the value, {1,2,3,4,... ,12,13}
	//Note: 1->A, 11->J, 12->Q, 13->K
	
	//For the Suit, 1->Heart, 2->Diamond, 3->Spade, 4->Club

	int val;
	int suit;
	do {
		val = (rand() % 13) + 1;
		suit = (rand() % 4) + 1;
	} while ( !isValid(val, suit) );
	
	//We can now construct a valid card
	Card* theCard = new Card(val, suit);
	//Update our map
	myMap[val] = suit;
	return theCard;
}

//Do we have a valid values and suit?
bool BlackJack::isValid(int val, int suit) {

	//We have to check our map
	if (myMap.find(val) == myMap.end()) {
		//The given key has not been found.
		//We have a valid key and also a valid suit.
		return true;
	}

	if (myMap[val] != suit) {
		return true;
	}

	return false;
}




//If we get a zero, display yourHand and also get the sum.
//If we get a one, display houseHand and also get the sum.
void BlackJack::getSumAndDisplayHand(int i) {
	
	int localSum = 0;
	//This is needed for the case that we are dealing with an A
	int localSumTwo = 0;
	bool flagA = 0;


	if (i == 0) {
		std::wcout << "\nYour Hand : ";
		//Display yourHand and return its sum
		for (unsigned int i = 0; i < yourHand.size(); i++) {
			Card* currentCard = yourHand[i];

			char ch = currentCard->getValue();
			int suit = currentCard->getSuit();
		
			
			if (ch == 'A') {
				flagA = 1;
				localSum = localSum + 1;
				localSumTwo = localSumTwo + 11;
			}
			else if (ch == 'J' || ch == 'Q' || ch == 'K') {
				localSum = localSum + 10;
				localSumTwo = localSumTwo + 10;
			}
			else {
				localSum = localSum + currentCard->getIntegerValue();
				localSumTwo = localSumTwo + currentCard->getIntegerValue();
			}
			
			switch (suit)
			{
				case 1:
					//Heart
					if ((int)ch == 58) {
						//For the case of printing 10
						wprintf(L"[10,\x2665]");

					}
					else {
						wprintf(L"[%c,\x2665]", ch);
					}
					std::wcout << "  ";
					break;
				case 2:
					//Diamond
					if ((int)ch == 58) {
						//For the case of printing 10
						wprintf(L"[10,\x2666]");

					}
					else {
						wprintf(L"[%c,\x2666]", ch);
					}
					std::wcout << "  ";
					break;
				case 3:
					//Spade
					if ((int)ch == 58) {
						//For the case of printing 10
						wprintf(L"[10,\x2660]");

					}
					else {
						wprintf(L"[%c,\x2660]", ch);
					}
					std::wcout << "  ";
					break;
				case 4:
					//Club
					if ((int)ch == 58) {
						//For the case of printing 10
						wprintf(L"[10,\x2663]");

					}
					else {
						wprintf(L"[%c,\x2663]", ch);
					}
					std::wcout << "  ";
					break;
				default:
					break;
			}
		
		}

		if (flagA == 1 && localSumTwo < 22) {
			//We are dealing with an A
			//We need to print two sums
			std::wcout << "\nYour Hand Sum : "
				<< localSum << "/" << localSumTwo << "\n";

			this->yourHandSumTwo = localSumTwo;
		}
		else {
			std::wcout << "\nYour Hand Sum : " << localSum <<"\n";
		
		}
		this->yourHandSum = localSum;
	}
	else {
		//Display houseHand and return its sum
		std::wcout << "\nDealers HAND : ";
		//Display yourHand and return its sum
		for (unsigned int i = 0; i < houseHand.size(); i++) {
			Card* currentCard = houseHand[i];

			char ch = currentCard->getValue();
			int suit = currentCard->getSuit();
			
			if (ch == 'A') {
				flagA = 1;
				localSum = localSum + 1;
				localSumTwo = localSumTwo + 11;
			}
			else if (ch == 'J' || ch == 'Q' || ch == 'K') {
				localSum = localSum + 10;
				localSumTwo = localSumTwo + 10;
			}
			else {
				localSum = localSum + currentCard->getIntegerValue();
				localSumTwo = localSumTwo + currentCard->getIntegerValue();
			}

			switch (suit)
			{
			case 1:
				//Heart
				if ((int)ch == 58) {
					//For the case of printing 10
					wprintf(L"[10,\x2665]");

				}
				else {
					wprintf(L"[%c,\x2665]", ch);
				}
				std::wcout << "  ";

				if (houseHand.size() == 1)
					wprintf(L"[?,?]");

				break;
			case 2:
				//Diamond
				if ((int)ch == 58) {
					//For the case of printing 10
					wprintf(L"[10,\x2666]");

				}
				else {
					wprintf(L"[%c,\x2666]", ch);
				}
				std::wcout << "  ";
				if (houseHand.size() == 1)
					wprintf(L"[?,?]");


				break;
			case 3:
				//Spade
				if ((int)ch == 58) {
					//For the case of printing 10
					wprintf(L"[10,\x2660]");

				}
				else {
					wprintf(L"[%c,\x2660]", ch);
				}
				std::wcout << "  ";

				if (houseHand.size() == 1)
					wprintf(L"[?,?]");

				break;
			case 4:
				//Club
				if ((int)ch == 58) {
					//For the case of printing 10
					wprintf(L"[10,\x2663]");

				}
				else {
					wprintf(L"[%c,\x2663]", ch);
				}

				std::wcout << "  ";
				
				if (houseHand.size() == 1) 
					wprintf(L"[?,?]");

				
				break;
			default:
				break;
			}
		}

		if (flagA == 1 && localSumTwo < 22) {
			//We are dealing with an A
			//We need to print two sums
			std::wcout << "\nDEALERS HAND SUM : "
				<< localSum << "/" << localSumTwo << "\n";
			
			this->houseHandSumTwo = localSumTwo;

		}
		else {
			std::wcout << "\nDEALERS HAND SUM : " << localSum << "\n";
			
		}


		this->houseHandSum = localSum;

	
	}
}





int BlackJack::houseTurn() {

	
	//This is the players hand that we have to beat
	int sumToBeat = getYourHandSum();
	std::wcout << "\nThe dealer will try to beat your hand sum of: "<< sumToBeat<<"\n";


	//This is the sum of the hand
	int currentSum = houseHandSum;

	//It is the turn of the house
	while (currentSum < 17)
	{
		if (currentSum > 22) {
			//The house loses
			return 0;
		}

		std::wcout << "The dealer will get another card.\n";
		std::wcout << "\nPress ENTER to continue...";
		std::string temp = "";
		std::getline(std::cin, temp);

		Card* theCard = hit();
		updateHouseHand(theCard);
		getSumAndDisplayHand(1);
		modifyHouseSumIfCharacterA();
		currentSum = getHouseHandSum();

		if (currentSum == 21) {
			
			if (sumToBeat == 21) {
				//Tied game
				return 3;
			}
			//The house wins
			return 1;
		}
		if (currentSum > 21)
		{
			//HOUSE BUST
			return 0;
		}
	
	}

	std::wcout << "\n\nYOUR HAND: " << sumToBeat
		<< " VS DEALERS HAND: " << currentSum << "\n";

	//Now we compare to see who wins
	if (sumToBeat == currentSum)
		return 3;
	else if (sumToBeat < currentSum)
		return 1;
	else
		return 0;
}




int BlackJack::getYourHandSum() {
	return yourHandSum;
}
int BlackJack::getYourHandSumTwo() {
	return yourHandSumTwo;
}

int BlackJack::getHouseHandSum() {
	return houseHandSum;
}

int BlackJack::getHouseHandSumTwo() {
	return houseHandSumTwo;
}


void BlackJack::updateYourHand(Card* theCard){
	yourHand.push_back(theCard);
}
void BlackJack::updateHouseHand(Card* theCard) {
	houseHand.push_back(theCard);
}

//Modify our sum if we are dealing with card of value: 'A'
void BlackJack::modifyYourSumIfCharacterA() {

	if (yourHandSum != yourHandSumTwo && yourHandSumTwo != 0) {

		if (yourHandSumTwo < 22)
		{
			//This is valid
			yourHandSum = yourHandSumTwo;
		}
	}
}

//Modify our sum if we are dealing with card of value: 'A'
//This is for the case of the hosue hand
void BlackJack::modifyHouseSumIfCharacterA() {

	if (houseHandSum != houseHandSumTwo) {

		if (houseHandSumTwo >16  &&  houseHandSumTwo < 22)
		{
			//This is valid
			houseHandSum = houseHandSumTwo;
		}
	}
}



int BlackJack::playersTurn() {

	bool yourTurn = true;
	std::string theMove;
	int sum = getYourHandSum();
	bool firstMove = 1;
	bool allowDouble = 0;

	if (getYourHandSumTwo() == 21) {
		//Player automatically wins
		std::wcout << "BLACKJACK\nYOU WIN.\n";
		return 0;
	}

	
	while (yourTurn && sum < 21 )
	{
		//We still have to implement split
		if (firstMove) {
			bool canWeDouble = ( (2 * BlackJack::betSize) <= BlackJack::yourHandMoney) ? 1 : 0;

			if (canWeDouble) {
				std::wcout << "Your turn {hit , stand , double} : ";
				allowDouble = 1;
			}
			else {
				std::wcout << "Note: You don't have enough money to play double.\n"
							<< "Your turn {hit , stand} : ";
				allowDouble = 0;
			}
			firstMove = 0;
		}
		else {
			std::wcout << "Your turn {hit , stand} : ";
			allowDouble = 0;
		}

		std::getline(std::cin, theMove);
		lowerCaseFunction(theMove);

		if (theMove == "stand")
		{
			//Player Stands, its the turn of the house
			
			//When we are dealing with an 'A', take the largest value
			getSumAndDisplayHand(0);
			modifyYourSumIfCharacterA();
			return 1;
		}
		else if(theMove == "hit"){
			//The player wants to hit
			Card* theCard = hit();
			updateYourHand(theCard);
			getSumAndDisplayHand(0);
			sum = getYourHandSum();

			if (sum == 21) {
				//Player automatically wins
				std::wcout << "BLACKJACK\nYOU WIN.\n";
				return 0;
			}
			
			if(sum > 21)
			{
				//Player automatically loses
				std::wcout << "BUST AT: " << sum <<"\nYOU LOSE.\n";
				return 4;
			}

		}
		else if (theMove == "double" && allowDouble) {
			//For typos, it is now the turn of the house

			//Three cases
			//The player wants to hit
			Card* theCard = hit();
			updateYourHand(theCard);
			getSumAndDisplayHand(0);
			modifyYourSumIfCharacterA();
			sum = getYourHandSum();

			if (sum == 21) {
				//Player automatically wins
				std::wcout << "BLACKJACK\nYOU WIN DOUBLE.\n";
				return 6;
			}
			if (sum > 21)
			{
				//Player automatically loses
				std::wcout << "BUST AT: " << sum << "\nYOU LOSE DOUBLE.\n";

				return 5;
			}
			//It is now the turn of the house, 
			//return 2 for a double game
			return 2;
		}
		else {
			//For typos, it is now the turn of the house
			return 1;
		}
	}
}








int main()
{
	//We need this to print out unicode characters.
	_setmode(_fileno(stdout), _O_U16TEXT);
	srand(time(NULL));


	std::wcout << "\nLet's play a game of BlackJack\n\n";

	std::string startNewGame;

	std::wcout << "You have: $" << BlackJack::yourHandMoney << "\n";
	std::wcout << "Would you like to start a new game (Yes or No) : ";
	std::getline(std::cin, startNewGame);
	lowerCaseFunction(startNewGame);

	while (startNewGame == "yes") {

		

		BlackJack theGame;

		//It is the players turn
		//If num == 0, player wins by blackjack
		//If num == 1, players stands, its the dealers turn
		//If num == 2, player doubles, its the dealers turn
		//If num == 4, player loses by bust
		//If num == 5, player loses (double)
		//If num == 6, player wins (double)


		int num = theGame.playersTurn();
		
		int houseWon;
		if (num == 1 || num == 2) {
			//We continue playing
			//Its the the turn of the house

			if (num == 1)
			{
				std::wcout << "\n\nDealers Turn";
				houseWon = theGame.houseTurn();

				if (houseWon == 0) {
					//We need to reward the winner with the same amount of the bet
					BlackJack::yourHandMoney = BlackJack::yourHandMoney + BlackJack::betSize;
					std::wcout << "\nPlayer WINS   Dealer LOSES, Therefore you have won: $" << BlackJack::betSize << "\n";


				}
				else if (houseWon == 1) {
					//We need to subtract his bet from his total hand money
					BlackJack::yourHandMoney = BlackJack::yourHandMoney - BlackJack::betSize;
					std::wcout << "\nPlayer LOSES   Dealer WINS, Therefore you have lost: $"<< BlackJack::betSize <<"\n";

				}
				else {
					//No modifications to the money
					std::wcout << "\nTied game: PUSH, Therefore you do not win or lose money\n";
				}
			}
			else
			{
				//A game of doubles
				std::wcout << "\n\nDealers Turn (This is a double game)";
				houseWon = theGame.houseTurn();

				if (houseWon == 0) {
					//We need to reward the winner with double the amount of the bet
					int betDouble = 2 * (BlackJack::betSize);
					BlackJack::yourHandMoney = BlackJack::yourHandMoney + betDouble;
					std::wcout << "\nPlayer WINS   Dealer LOSES, Therefore you have won: $" << betDouble << "\n";


				}
				else if (houseWon == 1) {
					//We need to subtract double the amount of the bet
					int betDouble = 2 * (BlackJack::betSize);
					BlackJack::yourHandMoney = BlackJack::yourHandMoney - betDouble;
					std::wcout << "\nPlayer LOSES   Dealer WINS, Therefore you have lost: $" << betDouble << "\n";

				}
				else {
					//No modifications to the money
					std::wcout << "\nTied game: PUSH, Therefore you do not win or lose money\n";
				}

			}

			

		}

		//If num == 0, player wins by blackjack
		if (num == 0) {
			//We need to reward the winner with 1.5 of the bet
			BlackJack::yourHandMoney = BlackJack::yourHandMoney + (1.5 * BlackJack::betSize);
			std::wcout << "\nPlayer WINS   Dealer LOSES, Therefore you have won: $" << (1.5 * BlackJack::betSize) << "\n";
		}

		//If num == 4, player loses by bust
		if (num == 4) {
			//We need to subtract his bet from his total hand money
			BlackJack::yourHandMoney = BlackJack::yourHandMoney - BlackJack::betSize;
			std::wcout << "\nPlayer WINS   Dealer LOSES, Therefore you have lost: $" << BlackJack::betSize << "\n";
		}
		

		//If num == 5, player loses (double)
		if (num == 5) {
			//We need to subtract double the amount of the bet
			int betDouble = 2 * (BlackJack::betSize);
			BlackJack::yourHandMoney = BlackJack::yourHandMoney - betDouble;
			std::wcout << "\nPlayer LOSES   Dealer WINS, Therefore you have lost: $" << betDouble << "\n";
		}
		//If num == 6, player wins (double)
		if (num == 6) {
			//We need to subtract double the amount of the bet
			int betDouble = 2 * (BlackJack::betSize);
			BlackJack::yourHandMoney = BlackJack::yourHandMoney + betDouble;
			std::wcout << "\nPlayer WINS    Dealer LOSES, Therefore you have won: $" << betDouble << "\n";
		}


		if (BlackJack::yourHandMoney != 0) {
			std::wcout << "\n\nYou have: $" << BlackJack::yourHandMoney << "\n";
			std::wcout << "Would you like to start a new game (Yes or No) : ";
			std::getline(std::cin, startNewGame);
			lowerCaseFunction(startNewGame);
		}
		else {
			std::wcout << "\n\nGAME OVER\n";
			std::wcout << "YOU HAVE LOST ALL YOUR MONEY\n";
			std::wcout << "Thank you for playing BlackJack with me.\n";
			std::wcout << "Have a good day.\n\n\n";
			return 0;
		}
	}



	std::wcout << "\n\n\n\n\nCALCULATING YOUR WINS\n";
	if (BlackJack::yourHandMoney >= 1000) {
		wprintf(L"\x263B\x263B\x263B\x263B\x263B TODAY WAS YOUR LUCKY DAY \x263B\x263B\x263B\x263B\x263B\n");
		std::wcout << "Your take home amount is: $" << BlackJack::yourHandMoney << " (fake money).\n";

	}
	else {
		wprintf(L"TODAY WAS A BAD DAY\n");
		std::wcout << "Your take home amount is: $" << BlackJack::yourHandMoney << " (fake money).\n";
	}
	std::wcout << "\nThank you for playing BlackJack with me.\n";
	std::wcout << "Have a good day.\n\n\n";

    return 0;
}


//Convert a string to all lower case characters
void lowerCaseFunction(std::string& word) {
	
	if (word.empty()) {
		return;
	}

	for (unsigned int i = 0; i < word.length(); i++) {
		char ch = word.at(i);

		if ((int)ch < 97) {
			word.at(i) = ( (int)ch + 32 );
		}
	}
	return;
}




#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

class Card
{
public:
	enum rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, KING, QUEEN };

	enum suit { CLUBS, SPADES, DIAMONDS, HEARTS };

	//Overloading << operator so can send Card output to standard output
	friend ostream& operator<<(ostream& os, Card& aCard)
	{
		const string RANKS[] = { "0","A","2","3","4","5","6","7","8","9","10","J","Q","K" };
		const string SUITS[] = { "c","d","h","s" };

		if (aCard.m_IsFaceUp)
		{
			os << RANKS[aCard.m_Rank] << SUITS[aCard.m_Suit];
		}

		else
		{
			os << "XX";

		}
		return os;
	}


	Card(rank r = ACE, suit s = SPADES, bool ifu = true);

	//return the value of a card, 1 - 11
	int GetValue() const;

	//flips a card; if face up, becomes facedown and vice versa
	void Flip();

private:
	rank m_Rank;
	suit m_Suit;
	bool m_IsFaceUp;
};

class Hand
{
public:
	Hand();

	virtual ~Hand();

	//adds a card to the Hand
	void Add(Card* pCard);

	//clears Hand
	void Clear();

	//gets total hand value. treats aces as 11 or 1
	int GetTotal() const;

protected:
	vector<Card*> m_Cards;
};

class GenericPlayer : public Hand
{
	friend ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer)
	{
		os << aGenericPlayer.m_Name << ":\t";

		vector<Card*>::const_iterator pCard;
		if (!aGenericPlayer.m_Cards.empty()) {
			for (pCard = aGenericPlayer.m_Cards.begin();
				pCard != aGenericPlayer.m_Cards.end();
				++pCard)
			{
				os << *(*pCard) << "\t";
			}

			if (aGenericPlayer.GetTotal() != 0) {
				cout << "(" << aGenericPlayer.GetTotal() << ")";
			}
		}
		else {
			os << "<empty>";
		}
		return os;
	}

public:
	GenericPlayer(const string& name = " ");

	virtual ~GenericPlayer();

	//indicates whether or not the Player wants to hit.
	virtual bool IsHitting() const = 0;

	//returns whether the player has busted
	bool IsBusted() const;

	//announces that the player has lost
	void Bust() const;

protected:
	string m_Name;
};

class Player : public GenericPlayer
{
public:
	Player(const string& name = " ");

	virtual ~Player();

	//returns whether or not the Player wants another hit
	virtual bool IsHitting() const;

	//Announces that the Player has won
	void Win() const;

	//Announces that the Player has lost
	void Lose() const;

	//Announces that the Player is Pushing
	void Push() const;
};

class House : public GenericPlayer
{
public:
	House(const string& name = "Mr. House");

	virtual ~House();

	//indicates whether or not the house is hitting. The House will always hit if its total is equal to or less then 16.
	virtual bool IsHitting() const;

	//flip over the first card
	void FlipFirstCard();
};

class Deck : public Hand
{
public:
	Deck();

	virtual ~Deck();

	//Create a Standard Deck Of 52 Cards
	void Populate();

	//Shuffle the Deck
	void Shuffle();

	//deal one Card to a hand
	void Deal(Hand& aHand);

	//give Additional Cards to a generic player
	void AdditionalCards(GenericPlayer& aGenericPlayer);

};

class Game
{
public:
	Game(const vector<string>& names);

	~Game();

	//plays the game of Blackjack
	void Play();

private:
	Deck m_Deck;
	House m_House;
	vector<Player> m_Players;
};
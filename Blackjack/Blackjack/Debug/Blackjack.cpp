// Blackjack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//Blackjack
//Plays a Simple Blackjack game. 1 - 7 Players

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
	friend ostream& operator<<(ostream& os, Card& aCard);

	Card(rank r = ACE, suit s = SPADES, bool ifu = true);

	//return the value of a card, 1 - 11
	int GetValue() const;

	//flips a card; if face up, becomes facedown and vice versa
	void Flip();

protected:
	rank m_Rank;
	suit m_Suit;
	bool m_IsFaceUp;
};

Card::Card(rank r, suit s, bool ifu) : m_Rank(r), m_Suit(s), m_IsFaceUp(ifu) {}

int Card::GetValue() const
{
	//if a card is face down, its value is 0
	int value = 0;
	if (m_IsFaceUp)
	{
		//value is number showing on card
		value = m_Rank;
		//value is 10 for face cards
		if (value > 10)
		{
			value = 10;
		}
	}

	return value;
}

void Card::Flip()
{
	m_IsFaceUp = !(m_IsFaceUp);
}

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

Hand::Hand()
{
	Clear();
}

Hand::~Hand()
{
	Clear();
}

void Hand::Add(Card* pCard)
{
	m_Cards.push_back(pCard);
}

void Hand::Clear()
{
	//iterate through vector, freeing all memory on the heap
	vector<Card*>::iterator iter = m_Cards.begin();
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		delete* iter;
		*iter = 0;
	}
	//clear vector of pointers
	m_Cards.clear();
}

int Hand::GetTotal() const
{
	//if no cards in hand, return 0
	if (m_Cards.empty())
	{
		return 0;
	}

	//if the first card has a value of 0, then the card is face down, return 0
	if (m_Cards[0]->GetValue() == 0)
	{
		return 0;
	}

	//add up all the card values treat each Ace as a 1
	int total = 0;
	vector<Card*>::const_iterator iter;
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		total += (*iter)->GetValue();
	}

	//determine if the hand contains an Ace
	bool containsAce = false;
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		if ((*iter)->GetValue() == Card::ACE)
		{
			containsAce = true;
		}
	}

	//if hand contains ace and the value is low enough, treat the ace as an 11
	if (containsAce && total <= 11)
	{
		//add only 10 because Ace already counts as 1
		total += 10;
	}

	return total;
}

class GenericPlayer : public Hand
{
	friend ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

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

GenericPlayer::GenericPlayer(const string& name):
	m_Name(name)

{}

GenericPlayer::~GenericPlayer()
{}

bool GenericPlayer::IsBusted() const
{
	return (GetTotal() > 21);
}

void GenericPlayer::Bust() const
{
	cout << m_Name << " busts!\n";
}

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

Player::Player(const string& name):
	GenericPlayer(name)
{}

Player::~Player()
{}

bool Player::IsHitting() const
{
	cout << m_Name << ", Would you like another card? (y/n): ";
	char response;
	cin >> response;
	return (response == 'y' || response == 'Y');
}

void Player::Win() const
{
	cout << m_Name << " has won!\n";
}

void Player::Lose() const
{
	cout << m_Name << " has lost!\n";
}

void Player::Push() const
{
	cout << m_Name << " is pushing.\n";
}

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

House::House(const string& name):
	GenericPlayer(name)
{}

House::~House()
{}

bool House::IsHitting() const
{
	return(GetTotal() <= 16);
}

void House::FlipFirstCard()
{
	if (!(m_Cards.empty()))
	{
		m_Cards[0]->Flip();
	}
	else
	{
		cout << "No Cards To Flip!\n";
	}
}

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

Deck::Deck()
{
	m_Cards.reserve(52);
	Populate();
}

Deck:: ~Deck()
{}

void Deck::Populate()
{
	Clear();
	//create standard deck
	for (int s = Card::CLUBS; s <= Card::SPADES; ++s)
	{
		for (int r = Card::ACE; r <= Card::KING; ++r)
		{
			Add(new Card(static_cast<Card::rank>(r), static_cast<Card::suit>(s)));
		}
	}
}

void Deck::Shuffle()
{
	random_shuffle(m_Cards.begin(), m_Cards.end());
}

void Deck::Deal(Hand& aHand)
{
	if (!m_Cards.empty())
	{
		aHand.Add(m_Cards.back());
		m_Cards.pop_back();
	}
	else
	{
		cout << "Out Of Cards. Unable to Deal New Cards.";
	}
}

void Deck::AdditionalCards(GenericPlayer& aGenericPlayer)
{
	cout << endl;
	//continue to deal a card as long as generic player is not busted and
	//wants another hit
	while (!(aGenericPlayer.IsBusted()) && aGenericPlayer.IsHitting())
	{
		Deal(aGenericPlayer);
		cout << aGenericPlayer << endl;

		if (aGenericPlayer.IsBusted())
		{
			aGenericPlayer.Bust();
		}
	}
}

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

Game::Game(const vector<string>& names)
{
	//create a vector of players from a vector of names
	vector<string>::const_iterator pName;
	for (pName = names.begin(); pName != names.end(); ++pName)
	{
		m_Players.push_back(Player(*pName));
	}

	//seed the random number generator
	srand(static_cast<unsigned int>(time(0)));
	m_Deck.Populate();
	m_Deck.Shuffle();
}

Game::~Game()
{}

void Game::Play()
{
	//initially deal two cards to every player
	vector<Player>::iterator pPlayer;
	for (int i = 0; i < 2; ++i)
	{
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
		{
			m_Deck.Deal(*pPlayer);
		}
		m_Deck.Deal(m_House);
	}

	//hide house's first card
	m_House.FlipFirstCard();

	//display everyone's hand
	for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
	{
		cout << *pPlayer << endl;
	}

	cout << m_House << endl;

	//deal additional cards to players
	for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
	{
		m_Deck.AdditionalCards(*pPlayer);
	}

	//reveal house's first card
	m_House.FlipFirstCard();
	cout << endl << m_House;

	//deal additional cards to house
	m_Deck.AdditionalCards(m_House);

	if (m_House.IsBusted())
	{
		//everyone still playing wins
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
		{
			if (!(pPlayer->IsBusted()))
			{
				pPlayer->Win();
			}
		}
	}
	else
	{
		//compare each player still playing to the house
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
		{
			if (!(pPlayer->IsBusted()))
			{
				if (pPlayer->GetTotal() > m_House.GetTotal())
				{
					pPlayer->Win();
				}
				else if (pPlayer->GetTotal() < m_House.GetTotal())
				{
					pPlayer->Lose();
				}
				else
				{
					pPlayer->Push();
				}
			}
		}
	}

	//remove everyone's cards
	for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
	{
		pPlayer->Clear();
	}
	m_House.Clear();
}

//function prototypes
ostream& operator<<(ostream os, const Card& aCard);
ostream& operator<<(ostream os, const GenericPlayer& aGenericPlayer);

int main()
{
	cout << "\n\n---Welcome To Lucky 38 Blackjack!===\n\n";

	int numPlayers = 0;
	while (numPlayers < 1 || numPlayers > 7)
	{
		cout << "How many chum- players are there? 1 - 7: ";
		cin >> numPlayers;
	}

	vector<string>names;
	string name;
	for (int i = 0; i < numPlayers; ++i)
	{
		cout << "Enter Player Name: ";
		cin >> name;
		names.push_back(name);
	}

	cout << endl;

	//core game loop
	Game aGame(names);
	char again = 'y';
	while (again != 'n' && again != 'N')
	{
		aGame.Play();
		cout << "\nDo you want to lose more mone- play again?: (Y/N)";
		cin >> again;
	}

	return 0;
}

//overloads << operator so Card can be sent to cout
ostream& operator<< (ostream& os, const Card& aCard)
{
	const string RANKS[] = { "0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
	const string SUITS[] = { "c", "s", "h", "s" };

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
//overloads << operator so a GenericPlayer object can be sent to cout
ostream& operator<< (ostream& os, const GenericPlayer& aGenericPlayer)
{
	os << aGenericPlayer.m_Name << ":\t";

	vector<Card*>::const_iterator pCard;

	if (!aGenericPlayer.m_Cards.empty())
	{
		for (pCard = aGenericPlayer.m_Cards.begin(); pCard != aGenericPlayer.m_Cards.end(); ++pCard)
		{
			os << *(*pCard) << "\t";
		}

		if (aGenericPlayer.GetTotal() != 0)
		{
			cout << "(" << aGenericPlayer.GetTotal() << ")";
		}
	}
	else
	{
		os << "<empty>";
	}
	return os;
}



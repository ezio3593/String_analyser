#include "stdafx.h"
#include <cctype>
#include "StateMachine.h"

//StateMachine

template <class C, class S> bool StateMachine<C, S>::readChar(char c, unsigned int pos, bool isLast)
{
	typedef std::list<Tr>::const_iterator const_iter;

	if (isEndParse) isEndParse = false;

	bool find = false;
	bool isEnd = false;
	bool isRead = false;
	const std::list<Tr>* stateTransitions;
	IS_CORRECT_SYMBOL func;

	Trans trans = transitions->at(currentState);

	stateTransitions = trans.getTransList();
	isEnd = trans.isFinalState();

	if (isEnd)
	{
		endPos = pos;
		isSetEndPos = true;
	}

	if (stateTransitions == NULL && isEnd) 
	{
		isEndParse = true;
		return isRead;
	}
		
	for (const_iter it = stateTransitions->cbegin(); it != stateTransitions->cend(); ++it)
	{
		func = it->getFunc();

		if ( (obj->*func)(c) ) 
		{ 
			find = true;
			currentState = it->getNextState();
			isRead = true;
			break;
		}
	}

	if (!find) 
		isEndParse = true;
	else if (isLast)
	{
		isEndParse = true;
		trans = transitions->at(currentState);
		isEnd = trans.isFinalState();
		if (!isEnd) 
			isRead = false;
		else 
		{
			endPos = pos;
			isSetEndPos = true;
		}
	}

	return isRead;
}

template <class C, class S> StateMachine<C, S>::~StateMachine()
{
	transitions->clear();
	delete transitions;
}

//NumberState

NumberState::NumberState()
{
	type = "Number";
	count = 0;
	sign = '-';
	separators = ".,";
	startPos = 0;


	transitions = new std::map<state, Trans>();

	std::list<Tr>* initTransitions = new std::list<Tr>();
	std::list<Tr>* gotSignTransitions = new std::list<Tr>();
	std::list<Tr>* gotNumBeforeSepTransitions = new std::list<Tr>();
	std::list<Tr>* gotSepTransitions = new std::list<Tr>();
	std::list<Tr>* gotNumAfterSepTransitions = new std::list<Tr>();

	initTransitions->push_back(Tr(&NumberState::isSign, GOT_SIGN));
	initTransitions->push_back(Tr(&NumberState::isDigit, GOT_NUM_BEFORE_SEP));

	gotSignTransitions->push_back(Tr(&NumberState::isDigit, GOT_NUM_BEFORE_SEP));

	gotNumBeforeSepTransitions->push_back(Tr(&NumberState::isDigit, GOT_NUM_BEFORE_SEP));
	gotNumBeforeSepTransitions->push_back(Tr(&NumberState::isSeparator, GOT_SEP));

	gotSepTransitions->push_back(Tr(&NumberState::isDigit, GOT_NUM_AFTER_SEP));

	gotNumAfterSepTransitions->push_back(Tr(&NumberState::isDigit, GOT_NUM_AFTER_SEP));
	
	transitions->insert(std::pair<state, Trans> (INIT, Trans(initTransitions, false)));
	transitions->insert(std::pair<state, Trans> (GOT_SIGN, Trans(gotSignTransitions, false)));
	transitions->insert(std::pair<state, Trans> (GOT_NUM_BEFORE_SEP, Trans(gotNumBeforeSepTransitions, true)));
	transitions->insert(std::pair<state, Trans> (GOT_SEP, Trans(gotSepTransitions, false)));
	transitions->insert(std::pair<state, Trans> (GOT_NUM_AFTER_SEP, Trans(gotNumAfterSepTransitions, true)));

	stateMachine = new StateMachine<NumberState, state>(transitions, this, INIT);
}

bool NumberState::isCorrectInputChar(char c)
{
	return (isDigit(c) || isSign(c)) ? true : false;
}

bool NumberState::isDigit(char c)
{
	return isdigit(c);
}

bool NumberState::isSign(char c)
{
	if (c == sign) 
		return true;
	else return false;
}

bool NumberState::isSeparator(char c)
{
	for (unsigned int i = 0; i < separators.length(); ++i)
		if (c == separators[i]) return true;
	return false;
}

unsigned int NumberState::readChar(char c, unsigned int pos, bool isLast)
{
	bool isRead = stateMachine->readChar(c, pos, isLast);
	if (stateMachine->isEnd()) 
	{
		pos = stateMachine->getPos();
		bool isSetEnd = stateMachine->isSetEndPosition();
		if (isSetEnd && pos - startPos >= 0) ++count;
		if (!pos) return startPos;
	} 
	return isRead ? ++pos : pos;
}

NumberState::~NumberState()
{
	transitions->clear();
	delete transitions;
}

//WordState


WordState::WordState()
{
	type = "Word";

	count = 0;
	startPos = 0;

	transitions = new std::map<state, Trans>();

	std::list<Tr>* initTransitions = new std::list<Tr>();
	std::list<Tr>* gotFirstCharTransitions = new std::list<Tr>();
	std::list<Tr>* gotCharTransitions = new std::list<Tr>();

	initTransitions->push_back(Tr(&WordState::isWordChar, GOT_FIRST_CHAR));

	gotFirstCharTransitions->push_back(Tr(&WordState::isWordChar, GOT_CHAR));

	gotCharTransitions->push_back(Tr(&WordState::isWordChar, GOT_CHAR));

	transitions->insert(std::pair<state, Trans> (INIT, Trans(initTransitions, false)));
	transitions->insert(std::pair<state, Trans> (GOT_FIRST_CHAR, Trans(gotFirstCharTransitions, false)));
	transitions->insert(std::pair<state, Trans> (GOT_CHAR, Trans(gotFirstCharTransitions, true)));

	stateMachine = new StateMachine<WordState, state>(transitions, this, INIT);
}

bool WordState::isWordChar(char c)
{
	if ('a' <= c && 'z' >= c || 'A' <= c && 'Z' >= c) 
		return true;
	else return false;
}

unsigned int WordState::readChar(char c, unsigned int pos, bool isLast)
{
	bool isRead = stateMachine->readChar(c, pos, isLast);
	if (stateMachine->isEnd()) 
	{
		pos = stateMachine->getPos();
		if (pos - startPos > 0) ++count;
		if (!pos) return startPos;
	} 
	return isRead ? ++pos : pos;;
}

WordState::~WordState()
{
	transitions->clear();
	delete transitions;
}

// Symbolstate

SymbolState::SymbolState()
{
	type = "Symbol";

	count = 0;
	startPos = 0;

	transitions = new std::map<state, Trans>();

	std::list<Tr>* initTransitions = new std::list<Tr>();

	initTransitions->push_back(Tr(&SymbolState::isSymbol, GOT_TOKEN));

	transitions->insert(std::pair<state, Trans> (INIT, Trans(initTransitions, false)));
	transitions->insert(std::pair<state, Trans> (GOT_TOKEN, Trans(NULL, true)));

	stateMachine = new StateMachine<SymbolState, state>(transitions, this, INIT);
}

bool SymbolState::isSymbol(char c)
{
	if (isalpha(c) || !isdigit(c) && !isspace(c)) 
		return true;
	else return false;
}

unsigned int SymbolState::readChar(char c, unsigned int pos, bool isLast)
{
	bool isRead = stateMachine->readChar(c, pos, isLast);
	if (stateMachine->isEnd()) 
	{
		pos = stateMachine->getPos();
		if (pos - startPos >= 0) ++count;
		if (!pos) return startPos;
	}
	return isRead ? ++pos : pos;
}

SymbolState::~SymbolState()
{
	transitions->clear();
	delete transitions;
}

// SpaceState

SpaceState::SpaceState()
{
	type = "Space";

	count = 0;
	startPos = 0;

	transitions = new std::map<state, Trans>();

	std::list<Tr>* initTransitions = new std::list<Tr>();

	initTransitions->push_back(Tr(&SpaceState::isSpace, GOT_TOKEN));

	transitions->insert(std::pair<state, Trans > (INIT, Trans(initTransitions, false)));
	transitions->insert(std::pair<state, Trans > (GOT_TOKEN, Trans(NULL, true)));

	stateMachine = new StateMachine<SpaceState, state>(transitions, this, INIT);
}

bool SpaceState::isSpace(char c)
{
	if (c == ' ') 
		return true;
	else return false;
}

unsigned int SpaceState::readChar(char c, unsigned int pos, bool isLast)
{
	bool isRead = stateMachine->readChar(c, pos, isLast);
	if (stateMachine->isEnd()) 
	{
		pos = stateMachine->getPos();
		if (pos - startPos >= 0) ++count;
		if (!pos) return startPos;
	}
	return isRead ? ++pos : pos;
}

SpaceState::~SpaceState()
{
	transitions->clear();
	delete transitions;
}
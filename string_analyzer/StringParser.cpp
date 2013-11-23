#include "stdafx.h"
#include "StringParser.h"

// StateManager

StateManager::StateManager()
{
	currentStateIndex = 0;
	stat = new std::map<std::string, int>();
}

State* StateManager::getState(char c)
{
	int index = -1;

	for (unsigned int i = 0; i < statesVector.size(); ++i)
	{
		if (statesVector[i]->isCorrectInputChar(c)) 
		{
			if (i == currentStateIndex && !statesVector[i]->isGood())
				continue;
			else 
			{
				index = i;
				break;
			}
		}
	}

	if (index == -1)
		return NULL;
	else 
	{
		currentStateIndex = index;
		return statesVector[currentStateIndex];
	}
}

std::map<std::string, int>* StateManager::getStat()
{
	for (unsigned int i = 0; i < statesVector.size(); ++i)
		stat->insert(std::pair<std::string, int>(statesVector[i]->getTypeName(), statesVector[i]->getCount()));

	return stat;
}

StateManager::~StateManager()
{
	statesVector.clear();
	stat->clear();
	delete stat;
}

//StringParser

void StringParser::Parse(const std::string* s) {
	
	if (s == NULL) return;
	State* currectState = NULL;
	char c = 0;
	unsigned int pos = 0;

	unsigned int len = s->length();

	while (pos < len)
	{
		c = (*s)[pos];

		if (currectState == NULL || currectState->isEnd())
		{
			currectState = stateManager.getState(c);
			if (currectState == NULL) 
			{
				++pos;
				continue;
			}
			currectState->setStartPos(pos);
			currectState->reset();
		}

		if (pos != len - 1) 
			pos = currectState->readChar(c, pos, false);
		else pos = currectState->readChar(c, pos, true);
	}
}
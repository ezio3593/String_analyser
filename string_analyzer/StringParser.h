#include "StateMachine.h"
#include <vector>

class StateManager
{
	std::vector<State*> statesVector;
	std::map<std::string, int>* stat;

	int currentStateIndex;

	public:
		StateManager();
		void addState(State* state){statesVector.push_back(state);}
		State* getState(char c);
		std::map<std::string, int>* getStat();
		~StateManager();
};

class StringParser
{
	StateManager stateManager;

	public:
		StringParser() {};
		void addState(State* state) { stateManager.addState(state); }
		void Parse(const std::string* s);
		std::map<std::string, int>* getStat() {return stateManager.getStat();}
};
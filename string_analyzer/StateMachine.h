#include <string>
#include <vector>
#include <map>
#include <list>
#include <utility>

class State
{
	protected:
		std::string type;
		int count;
		int startPos;
		bool isGoodParse;
	public:
		virtual bool isEnd() = 0;
		virtual unsigned int readChar(char, unsigned int, bool) = 0;
		virtual void reset() = 0;
		virtual bool isGood() {return isGoodParse;}
		virtual bool isCorrectInputChar(char) = 0;
		virtual void setStartPos(unsigned int pos) {startPos = pos;}
		virtual std::string getTypeName() {return type;}
		virtual int getCount() {return count;}
		virtual ~State() {}
};

template<class C, class S> class Transition
{
	typedef bool (C::* IS_CORRECT_SYMBOL)(char);
	typedef S state;

	IS_CORRECT_SYMBOL func;
	state nextState;

	public:
		Transition(IS_CORRECT_SYMBOL _func, S _nextState): func(_func), nextState(_nextState) {}
		S getNextState() const {return nextState;}
		IS_CORRECT_SYMBOL getFunc() const {return func;}
};

template<class C, class S> class Transitions
{
	const std::list<Transition<C, S>> *transList;
	bool isEndState;

	public:
		Transitions(const std::list<Transition<C, S>> *_transList, bool _isEndState): transList(_transList), isEndState(_isEndState) {}
		const std::list<Transition<C, S>>* getTransList() {return transList;}
		bool isFinalState() {return isEndState;}
		~Transitions() {};
};

template<class C, class S> class StateMachine
{
	typedef S state;
	typedef bool (C::* IS_CORRECT_SYMBOL)(char);
	typedef Transition<C, S> Tr;
	typedef Transitions<C, S> Trans;

	state currentState;
	state init;
	
	const std::map<S, Trans>* transitions;
	C *const obj;
	bool isEndParse;
	bool isSetEndPos;
	unsigned int endPos;

	public:
		StateMachine(const std::map<S, Trans>* _transitions,  C *const _obj, S _init) : 
			transitions(_transitions), obj(_obj), init(_init)
		{ currentState = init; isEndParse = false; endPos = 0; isSetEndPos = false;}

		bool readChar(char c, unsigned int pos, bool isLast);
		bool isEnd() {return isEndParse;}
		bool isSetEndPosition() {return isSetEndPos;}
		unsigned int getPos() {return endPos;}
		void reset() {endPos = 0; isEndParse = false; currentState = init; isSetEndPos = false;}
		~StateMachine();
};

class NumberState: public State
{
	static enum state
	{
		INIT,
		GOT_NUM_BEFORE_SEP,
		GOT_NUM_AFTER_SEP,
		GOT_SIGN,
		GOT_SEP,
	};

	typedef Transition<NumberState, state> Tr;
	typedef Transitions<NumberState, state> Trans;

	char sign;
	std::string separators;
	StateMachine<NumberState, state> *stateMachine;
	std::map <state, Trans> *transitions;

	bool isSeparator(char c);
	bool isSign(char c);
	bool isDigit(char c);

	public:
		NumberState();
		unsigned int readChar(char c, unsigned int pos, bool isLast);
		bool isCorrectInputChar(char c);
		void reset() {stateMachine->reset(); isGoodParse = true;}
		bool isEnd() {return stateMachine->isEnd();}
		~NumberState();
};

class WordState: public State
{
	static enum state
	{
		INIT,
		GOT_FIRST_CHAR,
		GOT_CHAR,
	};

	typedef Transition<WordState, state> Tr;
	typedef Transitions<WordState, state> Trans;

	StateMachine<WordState, state> *stateMachine;
	std::map <state, Trans> *transitions;

	bool isWordChar(char c);

	public:
		WordState();
		unsigned int readChar(char c, unsigned int pos, bool isLast);
		bool isCorrectInputChar(char c) {return isWordChar(c);}
		bool isEnd() {return stateMachine->isEnd();}
		void reset() {stateMachine->reset(); isGoodParse = true;}
 		~WordState();
};

class SymbolState: public State
{
	static enum state
	{
		INIT,
		GOT_TOKEN
	};

	typedef Transition<SymbolState, state> Tr;
	typedef Transitions<SymbolState, state> Trans;
	int i;
	StateMachine<SymbolState, state> *stateMachine;
	std::map <state, Trans> * transitions;

	bool isSymbol(char c);
	 
	public:
		SymbolState();
		unsigned int readChar(char c, unsigned int pos, bool isLast);
		bool isCorrectInputChar(char c) {return isSymbol(c);}
		bool isEnd() {return stateMachine->isEnd();}
		void reset() {stateMachine->reset(); isGoodParse = true;}
		~SymbolState();
};

class SpaceState: public State
{
	static enum state
	{
		INIT,
		GOT_TOKEN
	};

	typedef Transition<SpaceState, state> Tr;
	typedef Transitions<SpaceState, state> Trans;

	StateMachine<SpaceState, state> *stateMachine;
	std::map <state, Trans> * transitions;

	bool isSpace(char c);
	 
	public:
		SpaceState();
		unsigned int readChar(char c, unsigned int pos, bool isLast);
		bool isCorrectInputChar(char c) {return isSpace(c);}
		bool isEnd() {return stateMachine->isEnd();}
		void reset() {stateMachine->reset(); isGoodParse = true;}
		~SpaceState();
};
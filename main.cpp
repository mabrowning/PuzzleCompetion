#include <vector>

#include <csignal>
#include <cstring>
#ifndef _MSC_VER
#include <unistd.h>
#else
#define SIGUSR1 SIGINT
#endif

#include "sliding-puzzle.h"

//#include "astar-solve.h"
#include "idastar-solve.h"
#include "rbfs-solve.h"

namespace
{
	  volatile std::sig_atomic_t gSignalStatus;
}

bool* gPrintStatus = nullptr;
 
void signal_handler(int signal)
{
	if( gPrintStatus ) *gPrintStatus = true;
}

template<typename State>
State GetRandomInitialState( State state, int max = 50  )
{
	state.init();
	typename State::Action lastAction; //default
	for( int i = 0 ; i < max; ++i )
	{
		auto actions = state.AvailableActions( lastAction );
		auto paction = actions[0];
		while( ( paction = actions[rand()%actions.size()] ) == nullptr );

		state = state.Apply( *paction );
		lastAction = *paction;
	}

	return state;
}


int main(int argc, char** argv)
{

	std::signal(SIGUSR1, signal_handler);

	typedef SlidingPuzzleState<4, 4> State_t;

	State_t initial;

	//Solve that puzzle

	bool  ast = false;
	bool rbfs = false;
	bool rand = false;

	int i = 1;
	while (i < argc)
	{
		char * arg = argv[i++];

		ast |= (strcmp(arg, "--astar") == 0);
		rbfs |= (strcmp(arg, "--rbfs") == 0);
		rand |= (strcmp(arg, "--rand") == 0);
	}

	if (rand)
	{
		std::cerr << "Generating random initial state..." << std::endl;
		initial = GetRandomInitialState(initial, 100);
	}
	else
	{
		std::cerr << "Enter initial state:" << std::endl;
		std::cin >> initial;
	}


	std::cerr << "Solving...\n" << initial << std::endl;

	std::vector< State_t::Action > Solution;
	if( ast )
	{
		/*
		auto Solver = AStar<State_t>{};
		gPrintStatus = &Solver.PrintStatus;
		Solution = Solver.Solve( initial );
		*/

		std::cerr << "A* disabled!" << std::endl;
		return -1;
	}
	else if( rbfs ) 
	{
		std::cerr << "Using RBFS" << std::endl;
		auto Solver = RBFS<State_t>{};
		gPrintStatus = &Solver.PrintStatus;
		Solution = Solver.Solve( initial );
	}
	else
	{
		//Default
		std::cerr << "Using IDA*" << std::endl;
		auto Solver = IDAStar<State_t >{};
		gPrintStatus = &Solver.PrintStatus;
		Solution = Solver.Solve( initial );
	}


	auto state = initial;
	std::cout << state;
	for( auto action : Solution )
	{
		state = state.Apply( action );
		std::cout << action << "->\n" << state;
	}

	std::cerr << initial << "Took " << Solution.size() << " steps" << std::endl;
#ifdef _MSC_VER
	//Keep window open
	std::cin.get();
#endif
	return 0;
}


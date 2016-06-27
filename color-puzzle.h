#pragma once
#ifndef COLOR_PUZZLE_H
#define COLOR_PUZZLE_H
#include <array>
#include <vector>
#include <iostream>
#include <iomanip>

#include "hash.h"
	

struct ColorPuzzleAction
{
	enum ColorChoice
	{
		COLOR_RED,
		COLOR_GREEN,
		COLOR_BLUE,
		COLOR_YELLOW,
		COLOR_WHITE,

		NUM_ColorChoice
		
	};

	constexpr static std::size_t MaxBranch   = NUM_ColorChoice-1; //maximum out-degree of state graph
	constexpr static std::size_t MaxInBranch = NUM_ColorChoice-1; //maximum in-degree of state graph. This puzzle is reversible, so symmetric, so in=out

	typedef std::array< ColorPuzzleAction*, MaxBranch > Actions;

	int GetCost() const { return 1; }

	ColorChoice color;
	ColorPuzzleAction( ColorChoice d = NUM_ColorChoice ) : color(d){}
	bool operator==( const ColorPuzzleAction & o ) const { return color == o.color; };
	bool operator!=( const ColorPuzzleAction & o ) const { return color != o.color; };

	static ColorPuzzleAction RED    ;
	static ColorPuzzleAction BLUE  ;
	static ColorPuzzleAction GREEN  ;
	static ColorPuzzleAction YELLOW ;
	static ColorPuzzleAction WHITE ;
	static ColorPuzzleAction NONE ;
};

ColorPuzzleAction ColorPuzzleAction::RED    = COLOR_RED;
ColorPuzzleAction ColorPuzzleAction::BLUE   = COLOR_BLUE;
ColorPuzzleAction ColorPuzzleAction::GREEN  = COLOR_GREEN;
ColorPuzzleAction ColorPuzzleAction::YELLOW = COLOR_YELLOW;
ColorPuzzleAction ColorPuzzleAction::WHITE  = COLOR_WHITE;
ColorPuzzleAction ColorPuzzleAction::NONE   = NUM_ColorChoice;

std::ostream & operator<<(std::ostream &os, const ColorPuzzleAction & a )
{
	switch( a.color )
	{
		case ColorPuzzleAction::COLOR_RED:
			os << "RED";
			break;
		case ColorPuzzleAction::COLOR_BLUE:
			os << "BLUE";
			break;
		case ColorPuzzleAction::COLOR_GREEN:
			os << "GREEN";
			break;
		case ColorPuzzleAction::COLOR_YELLOW:
			os << "YELLOW";
			break;
		default:
			break;
	}
	return os;
}

std::istream & operator>>(std::istream &is, ColorPuzzleAction & a )
{
	std::string color ;
	is >> color;

	     if( color == "RED" )    a.color = ColorPuzzleAction::COLOR_RED;
	else if( color == "BLUE" )   a.color = ColorPuzzleAction::COLOR_BLUE;
	else if( color == "GREEN" )  a.color = ColorPuzzleAction::COLOR_GREEN;
	else if( color == "YELLOW" ) a.color = ColorPuzzleAction::COLOR_YELLOW;

	return is;
}

//For building initial graph
struct Node
{
	int x, y;
	ColorPuzzleAction color;
	std::vector< Node* > adj;
};

struct ColorPuzzle
{
	std::vector< std::vector< Node > > cells;

};

struct ColorPuzzleState
{
	typedef ColorPuzzleAction Action;

	typedef unsigned char index_t;
	//API for AStarSolve
	Action::Actions AvailableActions( Action PrevAction = Action{}  ) const
	{
		Action::Actions ret = { nullptr, nullptr, nullptr, nullptr };

		return ret;
	}


	ColorPuzzleState Apply( const Action & a ) const
	{
		//Use state-applying constructor
		return ColorPuzzleState( *this, a.color );
	}

	//Implementation details

	ColorPuzzleState( )
	{
	}

	int EstGoalDist() const { return GoalDist; };

	bool IsGoal() const 
	{
		return GoalDist == 0;
	}

	private:
	int DoEstGoalDist() const 
	{
		return 0;
	}


	int GoalDist;// = DoEstGoalDist();

	ColorPuzzleState( const ColorPuzzleState & o, Action::ColorChoice color )
		: arr ( o.arr )
		, GoalDist( o.GoalDist )
	{
	}
};

bool operator==( const ColorPuzzleState & lhs, const ColorPuzzleState & rhs )
{
	return lhs.arr == rhs.arr;
}

namespace std
{
	struct hash< ColorPuzzleState >
	{
		size_t operator() ( const ColorPuzzleState & state ) const
		{
			return hash< typename ColorPuzzleState::arr_t >()( state.arr ) ;
		}
	};
}

std::ostream & operator<<(std::ostream &os, const ColorPuzzleState & t )
{
	for( int i = 0; i < N; ++ i )
	{
		for( int j = 0; j < N; ++j )
			os << std::setw(6) << t.arr[j][i] << " ";
		os << "\n";
	}
	os << std::endl;

	return os;
}

std::istream & operator>>(std::istream &is, ColorPuzzleState & t )
{
	int size;
	is >> size;

	std::vector< std::vector< Node > > nodes;
	for
			is >> val;

	return is;
}

#endif

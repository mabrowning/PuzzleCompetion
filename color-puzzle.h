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

	     if( color == "RED" ) a.color = ColorPuzzleAction::COLOR_RED;
	else if( color == "BLUE" ) a.color = ColorPuzzleAction::COLOR_BLUE;
	else if( color == "GREEN" ) a.color = ColorPuzzleAction::COLOR_GREEN;
	else if( color == "YELLOW" ) a.color = ColorPuzzleAction::COLOR_YELLOW;

	return is;
}


//N rows by N columns
template< unsigned int N>
struct ColorPuzzleState
{
	typedef ColorPuzzleAction Action;

	typedef unsigned char index_t;
	//API for AStarSolve
	Action::Actions AvailableActions( Action PrevAction = Action{}  ) const
	{
		Action::Actions ret = { nullptr, nullptr, nullptr, nullptr };

		//TODO: find any adjacent colors
		auto act = ret.data();
		if( !BLUE.empty() ) *(act++) = &Action::BLUE;
		if( !RED.empty() ) *(act++) = &Action::RED;
		if( !YELLOW.empty() ) *(act++) = &Action::YELLOW ;
		if( !GREEN.empty() ) *(act++) = &Action::GREEN;

		return ret;
	}


	ColorPuzzleState Apply( const Action & a ) const
	{
		//Use state-applying constructor
		return ColorPuzzleState( *this, a.color );
	}

	//Implementation details
	
	//N columns of N rows
	typedef std::array< std::array< Action, N>, N > arr_t;
	arr_t arr;

	typedef std::vector< std::pair< char,char > > free_t;
	free_t RED;
	free_t BLUE;
	free_t GREEN;
	free_t YELLOW;

	free_t none;


	ColorPuzzleState( )
	{
	}

	free_t & GetFree( const Action & a )
	{

		switch( a.color )
		{
			case ColorPuzzleAction::COLOR_RED:
				return RED;
			case ColorPuzzleAction::COLOR_BLUE:
				return BLUE;
			case ColorPuzzleAction::COLOR_GREEN:
				return GREEN;
			case ColorPuzzleAction::COLOR_YELLOW:
				return YELLOW;
		}

		return none;
	}

	Action & GetColor( char n, char m )
	{
		if( n < 0 || m < 0 || n >= N || m >= N ) return Action::NONE;

		return arr[m][n];
	}

	void MarkSpace( char n, char m, Action & Color )
	{
		GetFree( Color ).push_back( std::pair<char,char>{ n, m } );

		auto & U = GetColor(n-1,m );
		if( Color == U || Color == Action::WHITE ) MarkSpace( n-1,m, Color );

		auto & D = GetColor(n+1,m );
		if( Color == D || Color == Action::WHITE ) MarkSpace( n+1,m, Color );

		auto & L = GetColor(n,m-1 );
		if( Color == L || Color == Action::WHITE ) MarkSpace( n,m-1, Color );

		auto & R = GetColor(n,m+1 );
		if( Color == R || Color == Action::WHITE ) MarkSpace( n,m+1, Color );

		arr[m][n] = Action::WHITE;
	}

	void MarkSpace( char n, char m )
	{
		MarkSpace( n,m, GetColor( n, m ) );

		GoalDist = RED.size() + BLUE.size() + GREEN.size() + YELLOW.size();
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
		free_t & f = GetFree( color );

		for( auto & nm : f )
			MarkSpace( nm.first, nm.second );

		f.clear();

		GoalDist = RED.size() + BLUE.size() + GREEN.size() + YELLOW.size();
	}
};

template<unsigned N> 
bool operator==( const ColorPuzzleState<N> & lhs, const ColorPuzzleState<N> & rhs )
{
	return lhs.arr == rhs.arr;
}

namespace std
{
	template<unsigned N> struct hash< ColorPuzzleState<N> >
	{
		size_t operator() ( const ColorPuzzleState<N> & state ) const
		{
			return hash< typename ColorPuzzleState<N>::arr_t >()( state.arr ) ;
		}
	};
}

template< unsigned int N >
std::ostream & operator<<(std::ostream &os, const ColorPuzzleState<N> & t )
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

template< unsigned int N>
std::istream & operator>>(std::istream &is, ColorPuzzleState<N> & t )
{

	for( auto & col : t.arr )
		for( auto & val : col )
			is >> val;

	return is;
}

#endif

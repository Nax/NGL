/* ************************************************************************** */
/*                                                                            */
/*                                                  &&&&&&       &&&&&&       */
/*    GameState.cpp                                &------&     &------&      */
/*                                                  &&-----&   &-----&&       */
/*                                                    &&&&#######&&&&         */
/*                                                       #.......#            */
/*                                                       #.....  #            */
/*    This file is part of the                           #...    #            */
/*    Lums library.                                       #######             */
/*                                                                            */
/* ************************************************************************** */

#include "GameState.h"
#include "Core.h"

using namespace lm;

Core&	GameState::Core() const
{
	return *_core;
}

void			GameState::Close()
{
	_deleteMark = true;
	_core->_deleteMark = true;
}
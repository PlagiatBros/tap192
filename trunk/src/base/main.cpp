/***************************************************************************
 *            main.cc
 *
 *  Mon Sep  4 10:12:59 2006
 *  Copyright  2006  Florent Berthaut
 *  florentberthaut@no-log.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "tapeutape.h"

using namespace std;

int main(int argc, char** argv)
{
	tapeutape tap(argc,argv);
	
	#ifdef WITH_GUI
		Fl::lock();
		return Fl::run();
	#else
		return EXIT_SUCCESS;
	#endif
}

/*
-----------------------------------------------------------------------------
This source file is part of labtd

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#ifndef POINT_H
#define POINT_H

namespace Apoc3D
{
	namespace Math
	{
		class Point
		{
		public:
			int X;
			int Y;




			Point(int x, int y) { X = x; Y = y; }

			friend static bool operator ==(Point a, Point b)
			{
				return (a.X  == b.X) && (b.X == b.Y);
			}
			friend static bool operator !=(Point a, Point b)
			{
				return (a.X != b.X) || (b.X != b.Y);
			}
			
		
			const static Point Zero;
		};

	}
}
#endif
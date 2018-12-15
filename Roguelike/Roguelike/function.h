#pragma once
#include <Siv3D.hpp>

//マップグリッドとXY座標の相互変換
Point GridToXY(Point);
Point GridToCenterXY(Point);
Point XYToGrid(Point);
Point GridToXY(int, int);
Point SubGridToXY(int, int);
Point GridToCenterXY(int, int);
Point XYToGrid(int, int);
Point getMouseGrid();

enum Direction
{
	Right, UpRight, Up, UpLeft, Left, DownLeft, Down, DownRight
};
void rotateDirection(Direction&, int);
Point DirectionToPoint(int);
Direction PointToDirection(Point);
double DirectionToRadian(int);
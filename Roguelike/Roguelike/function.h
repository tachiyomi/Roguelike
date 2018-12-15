#pragma once
#include <Siv3D.hpp>

//マップグリッドとXY座標の相互変換
Point GridtoXY(Point);
Point GridtoCenterXY(Point);
Point XYtoGrid(Point);
Point GridtoXY(int, int);
Point SubGridtoXY(int, int);
Point GridtoCenterXY(int, int);
Point XYtoGrid(int, int);
Point getMouseGrid();

enum Direction
{
	Right, UpRight, Up, UpLeft, Left, DownLeft, Down, DownRight
};
void rotateDirection(Direction&, int);
Point DirectionToPoint(int);
Direction PointToDirection(Point);
double ToRadian(int);
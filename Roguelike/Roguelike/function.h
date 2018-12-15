#pragma once
#include <Siv3D.hpp>

//�}�b�v�O���b�h��XY���W�̑��ݕϊ�
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
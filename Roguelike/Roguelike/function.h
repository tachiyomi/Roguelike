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

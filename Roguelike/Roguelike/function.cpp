#include "function.h"
#include "MapData.h"

Point GridtoXY(Point p)
{
	return Point(p.x*MapData::getInstance().getMainGridSize().x, p.y*MapData::getInstance().getMainGridSize().y);
}
Point GridtoCenterXY(Point p)
{
	return Point(p.x*MapData::getInstance().getMainGridSize().x + MapData::getInstance().getMainGridSize().x / 2, p.y*MapData::getInstance().getMainGridSize().y + MapData::getInstance().getMainGridSize().y / 2);
}
Point XYtoGrid(Point p)
{
	return Point((int)(p.x / MapData::getInstance().getMainGridSize().x), (int)(p.y / MapData::getInstance().getMainGridSize().y));
}
Point GridtoXY(int x, int y)
{
	return Point(x*MapData::getInstance().getMainGridSize().x, y*MapData::getInstance().getMainGridSize().y);
}
Point SubGridtoXY(int x, int y)
{
	return Point(x*MapData::getInstance().getSubGridSize().x, y*MapData::getInstance().getSubGridSize().y);
}
Point GridtoCenterXY(int x, int y)
{
	return Point(x*MapData::getInstance().getMainGridSize().x + MapData::getInstance().getMainGridSize().x / 2, y*MapData::getInstance().getMainGridSize().y + MapData::getInstance().getMainGridSize().y / 2);
}
Point XYtoGrid(int x, int y)
{
	return Point((int)(x / MapData::getInstance().getMainGridSize().x), (int)(y / MapData::getInstance().getMainGridSize().y));
}
Point getMouseGrid() { return XYtoGrid(Mouse::Pos()) + MapData::getInstance().getCenterPoint() - MapData::getInstance().getDrawRange() / 2; }

void rotateDirection(Direction & direction, int i)
{
	int n = (direction + i) % 8;
	direction = static_cast<Direction>(n);
}

Point DirectionToPoint(int i)
{
	Point re;
	Direction d = Direction::Right;
	rotateDirection(d, i);
	switch (d)
	{
	case Direction::Right:
	case Direction::UpRight:
	case Direction::DownRight:
		re.x = 1;
		break;
	case Direction::Up:
	case Direction::Down:
		re.x = 0;
		break;
	default:
		re.x = -1;
	}
	switch (d)
	{
	case Direction::UpRight:
	case Direction::Up:
	case Direction::UpLeft:
		re.y = -1;
		break;
	case Direction::Right:
	case Direction::Left:
		re.y = 0;
		break;
	default:
		re.y = 1;
	}
	return re;
}

Direction PointToDirection(Point p)
{
	Direction re = Direction::Right;

	if (p.x == 1)
	{
		switch (p.y)
		{
		case 1:
			re = Direction::UpRight;
			break;
		case 0:
			re = Direction::Right;
			break;
		case -1:
			re = Direction::DownRight;
			break;
		}
	}
	else if (p.x == -1)
	{
		switch (p.y)
		{
		case 1:
			re = Direction::UpLeft;
			break;
		case 0:
			re = Direction::Left;
			break;
		case -1:
			re = Direction::DownLeft;
			break;
		}
	}
	else
	{
		switch (p.y)
		{
		case 1:
			re = Direction::Up;
			break;
		case -1:
			re = Direction::Down;
			break;
		}
	}
	return re;
}

double ToRadian(int i)
{
	i += 8;
	return Radians(360 * (1 - i / 8.0));
}


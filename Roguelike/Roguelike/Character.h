#pragma once
#include "Item.h"
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"

//キャラクター
enum CharacterStatus
{
	WaitKeyInput,
	Acting,
	EndAction,
	WaitOtherAction
};
class Character
{
public:
	Character(Point);
	Character(int, int);
	virtual void act();
	virtual void draw();
	virtual void move();
	virtual void attack();
	virtual bool enableLive() 
	{ 
		if (HP > 0)
			return true;
		else
		{
			return false;
		}
	}
	virtual void doSomethingAtDeath();

	void setGridPosition(Point p) { xyPosition = p; }
	void setRad(int d) { direction = d; }
	Point getGridPosition() { return XYtoGrid(xyPosition); }
	double getRad() { return Radians(direction); }
	void setStatus(CharacterStatus cs) { status = cs; }
	CharacterStatus getStatus() { return status; }
	void deleteItem()
	{
		for (size_t i = 0; i < inventory.size(); i++)
		{
			if (inventory[i].lock()->isUsed())
			{
				inventory[i].lock()->doSomethingAtDeath();
				inventory.erase(inventory.begin() + i);
			}
		}
	}
	std::vector<String> getChoice(std::vector<size_t> ints)
	{ 
		std::vector<String> re;
		if (ints.size() == 1)
		{
			for (size_t i = 0; i < inventory.size(); i++)
				re.emplace_back(inventory[i].lock()->getName());
			return re;
		}
		else
		{
			int i = (int)ints[0];
			ints.erase(ints.begin());
			return inventory[i].lock()->getChoice(ints);
		}
	}

	String getName() { return name; }

	//changeHP
	int increaseHP(int i)
	{
		HP += i;
		return i;
	}
	int decreaseHP(int i) 
	{
		if (i > HP)
			i = HP;
		HP -= i;
		return i;
	}

	//changeATK
	int increaseATK(int i)
	{
		ATK += i;
		return i;
	}
	int decreaseATK(int i)
	{
		if (i > ATK)
			i = ATK;
		ATK -= i;
		return i;
	}

	//changeDEF

	int increaseDEF(int i)
	{
		DEF += i;
		return i;
	}
	int decreaseDEF(int i)
	{
		if (i > DEF)
			i = DEF;
		DEF -= i;
		return i;
	}

	//void setHP(int h) { HP = h; }
	//void setATK(int a) { ATK = a; }
	//void setDEF(int d) { DEF = d; }
	int getHP() { return HP; }
	int getATK() { return ATK; }
	int getDEF() { return DEF; }

protected:
	Texture img;
	Point xyPosition;
	Color color;
	String name;
	int direction;

	CharacterStatus status;
	Array<std::weak_ptr<Item>>inventory;
	int HP, ATK, DEF;
};
//プレイヤー
class Player :public Character, public std::enable_shared_from_this<Player>
{
public:
	Player(Point pos);
	Player(int x, int y) :Player(Point(x, y)) {}

	void act()override;
	void move()override;
	void attack()override;
	void openInventory();
	void useItem();
};
//エネミー
class Enemy :public Character
{
public:
	Enemy(Point pos) :Character(pos)
	{
		color = Palette::Tomato;

		HP = 100;
		ATK = 10;
		DEF = 70;
	}
	Enemy(int x, int y) :Enemy(Point(x, y)) {}
};
class Sandbag :public Enemy
{
public:
	Sandbag(Point pos) :Enemy(pos)
	{
		img = Texture((L"Images/sandbag.png"));
		name = L"サンドバッグ";

		HP = 900;
		ATK = 0;
		DEF = 20;
	}
	Sandbag(int x, int y) :Sandbag(Point(x, y)) {}

	void move()override;
};
class Kyonshih :public Enemy
{
public:
	Kyonshih(Point pos) :Enemy(pos)
	{
		img = Texture((L"Images/pop.png"));
		name = L"キョンシーもどき";

		HP = 200;
		ATK = 40;
		DEF = 40;
	}
	Kyonshih(int x, int y) :Kyonshih(Point(x, y)) {}

	void attack()override;
};
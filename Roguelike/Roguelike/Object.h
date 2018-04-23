#pragma once
#include <Siv3D.hpp>
#include <memory>
#include "MapData.h"
#include "LogSystem.h"
#include "function.h"

//アイテム
class Item
{
public:
	Item(Point);
	Item(int, int);
	virtual void draw();
	virtual bool isUsed()
	{
		if (!used)
			return false;
		else
		{
			MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).deleteItem();
			return true;
		}
	}
	virtual void doSomethingAtDeath();

	virtual void use() { used = true; }

	void setGridPosition(Point p) { gridPosition = p; }
	Point getGridPosition() { return gridPosition; }
	String getName() { return name; }
	
	Array<String> getChoice(Array<size_t> ints)
	{
		Array<String> re = Array<String>{};
		if (ints.size() == 1)
		{
			re = Array<String>{ L"func0",L"func1", L"func2" };
			return re;
		}
		else
		{
			use();
			int i = ints[0];
			switch (i)
			{
				case 0:
					func0();
					break;
				case 1:
					func1();
					break;
				case 2:
					func2();
					break;
			}
			re.clear();
			return re;
		}
	}

	void func0() { LogSystem::getInstance().addLog(name + L"のfunc0を実行しました。"); }
	void func1() { LogSystem::getInstance().addLog(name + L"のfunc1を実行しました。"); }
	void func2() { LogSystem::getInstance().addLog(name + L"のfunc2を実行しました。"); }
protected:
	Texture img;
	Point xyPosition;
	Point gridPosition;
	Color color;
	String name;
	bool used;
	Array<String> choice;
};

class Glasses :public Item
{
public:
	Glasses(Point pos) :Item(pos)
	{
		img = Texture((L"Images/glasses.png"));
		name = L"誰かのメガネ";
	}
	Glasses(int x, int y) :Glasses(Point(x, y)) {}

	void use()override;
};
class ShimarinDango :public Item
{
public:
	ShimarinDango(Point pos) :Item(pos)
	{
		img = Texture((L"Images/shimarindango.png"));
		name = L"しまりんだんご";
	}
	ShimarinDango(int x, int y) :ShimarinDango(Point(x, y)) {}

	void use()override;
};

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
	virtual bool move();
	virtual bool attack();
	virtual bool enableLive() 
	{ 
		if (HP > 0)
			return true;
		else
		{
			MapData::getInstance().getOneGridData(XYtoGrid(xyPosition)).deleteCharacter();
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
	void deleteItem(size_t i)
	{
		if (i < inventory.size())
		{
			inventory[i]->doSomethingAtDeath();
			inventory.erase(inventory.begin() + i - 1);
		}
	}
	Array<String> getChoice(Array<size_t> ints)
	{ 
		Array<String> re = Array<String>{};
		if (ints.size() == 1)
		{
			for (size_t i = 0; i < inventory.size(); i++)
				re.emplace_back(inventory[i]->getName());
			//re = Array<String>{ L"itemA",L"itemB", L"itemC" };
			return re;
		}
		else
		{
			int i = ints[0];
			ints.erase(ints.begin());
			return inventory[i]->getChoice(ints);
		}
	}

	String getName() { return name; }
	int causeDamage(int damage) { return HP = damage > HP ? 0 : HP - damage; }

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
	Array<std::shared_ptr<Item>>inventory;
	int HP, ATK, DEF;
};
//プレイヤー
class Player :public Character
{
public:
	Player(Point pos);
	Player(int x, int y) :Player(Point(x, y)) {}

	void act()override;
	bool move()override;
	bool attack()override;
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

	bool move()override;
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

	bool attack()override;
};
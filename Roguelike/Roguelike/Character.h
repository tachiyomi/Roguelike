#pragma once
#include "Item.h"
#include "Ability.h"
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"

//キャラクター
enum ActionStatus
{
	WaitKeyInput,
	Acting,
	EndAction,
	WaitOtherAction
};
struct CharacterStatus
{
	void setStatus(int h, int a, int d, int mh)
	{
		HP = h;
		baseATK = a;
		baseDEF = d;
		baseMaxHP = mh;
	};
	void setItemStatus(std::vector<int> i)
	{
		if (i.size() < 3)
			return;
		exATK += i[0];
		exDEF += i[1];
		exMaxHP += i[2];
	};
	void deleteItemStatus(std::vector<int> i)
	{
		if (i.size() < 3)
			return;
		exATK -= i[0];
		exDEF -= i[1];
		exMaxHP -= i[2];
		if (HP > getMaxHP())
			HP = getMaxHP();
	}
	
	void setATK(int i) 
	{
		exATK -= getATK() - i;
		if (exATK < 0)
		{
			baseATK += exATK;
			exATK = 0;
		}
		if (baseATK < 0)
			baseATK = 0;
	};
	void setDEF(int i)
	{
		exDEF -= getDEF() - i;
		if (exDEF < 0)
		{
			baseDEF += exDEF;
			exDEF = 0;
		}
		if (baseDEF < 0)
			baseDEF = 0;
			
	};
	void setHP(int i) { HP = i; };

	const int getMaxHP() { return baseMaxHP + exMaxHP; };
	const int getATK() { return baseATK + exATK; };
	const int getDEF() { return baseDEF + exDEF; };
	const int getHP() { return HP; };

	int baseMaxHP = 0, baseATK = 0, baseDEF = 0, HP = 0;
	int exMaxHP = 0, exATK = 0, exDEF = 0;
};
class Character : public std::enable_shared_from_this<Character>
{
public:
	Character(Point);
	Character(int, int);
	virtual void act();
	virtual void draw();
	virtual void move();
	virtual void attack();
	void applyTurnStartAbility();
	void applyTurnEndAbility();
	void applyAttackAbility(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character>copyB);
	void applyDefendAbility(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character>copyB);
	void addAbility(std::shared_ptr<Ability>);
	virtual bool enableLive();
	virtual void doSomethingAtDeath();

	void setGridPosition(Point p) { xyPosition = p; }
	void setRad(int d) { direction = d; }
	Point getGridPosition() { return XYtoGrid(xyPosition); }
	double getRad() { return Radians(direction); }
	void setStatus(ActionStatus cs) { AS = cs; }
	ActionStatus getStatus() { return AS; }
	Array<String> getAbility()
	{ 
		Array<String> strs;
		for (auto& e : abilities)
			strs.emplace_back(e->getName());
		return strs;
	};
	void deleteItem()
	{
		for (size_t i = 0; i < inventory.size(); i++)
		{
			if (inventory[i].expired())
			{
				inventory.erase(inventory.begin() + i);
				i--;
			}
		}
	}
	void deleteAbility()
	{
		for (size_t i = 0; i < abilities.size(); i++)
		{
			if (abilities[i]->remove())
			{
				abilities.erase(abilities.begin() + i);
				i--;
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

	bool isEquipping(ItemType type)
	{
		switch (type)
		{
		case ItemType::Weapon:
			return !weapon.expired();
		case ItemType::Armor:
			return !armor.expired();
		case ItemType::Accessory:
			return !accessory.expired();
		}
		return false;
	}

	std::shared_ptr<Equipment> getEquipmentPointer(ItemType type)
	{
		switch (type)
		{
		case ItemType::Weapon:
			return weapon.lock();
		case ItemType::Armor:
			return armor.lock();
		case ItemType::Accessory:
			return accessory.lock();
		}
		return nullptr;
	}

	void equipped(std::shared_ptr<Equipment> eq) 
	{
		switch (eq->getType())
		{
		case ItemType::Weapon:
			if (!weapon.expired())
				weapon.lock()->takeout();
			weapon = eq;
			break;
		case ItemType::Armor:
			if (!armor.expired())
				armor.lock()->takeout();
			armor = eq;
			break;
		case ItemType::Accessory:
			if (!accessory.expired())
				accessory.lock()->takeout();
			accessory = eq;
			break;
		}
		addAbility(eq->getAbility());
		CS.setItemStatus(eq->getItemStatus());
	};

	void takeout(std::shared_ptr<Equipment> eq)
	{
		switch (eq->getType())
		{
		case ItemType::Weapon:
			weapon.reset();
			break;
		case ItemType::Armor:
			armor.reset();
			break;
		case ItemType::Accessory:
			accessory.reset();
			break;
		}
		for (size_t i = 0; i < abilities.size(); i++)
		{
			if (abilities[i] == eq->getAbility())
			{
				abilities.erase(abilities.begin() + i);
				i--;
			}
		}
		CS.deleteItemStatus(eq->getItemStatus());
	};

	//changeHP
	int increaseHP(int i)
	{
		if (i + CS.getHP() > CS.getMaxHP())
			i = CS.getMaxHP() - CS.getHP();
		CS.HP += i;
		return i;
	}
	int decreaseHP(int i) 
	{
		if (i > CS.getHP())
			i = CS.getHP();
		CS.HP -= i;
		return i;
	}

	//changeATK
	int increaseATK(int i)
	{
		CS.exATK += i;
		return i;
	}
	int decreaseATK(int i)
	{
		if (i > CS.exATK)
			i = CS.exATK;
		CS.exATK -= i;
		return i;
	}

	//changeDEF

	int increaseDEF(int i)
	{
		CS.exDEF += i;
		return i;
	}
	int decreaseDEF(int i)
	{
		if (i > CS.exDEF)
			i = CS.exDEF;
		CS.exDEF -= i;
		return i;
	}

	void setHP(int h) { CS.setHP(h); }
	void setATK(int a) { CS.setATK(a); }
	void setDEF(int d) { CS.setDEF(d); }
	const int getHP() { return CS.getHP(); }
	const int getATK() { return CS.getATK(); }
	const int getDEF() { return CS.getDEF(); }
	const int getMaxHP() { return CS.getMaxHP(); }

protected:
	Texture img;
	Point xyPosition;
	Color color;
	String name;
	int direction;

	ActionStatus AS;
	CharacterStatus CS;
	std::weak_ptr<Equipment>weapon, armor, accessory;
	Array<std::weak_ptr<Item>>inventory;
	Array<std::shared_ptr<Ability>>abilities;
};
//プレイヤー
class Player :public Character
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

		CS.setStatus(100, 10, 70, 100);
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

		CS.setStatus(900, 0, 20, 900);
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

		CS.setStatus(200, 40, 40, 200);

		abilities.emplace_back(std::make_shared<IgnoreArmor>());
	}
	Kyonshih(int x, int y) :Kyonshih(Point(x, y)) {}

	void attack()override;
};
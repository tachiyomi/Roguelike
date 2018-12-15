#pragma once
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"
#include "Ability.h"

//アイテム
enum ItemType
{
	Weapon,
	Armor,
	Accessory,
	Consumables
};
enum ItemId
{
	item=0,
	potion,glasses,microphone,gemoffly,
	equipment=100,
	blade,shield,ring
};
class Item
{
public:
	Item(Point);
	Item(int, int);
	virtual void draw();
	virtual bool isUsed() { return used; }
	virtual bool isInInventory() { return inInventory; }
	virtual void doSomethingAtRemoval();

	virtual void use() { used = true; }

	void setGridPosition(Point p) { gridPosition = p; }
	Point getGridPosition() { return gridPosition; }
	String getName() { return name; }
	ItemType getType() { return type; }

	virtual std::vector<String> getChoice(std::vector<size_t> ints)
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"未設定" };
			return re;
		}
		else
		{
			int i = (int)ints[0];
			switch (i)
			{
			case 0:
				displayErrorMessage();
				break;
			}
			re.clear();
			return re;
		}
	}
	void displayErrorMessage() { LogSystem::getInstance().addLog(name + L"の使用選択肢が設定されていません。"); }

	bool inInventory;
protected:
	int id = ItemId::item;
	Texture img;
	Point position;
	Point gridPosition;
	Color color;
	String name;
	bool used;
	Array<String> choice;
	ItemType type;
};

//消耗品
class Glasses :public Item
{
public:
	Glasses(Point pos) :Item(pos)
	{
		id = ItemId::glasses;
		img = Texture((L"Images/glasses.png"));
		name = L"誰かのメガネ";
		type = ItemType::Consumables;
	}
	Glasses(int x, int y) :Glasses(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"食べる",L"握り潰す" };
			return re;
		}
		else
		{
			int i = (int)ints[0];
			switch (i)
			{
			case 0:
				eat();
				break;
			case 1:
				crush();
				break;
			}
			re.clear();
			return re;
		}
	}
	void eat()
	{
		LogSystem::getInstance().addLog(name + L"を食べようとしたが諦めた。");
	}
	void crush()
	{
		LogSystem::getInstance().addLog(name + L"を握り潰した。");
		use();
	}
};
class Potion :public Item
{
public:
	Potion(Point pos) :Item(pos)
	{
		id = ItemId::potion;
		img = Texture((L"/1003"));
		name = L"回復薬";
		type = ItemType::Consumables;
	}
	Potion(int x, int y) :Potion(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"食べさせる" };
			return re;
		}
		else
		{
			ints.erase(ints.begin());
			return selectTarget(ints);
		}
	}

	std::vector<String> selectTarget(std::vector<size_t>);

	void eat(size_t);
};
class Microphone :public Item
{
public:
	Microphone(Point pos) :Item(pos)
	{
		id = ItemId::microphone;
		img = Texture((L"Images/microphone.png"));
		name = L"マイク";
		type = ItemType::Consumables;
	}
	Microphone(int x, int y) :Microphone(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"熱唱する" };
			return re;
		}
		else
		{
			ints.erase(ints.begin());
			return selectSong(ints);
		}
	}

	std::vector<String> selectSong(std::vector<size_t>);

	void song(size_t);
};
class GemOfFly :public Item
{
public:
	GemOfFly(Point pos) :Item(pos)
	{
		id = ItemId::gemoffly;
		img = Texture((L"Images/microphone.png"));
		name = L"凄い玉";
		type = ItemType::Consumables;
	}
	GemOfFly(int x, int y) :GemOfFly(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"次の階層へ行く" };
			return re;
		}
		else
		{
			ints.erase(ints.begin());
			goNext();
			re.clear();
			return re;
		}
	}

	void goNext();
};

//装備品
class Equipment :public Item, std::enable_shared_from_this<Equipment>
{
public:
	Equipment(Point pos) :Item(pos)
	{
		name = L"Equipment";
		type = ItemType::Weapon;
		isEquipped = false;
		itemStatus = std::vector<int>{ 0,0,0 };
	}
	Equipment(int x, int y) :Equipment(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override;
	void equipped();
	void takeout();

	const std::vector<int> getItemStatus() { return itemStatus; }
	const std::shared_ptr<Ability> getAbility() { return ability; }
protected:
	bool isEquipped;
	std::vector<int> itemStatus;
	std::shared_ptr<Ability> ability;
};

class Blade :public Equipment
{
public:
	Blade(Point pos) :Equipment(pos)
	{
		img = Texture((L"Images/blade.png"));
		name = L"ただの剣";
		type = ItemType::Weapon;
		itemStatus = { 20,0,0 };
	}
	Blade(int x, int y) :Blade(Point(x, y)) {}
};
class Shield :public Equipment
{
public:
	Shield(Point pos) :Equipment(pos)
	{
		img = Texture((L"Images/shield.png"));
		name = L"ただの盾";
		type = ItemType::Armor;
		itemStatus = { 0,20,0 };
	}
	Shield(int x, int y) :Shield(Point(x, y)) {}
};
class Ring :public Equipment
{
public:
	Ring(Point pos) :Equipment(pos)
	{
		img = Texture((L"Images/ring.png"));
		name = L"ただの指輪";
		type = ItemType::Accessory;
		itemStatus = { 0,0,100 };
	}
	Ring(int x, int y) :Ring(Point(x, y)) {}
};
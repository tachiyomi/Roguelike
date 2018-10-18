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
class Item : public std::enable_shared_from_this<Item>
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
	std::shared_ptr<Ability> getAbility() { return std::make_shared<Shout>(); }

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
	Texture img;
	Point xyPosition;
	Point gridPosition;
	Color color;
	String name;
	bool used;
	Array<String> choice;
	ItemType type;
};

class Glasses :public Item
{
public:
	Glasses(Point pos) :Item(pos)
	{
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
class ShimarinDango :public Item
{
public:
	ShimarinDango(Point pos) :Item(pos)
	{
		img = Texture((L"Images/shimarindango.png"));
		name = L"しまりんだんご";
		type = ItemType::Consumables;
	}
	ShimarinDango(int x, int y) :ShimarinDango(Point(x, y)) {}

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
class Blade :public Item
{
public:
	Blade(Point pos) :Item(pos)
	{
		img = Texture((L"Images/blade.png"));
		name = L"ただの剣";
		type = ItemType::Weapon;

		isEquipped = false;
	}
	Blade(int x, int y) :Blade(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			if (!isEquipped)
				re = std::vector<String>{ L"装備する" };
			else
				re = std::vector<String>{ L"装備解除する" };
			return re;
		}
		else
		{
			if(!isEquipped)
				equipped();
			else
				takeout();
			re.clear();
			return re;
		}
	}
	void equipped();
	void takeout();
private:
	bool isEquipped;
};
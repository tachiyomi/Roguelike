#pragma once
#include "Character.h"
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"

//�A�C�e��
class Item
{
public:
	Item(Point);
	Item(int, int);
	virtual void draw();
	virtual bool isUsed() { return used; }
	virtual bool isInInventory() { return inInventory; }

	virtual void doSomethingAtDeath();

	virtual void use() { used = true; }

	void setGridPosition(Point p) { gridPosition = p; }
	Point getGridPosition() { return gridPosition; }
	String getName() { return name; }

	virtual std::vector<String> getChoice(std::vector<size_t> ints)
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"���ݒ�" };
			return re;
		}
		else
		{
			int i = ints[0];
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
	void displayErrorMessage() { LogSystem::getInstance().addLog(name + L"�̎g�p�I�������ݒ肳��Ă��܂���B"); }

	bool inInventory;
	/*
	std::vector<String> getChoice2(std::vector<size_t> ints)
	{
	std::vector<String> re;
	if (ints.size() == 1)
	{
	re = std::vector<String>{ L"func0ww",L"func1ww", L"func2ww" };
	return re;
	}
	else
	{
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

	void func0() { LogSystem::getInstance().addLog(name + L"��func0�����s���܂����B"); use(); }
	void func1() { LogSystem::getInstance().addLog(name + L"��func1�����s���܂����B"); use(); }
	void func2() { LogSystem::getInstance().addLog(name + L"��func2�����s���܂����B"); use(); }
	*/

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
		name = L"�N���̃��K�l";
	}
	Glasses(int x, int y) :Glasses(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"�H�ׂ�",L"����ׂ�" };
			return re;
		}
		else
		{
			int i = ints[0];
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
		LogSystem::getInstance().addLog(name + L"��H�ׂ悤�Ƃ��������߂��B");
	}
	void crush()
	{
		LogSystem::getInstance().addLog(name + L"������ׂ����B");
		use();
	}
};
class ShimarinDango :public Item
{
public:
	ShimarinDango(Point pos) :Item(pos)
	{
		img = Texture((L"Images/shimarindango.png"));
		name = L"���܂�񂾂�";
	}
	ShimarinDango(int x, int y) :ShimarinDango(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"�H�ׂ�����" };
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
		name = L"�}�C�N";
	}
	Microphone(int x, int y) :Microphone(Point(x, y)) {}

	std::vector<String> getChoice(std::vector<size_t> ints) override
	{
		std::vector<String> re;
		if (ints.size() == 1)
		{
			re = std::vector<String>{ L"�M������" };
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

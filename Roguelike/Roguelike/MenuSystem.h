#pragma once
#include <Siv3D.hpp>
#include "Object.h"

class MenuSystem
{
public:
	static MenuSystem& getInstance()
	{
		static MenuSystem menuInstance;
		return menuInstance;
	}

	void openMenu(std::shared_ptr<Character>);
	void closeMenu();
	void draw();

	void setOrigin(Point p) { menuOrigin = p; }
	void setSize(Size s) { menuSize = s; }
	const bool isOpening() { return enableSee; }
	const Size getSize() { return menuSize; }
protected:
	MenuSystem();
	virtual ~MenuSystem() {}
private:
	//void operator = (const MenuSystem& obj) {}
	//MenuSystem(const MenuSystem &obj) {}

	Point menuOrigin;
	Size menuSize;
	bool enableSee;
	std::shared_ptr<Character> playingCharacter;
	int selectItemNumber;
};
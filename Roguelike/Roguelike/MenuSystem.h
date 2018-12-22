#pragma once
#include <Siv3D.hpp>
#include "Character.h"
#include "Item.h"
#include "KeyInputManager.h"

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
	bool update();
	void draw();

	void setOrigin(Point p) { menuOrigin = p; }
	void setSize(Size s) { menuSize = s; }
	const bool isOpening() { return enableSee; }
	const Size getSize() { return menuSize; }
protected:
	MenuSystem();
	virtual ~MenuSystem() {}
private:
	void operator = (const MenuSystem&) {}
	MenuSystem(const MenuSystem&) {}

	Texture background;
	Point menuOrigin;
	Size menuSize;
	bool enableSee;
	std::shared_ptr<Character> playingCharacter;
	std::vector<size_t> selectChoiceNumber;
};
#include"MenuSystem.h"
#include <Siv3D.hpp>
#include <memory>
#include "MapData.h"
#include "LogSystem.h"
#include "Object.h"

MenuSystem::MenuSystem() :
	menuOrigin(Window::Width() / 2 + 10, 10), 
	menuSize(Window::Width() / 2, Window::Height() - LogSystem::getInstance().getSize().y), 
	enableSee(false),
	selectItemNumber(0)
{}
void MenuSystem::openMenu(std::shared_ptr<Character> c)
{
	playingCharacter = c;
	enableSee = true;
}
void MenuSystem::closeMenu()
{
	playingCharacter.reset();
	enableSee = false;
}
void MenuSystem::draw()
{
	if (!enableSee)
		return;

	if (playingCharacter->getInventory().size() != 0)
	{
		if (Input::KeyDown.clicked)
			selectItemNumber = (selectItemNumber + 1) % (int)playingCharacter->getInventory().size();
	}

	Transformer2D transformer(Mat3x2::Translate(menuOrigin), false);
	for (size_t i = 0; i < playingCharacter->getInventory().size(); i++)
	{
		if (i != selectItemNumber)
			FontAsset(L"logFont")(L" " + playingCharacter->getInventory()[i]->getName() + L" ").draw(0.0, (double)i * 30).drawFrame(0.0, 1.5, Palette::Gold);
		else
			FontAsset(L"logFont")(L" " + playingCharacter->getInventory()[i]->getName() + L" ").draw(0.0, (double)i * 30).drawFrame(0.0, 1.5, Palette::Red);
	}
}

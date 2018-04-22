#include"MenuSystem.h"
#include <Siv3D.hpp>
#include <memory>
#include "MapData.h"
#include "LogSystem.h"
#include "Object.h"

MenuSystem::MenuSystem() :
	menuOrigin(Window::Width() / 2 + 10, 10), 
	menuSize(Window::Width() / 2, Window::Height() - LogSystem::getInstance().getSize().y), 
	enableSee(false)
{
	selectChoiceNumber.emplace_back(0);
}
void MenuSystem::openMenu(std::shared_ptr<Character> c)
{
	playingCharacter = c;
	enableSee = true;
}
void MenuSystem::closeMenu()
{
	selectChoiceNumber.clear();
	selectChoiceNumber.emplace_back(0);
	playingCharacter.reset();
	enableSee = false;
}
void MenuSystem::draw()
{
	if (!enableSee)
		return;

	//if (playingCharacter->getChoice(selectChoiceNumber.size() - 1).size() == 0)
	//	return;

	if (Input::KeyDown.clicked)
		selectChoiceNumber.back() = (selectChoiceNumber.back() + 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
	if (Input::KeyEnter.clicked)
	{
		selectChoiceNumber.emplace_back(0);
		if (playingCharacter->getChoice(selectChoiceNumber).empty())
		{
			closeMenu();
			return;
		}
	}
	if (Input::KeyBackspace.clicked && selectChoiceNumber.size() != 0)
		selectChoiceNumber.pop_back();

	Transformer2D transformer(Mat3x2::Translate(menuOrigin), false);
	int k = 0;
	while (k < (int)selectChoiceNumber.size())
	{
		Array<size_t> ints = selectChoiceNumber;
		for (int i = 0; i < (int)selectChoiceNumber.size() - k - 1; i++)
			ints.pop_back();
		for (size_t i = 0; i < playingCharacter->getChoice(ints).size(); i++)
		{
			int j = 0;
			for (int l = 0; l < ints.size() - 1; l++)
				j += ints[l];
			if (i != selectChoiceNumber[k])
				FontAsset(L"logFont")(L" " + playingCharacter->getChoice(ints)[i] + L" ").draw(k * 150.0, (j+i) * 30.0).drawFrame(0.0, 1.5, Palette::Gold);
			else
				FontAsset(L"logFont")(L" " + playingCharacter->getChoice(ints)[i] + L" ").draw(k * 150.0, (j+i) * 30.0).drawFrame(0.0, 1.5, Palette::Red);
		}
		k++;
	}

	/*
	for (size_t i = 0; i < playingCharacter->getInventory().size(); i++)
	{
		if (i != selectItemNumber)
			FontAsset(L"logFont")(L" " + playingCharacter->getInventory()[i]->getName() + L" ").draw(0.0, (double)i * 30).drawFrame(0.0, 1.5, Palette::Gold);
		else
			FontAsset(L"logFont")(L" " + playingCharacter->getInventory()[i]->getName() + L" ").draw(0.0, (double)i * 30).drawFrame(0.0, 1.5, Palette::Red);
	}
	for (size_t i = 0; i < playingCharacter->getInventory()[selectItemNumber]->getChoice().size(); i++)
	{
		FontAsset(L"logFont")(L" " + playingCharacter->getInventory()[selectItemNumber]->getChoice()[i] + L" ").draw(150.0, (double)(i + selectItemNumber) * 30).drawFrame(0.0, 1.5, Palette::Gold);
	}
	*/
}

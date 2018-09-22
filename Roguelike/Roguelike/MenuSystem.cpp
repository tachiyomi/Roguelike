#include"MenuSystem.h"
#include <Siv3D.hpp>
#include <memory>
#include <numeric>
#include "MapData.h"
#include "LogSystem.h"
#include "Character.h"
#include "Item.h"

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
	playingCharacter->deleteItem();
	playingCharacter.reset();
	enableSee = false;
}
bool MenuSystem::update()
{
	//ƒ^[ƒ“Œo‰ß‚³‚¹‚é‚Æ‚«true
	if (!enableSee)
		return false;

	if (Input::KeyDown.clicked || Gamepad(0).povBackward.clicked)
		selectChoiceNumber.back() = (selectChoiceNumber.back() + 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
	if (Input::KeyUp.clicked || Gamepad(0).povForward.clicked)
		selectChoiceNumber.back() = (selectChoiceNumber.back() + (int)playingCharacter->getChoice(selectChoiceNumber).size() - 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
	if ((Input::KeyEnter.clicked || Gamepad(0).button(1).clicked) && (int)playingCharacter->getChoice(selectChoiceNumber).size() > 0)
	{
		selectChoiceNumber.emplace_back(0);
		if (playingCharacter->getChoice(selectChoiceNumber).empty())
		{
			closeMenu();
			return true;
		}
	}
	if ((Input::KeyBackspace.clicked || Gamepad(0).button(0).clicked) && selectChoiceNumber.size() > 1)
		selectChoiceNumber.pop_back();

	return false;
}
void MenuSystem::draw()
{
	if (!enableSee)
		return;

	Transformer2D transformer(Mat3x2::Translate(menuOrigin), false);

	int k = 0;
	while (k < (int)selectChoiceNumber.size())
	{
		std::vector<size_t> ints(k + 1);
		std::copy(selectChoiceNumber.begin(), selectChoiceNumber.begin() + k, ints.begin());
		for (size_t i = 0; i < playingCharacter->getChoice(ints).size(); i++)
		{
			int sum = std::accumulate(ints.begin(), ints.end() - 1, 0);
			Color color = i != selectChoiceNumber[k] ? Palette::Yellow : Palette::Red;
			FontAsset(L"logFont")(L" " + playingCharacter->getChoice(ints)[i] + L" ").draw(k * 150.0, (sum + i) * 30.0).drawFrame(0.0, 1.5, color);
		}
		k++;
	}
}

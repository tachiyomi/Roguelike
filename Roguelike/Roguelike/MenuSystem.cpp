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
	background = Texture(L"Images/menuBackground.png");
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
void MenuSystem::draw()
{
	if (!enableSee)
		return;

	if ((int)playingCharacter->getChoice(selectChoiceNumber).size() > 0)
	{
		if (Input::KeyDown.clicked || Gamepad(0).povBackward.clicked)
			selectChoiceNumber.back() = (selectChoiceNumber.back() + 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
		if (Input::KeyUp.clicked || Gamepad(0).povForward.clicked)
			selectChoiceNumber.back() = (selectChoiceNumber.back() + (int)playingCharacter->getChoice(selectChoiceNumber).size() - 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
		if ((Input::KeyEnter.clicked || Gamepad(0).button(1).clicked))
		{
			selectChoiceNumber.emplace_back(0);
			if (playingCharacter->getChoice(selectChoiceNumber).empty())
			{
				closeMenu();
				return;
			}
		}
		if ((Input::KeyBackspace.clicked || Gamepad(0).button(0).clicked) && selectChoiceNumber.size() > 1)
			selectChoiceNumber.pop_back();
	}
		
	Rect(menuSize).movedBy(Window::Width() / 2, 0)(background).draw().drawFrame(1.0,0,Palette::Black);

	Transformer2D transformer(Mat3x2::Translate(menuOrigin), false);

	int k = 0;
	double w = 0;
	while (k < (int)selectChoiceNumber.size())
	{
		const int width = (int)w;
		std::vector<size_t> ints(k + 1);
		std::copy(selectChoiceNumber.begin(), selectChoiceNumber.begin() + k, ints.begin());
		for (size_t i = 0; i < playingCharacter->getChoice(ints).size(); i++)
		{
			int sum = std::accumulate(ints.begin(), ints.end() - 1, 0);
			String str = L" " + playingCharacter->getChoice(ints)[i] + L" ";
			Point point = Point(width, (sum + i) * 25.0);
			if (i == selectChoiceNumber[k])
			{
				FontAsset(L"menuFont")(str).region(point).draw(Palette::Darkslateblue);
				w += FontAsset(L"menuFont")(str).draw(point).drawFrame(0.0, 2.5, Palette::Dodgerblue).w + 6;
			}
			else
			{
				FontAsset(L"menuFont")(str).region(point).draw(Palette::Black);
				FontAsset(L"menuFont")(str).draw(point, Palette::Silver).drawFrame(0.0, 1.5, Palette::Skyblue);
			}
		}
		k++;
	}
}

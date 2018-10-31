#include"MenuSystem.h"
#include <memory>
#include <numeric>
#include "MapData.h"
#include "LogSystem.h"

MenuSystem::MenuSystem() :
	menuOrigin(Window::Width() / 2 + 10, 10), 
	menuSize(Window::Width() / 2,Window::Width() / 2 -100),
	enableSee(false)
{
	background = Texture(L"Images/menuBackground.jpg");
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
bool MenuSystem::update()
{
	//ƒ^[ƒ“Œo‰ß‚³‚¹‚é‚Æ‚«true
	if (!enableSee)
		return false;

	if ((int)playingCharacter->getChoice(selectChoiceNumber).size() > 0)
	{
		if (Input::KeyDown.clicked || Gamepad(0).povBackward.clicked)
			selectChoiceNumber.back() = (selectChoiceNumber.back() + 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
		if (Input::KeyUp.clicked || Gamepad(0).povForward.clicked)
			selectChoiceNumber.back() = (selectChoiceNumber.back() + (int)playingCharacter->getChoice(selectChoiceNumber).size() - 1) % (int)playingCharacter->getChoice(selectChoiceNumber).size();
		if ((Input::KeyEnter.clicked || Gamepad(0).button(1).clicked))
		{
			selectChoiceNumber.emplace_back(0);
			std::vector<s3d::String> str = playingCharacter->getChoice(selectChoiceNumber);
			if (str.empty())
			{
				closeMenu();
				return true;
			}
			else if (str[0] == L"false")
			{
				closeMenu();
				return false;
			}
		}
		if ((Input::KeyBackspace.clicked || Gamepad(0).button(0).clicked) && selectChoiceNumber.size() > 1)
			selectChoiceNumber.pop_back();
	}
	return false;
}

void MenuSystem::draw()
{
	Rect(menuSize).movedBy(menuOrigin)(background).draw(Color(Palette::Silver,100)).drawFrame(1.0, 0, Palette::Black);

	Transformer2D transformer(Mat3x2::Translate(menuOrigin + Point::One * 10), false);

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
			Point point = Point(width, (sum + i) * (FontAsset(L"menuFont").height + 7));
			if (i == selectChoiceNumber[k])
			{
				FontAsset(L"menuFont")(str).region(point).draw(Palette::Royalblue);
				w += FontAsset(L"menuFont")(str).draw(point).drawFrame(0.0, 2.5, Palette::Navy).w + 6;
			}
			else
			{
				FontAsset(L"menuFont")(str).region(point).draw(Palette::Black);
				FontAsset(L"menuFont")(str).draw(point, Palette::Gray).drawFrame(1.5, 0.0, Palette::Gray);
			}
		}
		k++;
	}
}

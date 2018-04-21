#include "LogSystem.h"
#include <Siv3D.hpp>

LogSystem::LogSystem() :
	maxLogSize(20),
	logOrigin(0, Window::Height()),
	logSize(Window::Width(), 180)
{
	for (int i = 1; i <= 5; i++)
		icon.emplace_back(Format(L"Images/icon_nadeshiko_{}.png"_fmt, i), Format(L"<nade{}>"_fmt, i), Palette::Pink);

	for (int i = 1; i <= 5; i++)
		icon.emplace_back(Format(L"Images/icon_shimarin_{}.png"_fmt, i), Format(L"<shima{}>"_fmt, i), Palette::Royalblue);

	const int iconHeight = 50;
	for (size_t i = 0; i < icon.size(); i++)
	{
		const double ratio = iconHeight / (double)(icon[i].size.y);
		icon[i].size = Size((int)(icon[i].size.x*ratio), (int)(icon[i].size.y*ratio));
	}
}
void LogSystem::displayLog()
{
	Transformer2D transformer(Mat3x2::Translate(logOrigin), false);
	const int gap = 10;
	int h = 0;
	for (int i = 0; i < maxLogSize; i++)
	{
		if (i >= (int)log.size())
			break;

		const int nexti = i + 1;
		if (log.at(i).icon == -1)
		{
			h += FontAsset(L"logFont").height + gap;
			FontAsset(L"logFont")(log.at(i).str).draw(gap, h - gap - FontAsset(L"logFont").ascent);
		}
		else
		{
			icon[log.at(i).icon].img.resize(icon[log.at(i).icon].size)
				.draw(gap, h + gap).drawFrame(0, 2, icon[log.at(i).icon].color);

			h += icon[log.at(i).icon].size.y + gap + 2;

			FontAsset(L"logFont")(log.at(i).str)
				.draw(gap * 2 + icon[log.at(i).icon].size.x, h - gap - FontAsset(L"logFont").ascent);
		}
		Line(0, h, logSize.x, h).draw(Palette::Gold);
	}
}
void LogSystem::addLog(String str)
{
	int k = -1;
	for (size_t i = 0; i < icon.size(); i++)
	{
		if (int wordpos = str.indexOf(icon[i].keyword) != -1)
		{
			k = (int)i;
			str.erase(str.begin() + wordpos - 1, str.begin() + wordpos + icon[i].keyword.length - 1);
		}
	}

	log.push_front(LogData(str, k));
	if ((int)log.size() > maxLogSize)
		log.pop_back();
}

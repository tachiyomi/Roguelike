#include "Scene.h"
#include "DungeonSystem.h"

void Title::init()
{
	texture = Texture(L"/3000");
}

void Title::update()
{
	if (Input::AnyKeyReleased())
	{
		this->changeScene(2, 2000);
		DungeonSystem::getInstance().startAtFirstFloor();
	}
}

void Title::draw() const
{
	Window::ClientRect()(texture).draw();
	FontAsset(L"titleFont")(L"Press Any Key").drawAt(Window::Center() + Point::UnitY * 150, Palette::White);
}

void Select::init()
{
	DungeonSystem::getInstance().startAtFirstFloor();
}

void Select::update()
{
	if (Input::AnyKeyReleased())
	{
		this->changeScene(2, 2000);
	}
}

void Select::draw() const
{
	FontAsset(L"PauseFont")(L"新しくゲームを始める").drawAt(Window::Center(), Palette::White);
}

void DisplayFloor::init()
{
	m_data->floorNum = DungeonSystem::getInstance().getFloorNumber();
}

void DisplayFloor::update()
{
	this->changeScene(3);
}

void DisplayFloor::draw() const
{
	//FontAsset(L"PauseFont")(L"邪悪な墓　B" + ToString(6 - m_data->floorNum) + L"F").drawAt(Window::Center(), Palette::White);
}

void Play::init()
{
	conti = L"ゲームを続ける";
	exit = L"タイトルに戻る";
	flag = true;
	screenShot = Texture();
	paused = false;
}

void Play::update()
{
	if (Input::KeyP.clicked)
	{
		paused = !paused;
		if (paused)
		{
			ScreenCapture::Request();
			flag = true;
		}
		else
			screenShot.release();
	}

	if (paused)
	{
		if (ScreenCapture::HasNewFrame())
		{
			Image img = ScreenCapture::GetFrame();
			screenShot = Texture(img.gaussianBlurred(10, 10).brightened(-20));
		}
		if (Input::KeyUp.clicked || Input::KeyDown.clicked)
			flag = !flag;
		else if (Input::KeyEnter.clicked)
		{
			if (flag)
			{
				paused = false;
				screenShot.release();
			}
			else
				changeScene(0);
		}
		return;
	}

	DungeonSystem::getInstance().update();

	if (DungeonSystem::getInstance().gameClear())
	{
		changeScene(4, 3000);
		return;
	}
	else if (DungeonSystem::getInstance().gameOver())
	{
		changeScene(5, 3000);
		return;
	}

	if (m_data->floorNum != DungeonSystem::getInstance().getFloorNumber())
	{
		changeScene(2, 2000);
		m_data->floorNum = DungeonSystem::getInstance().getFloorNumber();
	}
}

void Play::draw() const
{
	if (paused && !screenShot.isEmpty())
	{
		screenShot.draw();
		drawButtonAtPause();
		return;
	}

	DungeonSystem::getInstance().draw();
}

void Play::drawButtonAtPause() const
{
	FontAsset(L"PauseFont")(conti).region().scaled(1.5).setCenter(Window::Center()).draw(Color(Palette::White, 210)).drawFrame(1.0, flag ? 5.0 : 0.0, Palette::Deepskyblue);
	FontAsset(L"PauseFont")(conti).drawAt(Window::Center(), Palette::Black);

	FontAsset(L"PauseFont")(exit).region().scaled(1.5).setCenter(Window::Center() + Point::UnitY * 150).draw(Color(Palette::White, 210)).drawFrame(1.0, !flag ? 5.0 : 0.0, Palette::Tomato);
	FontAsset(L"PauseFont")(exit).drawAt(Window::Center() + Point::UnitY * 150, Palette::Black);
}

void GameClear::init()
{
}

void GameClear::update()
{
	if (Input::AnyKeyReleased())
		this->changeScene(0);
}

void GameClear::draw() const
{
	const Vec2 tl = FontAsset(L"PauseFont")(L"無事生還!").drawAt(Window::Center(), Palette::White).tl;
	FontAsset(L"PauseFont")(L"到達階層:地上").draw(tl + Point::UnitY * 50, Palette::White);
	FontAsset(L"PauseFont")(L"プレイ時間:" + DungeonSystem::getInstance().getTotaltimeANDstop()).draw(tl + Point::UnitY * 100, Palette::White);
}

void GameOver::init()
{
}

void GameOver::update()
{
	if (Input::AnyKeyReleased())
		this->changeScene(0);
}

void GameOver::draw() const
{
	const Vec2 tl = FontAsset(L"PauseFont")(L"道中で力尽きた...").drawAt(Window::Center(), Palette::White).tl;
	FontAsset(L"PauseFont")(L"到達階層:地下" + ToString(6 - m_data->floorNum) + L"階").draw(tl + Point::UnitY * 50, Palette::White);
	FontAsset(L"PauseFont")(L"プレイ時間:" + DungeonSystem::getInstance().getTotaltimeANDstop()).draw(tl + Point::UnitY * 100, Palette::White);
}

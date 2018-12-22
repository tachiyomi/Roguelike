#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include"KeyInputManager.h" 

struct DungeonData
{
	DungeonData(String n,String u)
	{
		name = n;
		url = u;
	}

	String name, url;
};

struct CommonData
{
	int floorNum = 0;
};

class Title :public SceneManager<int,CommonData>::Scene
{
	void init()override;
	void update()override;
	void draw()const override;

private:
	Texture texture;
};

class Select :public SceneManager<int, CommonData>::Scene
{
	void init()override;
	void update()override;
	void draw()const override;

private:
	std::vector<DungeonData> dungeonList;
	int selectNumber;
};

/*
class DisplayFloor :public SceneManager<int, CommonData>::Scene
{
	void init()override;
	void update()override;
	void draw()const override;
};
*/ 

class Play :public SceneManager<int, CommonData>::Scene
{
	void init()override;
	void update()override;
	void draw()const override;
	void drawButtonAtPause()const;

private:
	String conti, exit;
	bool flag;
	Texture screenShot;
	bool paused;
};

class GameClear :public SceneManager<int, CommonData>::Scene
{
	void init()override;
	void update()override;
	void draw()const override;
};

class GameOver :public SceneManager<int, CommonData>::Scene
{
	void init()override;
	void update()override;
	void draw()const override;
};
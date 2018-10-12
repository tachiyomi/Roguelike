#pragma once
#include<Siv3D.hpp>
#include <memory>
#include "Character.h"
#include "Item.h"

//一つのグリッドの情報
struct GridData
{
public:
	GridData() :
		terrain(-1),
		enableInvade(false),
		enableDraw(false)
	{}

	void setTerrain(int t)
	{
		terrain = t;
		if (terrain == 1)
			enableInvade = true;
	}
	void setEnableDraw(bool b) { enableDraw = b; }
	void setCharacter(std::shared_ptr<Character> c) { characterPointer = c; }
	void setItem(std::shared_ptr<Item> i) { itemPointer = i; }

	int getTerrain() { return terrain; }
	std::weak_ptr<Character>&  getWeakCharacter() { return characterPointer; }
	std::weak_ptr<Item>&  getWeakItem() { return itemPointer; }
	std::shared_ptr<Character> getCharacter() { return characterPointer.lock(); }
	std::shared_ptr<Item>  getItem() { return itemPointer.lock(); }
	bool enableAddCharacter() { return enableInvade && characterPointer.expired(); }
	bool enableAddItem() { return enableInvade && itemPointer.expired(); }
	bool canBeDraw() { return enableDraw; }
	bool isUnderCharacter() { return !characterPointer.expired(); }
	bool isUnderItem() { return !itemPointer.expired(); }

private:
	int terrain;
	bool enableInvade;
	bool enableDraw;
	std::weak_ptr<Character> characterPointer;
	std::weak_ptr<Item> itemPointer;
};

//全てのグリッドの情報
class MapData
{
public:
	static MapData& getInstance()
	{
		static MapData Instance;
		return Instance;
	}
	void update();
	void fight(const std::shared_ptr<Character>&, const std::shared_ptr<Character>&);
	void drawMainMap();
	void drawSubMap();

	void sort() { ; }
	void deleteObject();

	void loadMap();
	void drawOneGridGround(Point, Size, int);

	template<typename T>
	void registerCharacter(T t)
	{
		characters.emplace_back(std::make_shared<T>(t.getGridPosition()));
		getOneGridData(t.getGridPosition()).setCharacter(characters.back());
		//characters.emplace_back(t);
		//getOneGridData(characters.back()->getGridPosition()).setCharacter(characters.back());
	}

	template<typename T>
	void registerItem(T t)
	{
		items.emplace_back(std::make_shared<T>(t.getGridPosition()));
		getOneGridData(t.getGridPosition()).setItem(items.back());
	}

	GridData& getOneGridData(int x, int y)
	{
		if ((x < 0 || x >= (int)mapGrid.height) || (y < 0 || y >= (int)mapGrid.width))
			return outsideGrid;
		else
			return mapGrid[x][y];
	}
	GridData& getOneGridData(Point p)
	{
		const int x = p.x;
		const int y = p.y;
		if ((x < 0 || x >= (int)mapGrid.height) || (y < 0 || y >= (int)mapGrid.width))
			return outsideGrid;
		else
			return mapGrid[x][y];
	}

	void setCenterPoint(Point p) { centerGrid = p; }
	void setDrawRange(Size s) { mainDrawRange = s; }
	void setGridSize(Size s) { mainGridSize = s; }
	Point getCenterPoint() { return centerGrid; }
	Size getDrawRange() { return mainDrawRange; }
	Size getMainGridSize() { return mainGridSize; }
	Size getSubGridSize() { return subGridSize; }

	Array<std::shared_ptr<Character>> getCharacterData() { return characters; }
	Array<std::shared_ptr<Item>> getItemData() { return items; }

	void setAllGridEnableDraw()
	{
		for (size_t y = 0; y < mapGrid.width; y++)
		{
			for (size_t x = 0; x < mapGrid.height; x++)
			{
				mapGrid[x][y].setEnableDraw(false);
			}
		}
	}

protected:
	MapData();
	virtual ~MapData() {}
private:

	Grid<GridData> mapGrid;
	GridData outsideGrid;
	Point centerGrid;
	Size mainDrawRange;
	Size mainGridSize; Point mainOrigin; Size mainDrawSize;;
	Size subGridSize; Point subOrigin; Size subDrawSize;
	Array<std::shared_ptr<Character>> characters;
	Array<std::shared_ptr<Item>> items;

	Stopwatch updateTimer;
};
#include <Siv3D.hpp>
#include <memory>
#include <deque>

//ログの設定
const int logWidth = 600;
struct LogData
{
	LogData(String s, int i):
		str(s),
		icon(i)
	{}

	String str = L"There is not data.";
	int icon;
};
struct IconData
{
	IconData(FilePath path, String word, Color c):
		img(Texture(path)),
		keyword(word),
		size(img.size),
		color(c)
	{}

	Texture img;
	Size size;
	String keyword;
	Color color;
};
class LogSystem
{
public:
	static LogSystem& getInstance()
	{
		static LogSystem logInstance;
		return logInstance;
	}

	void displayLog();
	void addLog(String);
	void setOrigin(Point p) { origin = p; }
protected:
	LogSystem();
	virtual ~LogSystem() {}
private:
	//void operator = (const logSystem& obj) {}
	//logSystem(const logSystem &obj) {}

	const Font font;
	Point origin;
	int maxLogSize;
	std::deque<LogData> log;
	Array<IconData> icon;
};
void LogSystem::displayLog()
{
	const int gap = 10;
	int h = 0;
	for (int i = 0; i < maxLogSize; i++)
	{
		if (i >= (int)log.size())
			break;

		const int nexti = i + 1;
		if (log.at(i).icon == -1)
		{
			h += font.height + gap;
			font(log.at(i).str).draw(origin + Point(gap, h - gap - font.ascent));
		}
		else
		{
			icon[log.at(i).icon].img.resize(icon[log.at(i).icon].size)
				.draw(origin + Point(gap, h + gap)).drawFrame(0, 2, icon[log.at(i).icon].color);

			h += icon[log.at(i).icon].size.y + gap + 2;

			font(log.at(i).str)
				.draw(origin + Point(gap * 2 + icon[log.at(i).icon].size.x, h - gap - font.ascent));
		}
		Line(origin.x, h, Window::Width(), h).draw(Palette::Gold);
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
LogSystem::LogSystem() :
	font(20),
	maxLogSize(20),
	origin(Window::Width(), 0)
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

//マップグリッドとXY座標の相互変換
const Size gridSize = Size::One * 60;
Point GridtoXY(Point);
Point GridtoCenterXY(Point);
Point XYtoGrid(Vec2);
Point GridtoXY(int, int);
Point GridtoCenterXY(int, int);
Point XYtoGrid(double, double);

//一つのグリッドの情報
struct GridData
{
public:
	GridData() :
		terrain(-1),
		enableInvade(false),
		enableDraw(false),
		charaOn(false),
		itemOn(false)
	{
		//resetCharacter();
		//resetItem();
	}

	void setTerrain(int t)
	{
		terrain = t;
		if (terrain == 1)
			enableInvade = true;
	}
	void setEnableDraw(bool b) { enableDraw = b; }
	void setChara(bool b) { charaOn = b; }
	void setItem(bool b) { itemOn = b; }

	int getTerrain() { return terrain; }
	bool canBeInvade() { return enableInvade && !charaOn; }
	bool canBeDraw() { return enableDraw; }
	bool CharaOn() { return charaOn; }
	bool ItemOn() { return itemOn; }
	/*
	void setCharacter(Character* c)
	{
	character = std::make_shared<Character>(c);
	charaOn = true;
	LogSystem::getInstance().addLog(c->getName() + L"が登録されました。");
	}

	void setItem(Item i)
	{
	item = std::make_shared<Item>(i);
	itemOn = true;
	LogSystem::getInstance().addLog(L"アイテムが登録されました。");
	}

	void resetCharacter()
	{
	character = NULL;
	charaOn = false;
	}

	void resetItem()
	{
	item = NULL;
	itemOn = false;
	}
	*/

private:
	int terrain;
	bool enableInvade;
	bool enableDraw;

	bool charaOn;
	bool itemOn;
	/*
	std::shared_ptr<Character> character;
	std::shared_ptr<Item> item;
	*/
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

	void loadMap();
	void setCenterPoint(Point p) { centerGrid = p; }
	Grid<GridData>& getAllGridData() { return mapGrid; }
	GridData& getOneGridData(int x, int y) 
	{
		if ((x < 0 || x >= mapGrid.height) || (y < 0 || y >= mapGrid.width))
			return defaultGridData;
		else
			return mapGrid[x][y];
	}
	GridData& getOneGridData(Point p) { return getOneGridData(p.x, p.y); }
	Point getCenterPoint() { return centerGrid; }

	void setAllGridEnableDraw(Size range)
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
	GridData defaultGridData;
	Point centerGrid;
};
MapData::MapData()
{
	defaultGridData = GridData();
	centerGrid = Point(0, 0);
}
void MapData::loadMap()
{
	const CSVReader reader(L"Csvs/data.csv");

	if (!reader)
		return;

	const size_t width = reader.columns(0);
	const size_t height = reader.rows;

	mapGrid = Grid<GridData>(height, width);

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			mapGrid[x][y].setTerrain(reader.getOr(y, x, -1));
		}
	}
	LogSystem::getInstance().addLog(L"Map has been loaded.");
}

//キャラクター
class Character
{
public:
	Character(Point);
	Character(int, int);
	void draw(Size);
	bool move();
	void act();

	void setPos(Point p) { position = GridtoCenterXY(p); }
	void setRad(int d) { direction = d; }
	Point getPos() { return XYtoGrid(position); }
	double getRad() { return Radians(direction); }

	String getName() { return name; }

protected:
	Texture img;
	Point position;
	Color color;
	String name;
	int direction;
};
Character::Character(Point pos)
{
	position = GridtoCenterXY(pos);
	direction = 0;
	MapData::getInstance().getOneGridData(XYtoGrid(position)).setChara(true);
}
Character::Character(int x, int y) :Character(Point(x, y)) {}
void Character::draw(Size range)
{
	if (MapData::getInstance().getOneGridData(XYtoGrid(position)).canBeDraw())
		Rect(GridtoXY(XYtoGrid(position) - MapData::getInstance().getCenterPoint() + range / 2), gridSize)(img).draw().drawFrame(1, 1, color);
}
bool Character::move()
{
	Grid<GridData> mapgrid = MapData::getInstance().getAllGridData();

	Point grid = XYtoGrid(position);
	const Point formerGrid = grid;

	if (mapgrid[grid.x + 1][grid.y].canBeInvade() && mapgrid[grid.x - 1][grid.y].canBeInvade())
	{
		if (Random() < 0.5)
			grid.x += 1;
		else
			grid.x -= 1;
	}
	else if (mapgrid[grid.x + 1][grid.y].canBeInvade())
		grid.x += 1;
	else if (mapgrid[grid.x - 1][grid.y].canBeInvade())
		grid.x -= 1;

	position = GridtoCenterXY(grid);

	MapData::getInstance().getOneGridData(formerGrid).setChara(false);
	MapData::getInstance().getOneGridData(grid).setChara(true);

	LogSystem::getInstance().addLog(L"<shima3>" + ToString(grid.x) + L" , " + ToString(grid.y) + L"に移動しました。");
	return true;
}
void Character::act()
{
}

class Player :public Character
{
public:
	Player(Point pos) :Character(pos)
	{
		img = Texture((L"Images/image.png"));
		img.resize(gridSize);
		color = Palette::Dodgerblue;
		name = L"Player";
		MapData::getInstance().setCenterPoint(XYtoGrid(position));
	}
	Player(int x, int y) :Player(Point(x, y)) {}

	bool move();
};
bool Player::move()
{
	bool moved = false;

	Grid<GridData> mapgrid = MapData::getInstance().getAllGridData();

	Point grid = XYtoGrid(position);
	const Point formerGrid = grid;

	if (Input::KeySpace.clicked)
	{
		LogSystem::getInstance().addLog(L"<shima1>うい");
		MapData::getInstance().setCenterPoint(grid);
	}

	if (Input::KeyUp.clicked)
	{
		if (mapgrid[grid.x][grid.y - 1].canBeInvade())
			grid.y -= 1;
		direction = 270;
	}
	if (Input::KeyDown.clicked)
	{
		if (mapgrid[grid.x][grid.y + 1].canBeInvade())
			grid.y += 1;
		direction = 90;
	}

	if (Input::KeyLeft.clicked)
	{
		if (mapgrid[grid.x - 1][grid.y].canBeInvade())
			grid.x -= 1;
		direction = 180;
	}
	if (Input::KeyRight.clicked)
	{
		if (mapgrid[grid.x + 1][grid.y].canBeInvade())
			grid.x += 1;
		direction = 0;
	}

	position = GridtoCenterXY(grid);
	if (grid != formerGrid)
	{
		MapData::getInstance().getOneGridData(formerGrid).setChara(false);
		MapData::getInstance().getOneGridData(grid).setChara(true);

		LogSystem::getInstance().addLog(L"<nade1>" + ToString(grid.x) + L" , " + ToString(grid.y) + L"に移動しました。");
		moved = true;

		if (MapData::getInstance().getOneGridData(grid).ItemOn())
			LogSystem::getInstance().addLog(L"<nade2>何か踏んだ?");
	}
	return moved;
}

class Enemy :public Character
{
public:
	Enemy(Point pos) :Character(pos)
	{
		img = Texture((L"Images/enemy.png"));
		img.resize(gridSize);
		color = Palette::Tomato;
		name = L"Enemy";
	}
	Enemy(int x, int y) :Enemy(Point(x, y)) {}
};

//アイテム
class Item
{
public:
	Item(Point);
	Item(int, int);
	void draw(Size);
	String getName() { return name; }
protected:
	Texture img;
	Point position;
	Color color;
	String name;
};
Item::Item(Point pos)
{
	position = GridtoCenterXY(pos);
	color = Palette::Palegreen;
	MapData::getInstance().getOneGridData(XYtoGrid(position)).setItem(true);
}
Item::Item(int x, int y) :Item(Point(x, y)) {}
void Item::draw(Size range)
{
	if (MapData::getInstance().getOneGridData(XYtoGrid(position)).canBeDraw())
		Rect(GridtoXY(XYtoGrid(position) - MapData::getInstance().getCenterPoint() + range / 2), gridSize)(img).draw().drawFrame(1, 1, color);
}

class Glasses :public Item
{
public:
	Glasses(Point pos) :Item(pos)
	{
		img = Texture((L"Images/glasses.png"));
		img.resize(gridSize);
		name = L"Ogaki's glasses";
	}
	Glasses(int x, int y) :Glasses(Point(x, y)) {}

	String getName() { return L"Player"; }
};

//地形を描く
void drawGroundImage(Size);
void drawOneGrid(Point, Size, int);

void Main()
{
	MapData::getInstance().loadMap();
	Size drawRange(11, 9);
	Size playerDrawPosition((int)(drawRange.x / 2.0), (int)(drawRange.y / 2.0));
	Window::Resize(GridtoXY(drawRange) + Point(logWidth, 0));

	LogSystem::getInstance().setOrigin(Point(Window::Width() - logWidth, 0));

	Player player(5, 5);
	Enemy enemy(4, 3);
	Glasses glasses(5, 4);

	while (System::Update())
	{
		player.move();

		if (Input::MouseL.clicked)
			MapData::getInstance().setCenterPoint(XYtoGrid(Mouse::Pos())
				+ MapData::getInstance().getCenterPoint() - drawRange / 2);

		MapData::getInstance().setAllGridEnableDraw(drawRange);
		drawGroundImage(drawRange);

		glasses.draw(drawRange);
		enemy.draw(drawRange);
		player.draw(drawRange);

		LogSystem::getInstance().displayLog();
	}
}

Point GridtoXY(Point p)
{
	return Point(p.x*gridSize.x, p.y*gridSize.y);
}
Point GridtoCenterXY(Point p)
{
	return Point(p.x*gridSize.x + gridSize.x / 2, p.y*gridSize.y + gridSize.y / 2);
}
Point XYtoGrid(Vec2 p)
{
	return Point((int)(p.x / gridSize.x), (int)(p.y / gridSize.y));
}
Point GridtoXY(int x, int y)
{
	return Point(x*gridSize.x, y*gridSize.y);
}
Point GridtoCenterXY(int x, int y)
{
	return Point(x*gridSize.x + gridSize.x / 2, y*gridSize.y + gridSize.y / 2);
}
Point XYtoGrid(double x, double y)
{
	return Point((int)(x / gridSize.x), (int)(y / gridSize.y));
}

void drawGroundImage(Size range)
{
	const Point startingPos = MapData::getInstance().getCenterPoint() - range / 2;
	const Point endPos = startingPos + range;

	for (int y = startingPos.y; y < endPos.y; y++)
	{
		for (int x = startingPos.x; x < endPos.x; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGrid(GridtoXY(x - startingPos.x, y - startingPos.y), gridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).CharaOn())
				k = 10;
			if (MapData::getInstance().getOneGridData(x, y).ItemOn())
				k = 20;
			if (MapData::getInstance().getOneGridData(x, y).CharaOn() && MapData::getInstance().getOneGridData(x, y).ItemOn())
				k = 30;

			drawOneGrid(GridtoXY(x - startingPos.x, y - startingPos.y), gridSize, k);
		}
	}

	/*
	for (int y = startingPos.y; y < endPos.y; y++)
	{
	for (int x = startingPos.x; x < endPos.x; x++)
	{
	if ((x < 0 || x >(int)mapgrid.height - 1) || (y < 0 || y >(int)mapgrid.width - 1))
	continue;

	if (mapgrid[x][y].charaOn)
	{
	mapgrid[x][y].character->draw(range);
	}
	}
	}
	*/
}
void drawOneGrid(Point p, Size s, int k)
{
	switch (k)
	{
	case 0:
		Rect(p, s).draw(Palette::Darkgray).drawFrame(1, 0, Palette::Black);
		break;
	case 1:
		Rect(p, s).draw(Palette::Sandybrown).drawFrame(1, 0, Palette::Sienna);
		break;

	case 10:
		Rect(p, s).draw(Color(Palette::Aqua.r, Palette::Aqua.g, Palette::Aqua.b, 150)).drawFrame(1, 0, Palette::Black);
		break;
	case 20:
		Rect(p, s).draw(Color(Palette::Tomato.r, Palette::Tomato.g, Palette::Tomato.b, 150)).drawFrame(1, 0, Palette::Black);
		break;
	case 30:
		Rect(p, s).draw(Color(Palette::Mediumorchid.r, Palette::Mediumorchid.g, Palette::Mediumorchid.b, 150)).drawFrame(1, 0, Palette::Black);
		break;

	default:
		Rect(p, s).draw(Palette::Dimgray).drawFrame(1, 0, Palette::Black);
		break;
	}

}
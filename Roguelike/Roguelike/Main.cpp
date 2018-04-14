#include <Siv3D.hpp>
#include <memory>
#include <deque>
#include <typeinfo>

//ログの設定
struct LogData
{
	LogData(String s = L"There is not data.", int i = -1):
		str(s),
		icon(i)
	{}

	String str;
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

	void setOrigin(Point p) { logOrigin = p; }
	void setSize(Size s) { logSize = s; }
	const Size getSize() { return logSize; }
protected:
	LogSystem();
	virtual ~LogSystem() {}
private:
	//void operator = (const logSystem& obj) {}
	//logSystem(const logSystem &obj) {}

	int maxLogSize;
	Point logOrigin;
	Size logSize;
	std::deque<LogData> log;
	Array<IconData> icon;
};
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

//マップグリッドとXY座標の相互変換
Point GridtoXY(Point);
Point GridtoCenterXY(Point);
Point XYtoGrid(Vec2);
Point GridtoXY(int, int);
Point SubGridtoXY(int, int);
Point GridtoCenterXY(int, int);
Point XYtoGrid(double, double);
Point getMouseGrid();

//キャラクター
class Character
{
public:
	Character(Point);
	Character(int, int);
	virtual void draw();
	virtual bool move();
	virtual bool attack();
	virtual bool enableLive() { return HP > 0; }
	virtual void doSomethingAtDeath();

	void setGridPosition(Point p) { gridPosition = p; }
	void setRad(int d) { direction = d; }
	Point getGridPosition() { return gridPosition; }
	double getRad() { return Radians(direction); }

	String getName() { return name; }
	int causeDamage(int damage) { return HP = damage > HP ? 0 : HP - damage; }

	//void setHP(int h) { HP = h; }
	//void setATK(int a) { ATK = a; }
	//void setDEF(int d) { DEF = d; }
	int getHP() { return HP; }
	int getATK() { return ATK; }
	int getDEF() { return DEF; }

protected:
	Texture img;
	Vec2 xyPosition;
	Point gridPosition;
	Color color;
	String name;
	int direction;

	int HP, ATK, DEF;
};
class Player :public Character
{
public:
	Player(Point pos);
	Player(int x, int y) :Player(Point(x, y)) {}

	bool move()override;
	bool attack()override;
};
//アイテム
class Item
{
public:
	Item(Point);
	Item(int, int);
	virtual void draw();
	virtual bool enableLive() { return false; }
	virtual void doSomethingAtDeath();

	void setGridPosition(Point p) { gridPosition = p; }
	Point getGridPosition() { return gridPosition; }
	String getName() { return name; }
protected:
	Texture img;
	Vec2 xyPosition;
	Point gridPosition;
	Color color;
	String name;
};

//一つのグリッドの情報
struct GridData
{
public:
	GridData() :
		terrain(-1),
		enableInvade(false),
		enableDraw(false),
		underCharacter(false),
		underItem(false)
	{}

	void setTerrain(int t)
	{
		terrain = t;
		if (terrain == 1)
			enableInvade = true;
	}
	void setEnableDraw(bool b) { enableDraw = b; }

	int getTerrain() { return terrain; }
	bool enableAddCharacter() { return enableInvade && !underCharacter; }
	bool enableAddItem() { return enableInvade && !underItem; }
	bool canBeDraw() { return enableDraw; }
	bool isUnderCharacter() { return underCharacter; }
	bool isUnderItem() { return underItem; }

	int terrain;
	bool enableInvade;
	bool enableDraw;
	bool underCharacter;
	bool underItem;
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
	void drawMainMap();
	void drawSubMap();
	void deletecCharacter();

	void loadMap();
	void drawOneGridGround(Point, Size, int);

	template<typename T>
	void registerCharacter(T t) { characters.emplace_back(std::make_unique<T>(t.getGridPosition())); }
	template<typename T>
	void registerItem(T t) { items.emplace_back(std::make_unique<T>(t.getGridPosition())); }

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

	Character* getCharacterPointer(int x, int y)
	{
		for (auto& e : characters)
		{
			if (e->getGridPosition() == Point(x, y))
				return e.get();
		}
		return nullptr;
	}
	Character* getCharacterPointer(Point p)
	{
		for (auto& e : characters)
		{
			if (e->getGridPosition() == p)
				return e.get();
		}
		return nullptr;
	}

	Item* getItemPointer(int x, int y)
	{
		for (auto& e : items)
		{
			if (e->getGridPosition() == Point(x, y))
				return e.get();
		}
		return nullptr;
	}
	Item* getItemPointer(Point p)
	{
		for (auto& e : items)
		{
			if (e->getGridPosition() == p)
				return e.get();
		}
		return nullptr;
	}

	void setCenterPoint(Point p) { centerGrid = p; }
	void setDrawRange(Size s) { mainDrawRange = s; }
	void setGridSize(Size s) { mainGridSize = s; }
	Point getCenterPoint() { return centerGrid; }
	Size getDrawRange() { return mainDrawRange; }
	Size getMainGridSize() { return mainGridSize; }
	Size getSubGridSize() { return subGridSize; }

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
	Array<std::unique_ptr<Character>> characters;
	Array<std::unique_ptr<Item>> items;

	Stopwatch updateTimer;
};
MapData::MapData()
{
	outsideGrid = GridData();
	centerGrid = Point(0, 0);
	mainDrawRange = Size(7, 7);
	mainGridSize = Size(80, 80);
	mainOrigin = Point(0, 0);
	mainDrawSize = mainDrawRange*mainGridSize;
	subOrigin = Point(mainDrawSize.x, 0) + Point::One * 20;
	subDrawSize = mainDrawSize - Size::One * 40;

	updateTimer.start();
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

	subGridSize = subDrawSize / std::max((int)mapGrid.height, (int)mapGrid.width);
}
void MapData::update()
{
	for (auto& e : characters)
	{
		if (typeid(*e) == typeid(Player) || updateTimer.s() > 3)
		{
			e->move();
			e->attack();
		}
	}
	if (updateTimer.s() > 3)
		updateTimer.restart();
}
void MapData::drawMainMap()
{
	Transformer2D transformer(Mat3x2::Translate(mainOrigin), false);
	setAllGridEnableDraw();
	const Size range = MapData::getInstance().getDrawRange();
	const Point startingPos = MapData::getInstance().getCenterPoint() - range / 2;
	const Point endPos = startingPos + range;

	for (int y = startingPos.y; y < endPos.y; y++)
	{
		for (int x = startingPos.x; x < endPos.x; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGridGround(GridtoXY(x - startingPos.x, y - startingPos.y), mainGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
			{
				if (typeid(*getCharacterPointer(x, y)) == typeid(Player))
					k = 10;
				else
					k = 20;
			}
			else if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 30;
			drawOneGridGround(GridtoXY(x - startingPos.x, y - startingPos.y), mainGridSize, k);
		}
	}

	for (size_t i = 0; i < items.size(); i++)
	{
		if (MapData::getInstance().getOneGridData(items[i]->getGridPosition()).canBeDraw())
			items[i]->draw();
	}
	for (size_t i = 0; i < characters.size(); i++)
	{
		if (MapData::getInstance().getOneGridData(characters[i]->getGridPosition()).canBeDraw())
			characters[i]->draw();
	}

	if (Input::Key0.clicked)
		ClearPrint();
}
void MapData::drawSubMap()
{
	Transformer2D transformer(Mat3x2::Translate(subOrigin), false);
	for (size_t y = 0; y < mapGrid.width; y++)
	{
		for (size_t x = 0; x < mapGrid.height; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGridGround(SubGridtoXY(x, y), subGridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
			{
				if (typeid(*getCharacterPointer(x, y)) == typeid(Player))
					k = 100;
				else
					k = 200;
			}
			else if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 300;
			drawOneGridGround(SubGridtoXY(x, y), subGridSize, k);
		}
	}
}
void MapData::deletecCharacter()
{
	for (size_t i = 0; i < characters.size(); i++)
	{
		if (!characters[i]->enableLive())
		{
			getOneGridData(characters[i]->getGridPosition()).underCharacter = false;
			characters[i]->doSomethingAtDeath();
			characters.erase(characters.begin() + i);
			i--;
		}
	}
}
void MapData::drawOneGridGround(Point p, Size s, int k)
{
	switch (k)
	{
	case 0:
		Rect(p, s).draw(Palette::Darkgray).drawFrame(1, 0, Palette::Black);
		break;
	case 1:
		Rect(p, s).draw(Palette::Sandybrown).drawFrame(1, 0, Palette::Sienna);
		break;

	//mainMap
	case 10:
		Rect(p, s).draw(Color(Palette::Aqua, 100)).drawFrame(1, 0, Palette::Black);
		break;
	case 20:
		Rect(p, s).draw(Color(Palette::Tomato, 100)).drawFrame(1, 0, Palette::Black); 
		break;
	case 30:
		Rect(p, s).draw(Color(Palette::Mediumspringgreen, 100)).drawFrame(1, 0, Palette::Black);
		break;

	//subMap
	case 100:
		Rect(p, s).draw(Color(Palette::Aqua, 200)).drawFrame(1, 0, Palette::Black);
		break;
	case 200:
		Rect(p, s).draw(Color(Palette::Tomato, 200)).drawFrame(1, 0, Palette::Black);
		break;
	case 300:
		Rect(p, s).draw(Color(Palette::Mediumspringgreen, 200)).drawFrame(1, 0, Palette::Black);
		break;

	default:
		Rect(p, s).draw(Palette::Dimgray).drawFrame(1, 0, Palette::Black);
		break;
	}

}


//キャラクター
Character::Character(Point pos)
{
	gridPosition = pos;
	xyPosition = GridtoCenterXY(pos);
	direction = 0;
	MapData::getInstance().getOneGridData(pos).underCharacter = true;
}
Character::Character(int x, int y) :Character(Point(x, y)) {}
void Character::draw()
{
	if (!MapData::getInstance().getOneGridData(gridPosition).canBeDraw())
		return;

	const Vec2 drawPosition = GridtoXY(gridPosition - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2);
	Rect(Point(drawPosition.x, drawPosition.y), MapData::getInstance().getMainGridSize())(img).draw().drawFrame(1, 1, color);
	Circle(GridtoCenterXY(XYtoGrid(drawPosition)) + Vec2(MapData::getInstance().getMainGridSize().x / 2 * cos(Radians(direction)), MapData::getInstance().getMainGridSize().x / 2 * sin(Radians(direction))), 4).draw(Palette::Black);
	
	FontAsset(L"statusFont")(L"HP " + ToString(HP)).draw(drawPosition + Point(5, MapData::getInstance().getMainGridSize().y / 3 * 0), Palette::Black);
	//FontAsset(L"statusFont")(L"ATK " + ToString(ATK)).draw(drawPosition + Point(5, gridSize.y / 3 * 1), Palette::Black);
	//FontAsset(L"statusFont")(L"DEF " + ToString(DEF)).draw(drawPosition + Point(5, gridSize.y / 3 * 2), Palette::Black);
}
bool Character::move()
{
	return false;
}
bool Character::attack()
{
	return false;
}
void Character::doSomethingAtDeath()
{
	LogSystem::getInstance().addLog(name + L"を倒しました。");
}

//プレイヤー
Player::Player(Point pos) :Character(pos)
{
	img = Texture(L"Images/image.png");
	color = Palette::Dodgerblue;
	name = L"Player";
	MapData::getInstance().setCenterPoint(gridPosition);

	HP = 100;
	ATK = 80;
	DEF = 60;
}
bool Player::move()
{
	const Point formerGrid = gridPosition;
	const int formerDirection = direction;

	if (Input::KeySpace.clicked)
	{
		LogSystem::getInstance().addLog(L"<shima1>うい");
		MapData::getInstance().setCenterPoint(gridPosition);
	}

	bool keyInput = true;
	if (Input::KeyUp.clicked)
		direction = 270;
	else if (Input::KeyDown.clicked)
		direction = 90;
	else if (Input::KeyLeft.clicked)
		direction = 180;
	else if (Input::KeyRight.clicked)
		direction = 0;
	else
		keyInput = false;

	if (Input::KeyShift.pressed || !keyInput)
		return false;

	Point p(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(gridPosition + p).enableAddCharacter())
		gridPosition += p;
	xyPosition = GridtoCenterXY(gridPosition);

	if (gridPosition != formerGrid || direction != formerDirection)
	{
		MapData::getInstance().getOneGridData(formerGrid).underCharacter = false;
		MapData::getInstance().getOneGridData(gridPosition).underCharacter = true;
		MapData::getInstance().setCenterPoint(gridPosition);

		if (MapData::getInstance().getOneGridData(gridPosition).isUnderItem())
			LogSystem::getInstance().addLog(MapData::getInstance().getItemPointer(gridPosition)->getName() + L"を踏んでしまった。");

		return true;
	}
	return false;
}
bool Player::attack()
{
	if (!Input::KeyEnter.clicked)
		return false;

	const Point frontOfMe = gridPosition + Point(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
	{
		const int damage = ATK - MapData::getInstance().getCharacterPointer(frontOfMe)->getDEF();
		MapData::getInstance().getCharacterPointer(frontOfMe)->causeDamage(damage);
		LogSystem::getInstance().addLog(name + L"は" + MapData::getInstance().getCharacterPointer(frontOfMe)->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
	}
	return true;
}

//エネミー
class Enemy :public Character
{
public:
	Enemy(Point pos) :Character(pos)
	{
		color = Palette::Tomato;
		name = L"Enemy";
	
		HP = 100;
		ATK = 10;
		DEF = 70;
	}
	Enemy(int x, int y) :Enemy(Point(x, y)) {}
};

class Sandbag :public Enemy
{
public:
	Sandbag(Point pos) :Enemy(pos)
	{
		img = Texture((L"Images/sandbag.png"));
		name = L"Sandbag";

		HP = 900;
		ATK = 0;
		DEF = 20;
	}
	Sandbag(int x, int y) :Sandbag(Point(x, y)) {}

	bool move()override;
};
bool Sandbag::move()
{
	const Point formerGrid = gridPosition;
	const int formerDirection = direction;

	direction = int(Random() * 4) + 1;
	direction *= 90;

	Point p(cos(Radians(direction)), sin(Radians(direction)));
	if (MapData::getInstance().getOneGridData(gridPosition + p).enableAddCharacter())
		gridPosition += p;
	xyPosition = GridtoCenterXY(gridPosition);

	if (gridPosition != formerGrid || direction != formerDirection)
	{
		MapData::getInstance().getOneGridData(formerGrid).underCharacter = false;
		MapData::getInstance().getOneGridData(gridPosition).underCharacter = true;
		return true;
	}

	return false;
}

class Kyonshih :public Enemy
{
public:
	Kyonshih(Point pos) :Enemy(pos)
	{
		img = Texture((L"Images/pop.png"));
		name = L"Kyonshih";

		HP = 200;
		ATK = 40;
		DEF = 40;
	}
	Kyonshih(int x, int y) :Kyonshih(Point(x, y)) {}

	bool attack()override;
};
bool Kyonshih::attack()
{
	for (int i = 0; i < 4; i++)
	{
		const Point frontOfMe = gridPosition + Point(cos(Radians(i * 90)), sin(Radians(i * 90)));
		if (MapData::getInstance().getOneGridData(frontOfMe).isUnderCharacter())
		{
			if (typeid(*MapData::getInstance().getCharacterPointer(frontOfMe)) == typeid(Player))
			{
				const int damage = 20;
				MapData::getInstance().getCharacterPointer(frontOfMe)->causeDamage(damage);
				LogSystem::getInstance().addLog(name + L"は" + MapData::getInstance().getCharacterPointer(frontOfMe)->getName() + L"に" + ToString(damage) + L"ダメージ与えた。");
				direction = i * 90;
				return true;
			}
		}
	}
	return false;
}

//アイテム
Item::Item(Point pos)
{
	gridPosition = pos;
	xyPosition = GridtoCenterXY(pos);
	color = Palette::Palegreen;
	MapData::getInstance().getOneGridData(pos).underItem = true;
}
Item::Item(int x, int y) :Item(Point(x, y)) {}
void Item::draw()
{
	if (MapData::getInstance().getOneGridData(gridPosition).canBeDraw())
		Rect(GridtoXY(gridPosition - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2), MapData::getInstance().getMainGridSize())(img).draw().drawFrame(1, 1, color);
}
void Item::doSomethingAtDeath()
{
	LogSystem::getInstance().addLog(name + L"を倒しました。");
}

class Glasses :public Item
{
public:
	Glasses(Point pos) :Item(pos)
	{
		img = Texture((L"Images/glasses.png"));
		name = L"Ogaki's glasses";
	}
	Glasses(int x, int y) :Glasses(Point(x, y)) {}
};

void Main()
{
	MapData::getInstance().loadMap();
	Window::Resize(GridtoXY(MapData::getInstance().getDrawRange().x * 2, MapData::getInstance().getDrawRange().y) + Point(0, LogSystem::getInstance().getSize().y));
	LogSystem::getInstance().setOrigin(Point(0, Window::Height() - LogSystem::getInstance().getSize().y));
	LogSystem::getInstance().setSize(Size(Window::Width(), LogSystem::getInstance().getSize().y));

	FontAsset::Register(L"statusFont", int(MapData::getInstance().getMainGridSize().x / 6.0), Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);

	MapData::getInstance().registerCharacter(Player(5, 5));
	MapData::getInstance().registerCharacter(Kyonshih(6, 7));
	MapData::getInstance().registerItem(Glasses(5, 4));

	LogSystem::getInstance().addLog(L"Click a grid to create an enemy.");
	while (System::Update())
	{
		MapData::getInstance().update();
		MapData::getInstance().drawSubMap();
		MapData::getInstance().drawMainMap();
		MapData::getInstance().deletecCharacter();


		if (Input::MouseL.clicked && MapData::getInstance().getOneGridData(getMouseGrid()).enableAddCharacter())
			MapData::getInstance().registerCharacter(Sandbag(getMouseGrid()));
		if (Input::MouseR.clicked && MapData::getInstance().getOneGridData(getMouseGrid()).enableAddItem())
			MapData::getInstance().registerCharacter(Kyonshih(getMouseGrid()));

		LogSystem::getInstance().displayLog();
	}
}

Point GridtoXY(Point p)
{
	return Point(p.x*MapData::getInstance().getMainGridSize().x, p.y*MapData::getInstance().getMainGridSize().y);
}
Point GridtoCenterXY(Point p)
{
	return Point(p.x*MapData::getInstance().getMainGridSize().x + MapData::getInstance().getMainGridSize().x / 2, p.y*MapData::getInstance().getMainGridSize().y + MapData::getInstance().getMainGridSize().y / 2);
}
Point XYtoGrid(Vec2 p)
{
	return Point((int)(p.x / MapData::getInstance().getMainGridSize().x), (int)(p.y / MapData::getInstance().getMainGridSize().y));
}
Point GridtoXY(int x, int y)
{
	return Point(x*MapData::getInstance().getMainGridSize().x, y*MapData::getInstance().getMainGridSize().y);
}
Point SubGridtoXY(int x, int y)
{
	return Point(x*MapData::getInstance().getSubGridSize().x, y*MapData::getInstance().getSubGridSize().y);
}
Point GridtoCenterXY(int x, int y)
{
	return Point(x*MapData::getInstance().getMainGridSize().x + MapData::getInstance().getMainGridSize().x / 2, y*MapData::getInstance().getMainGridSize().y + MapData::getInstance().getMainGridSize().y / 2);
}
Point XYtoGrid(double x, double y)
{
	return Point((int)(x / MapData::getInstance().getMainGridSize().x), (int)(y / MapData::getInstance().getMainGridSize().y));
}
Point getMouseGrid() { return XYtoGrid(Mouse::Pos()) + MapData::getInstance().getCenterPoint() - MapData::getInstance().getDrawRange() / 2; }
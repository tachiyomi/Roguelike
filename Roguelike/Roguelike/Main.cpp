#include <Siv3D.hpp>
#include <memory>
#include <deque>

//ログの設定
const int logWidth = 500;
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
	void setOrigin(Point p) { origin = p; }
protected:
	LogSystem();
	virtual ~LogSystem() {}
private:
	//void operator = (const logSystem& obj) {}
	//logSystem(const logSystem &obj) {}

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
			h += FontAsset(L"logFont").height + gap;
			FontAsset(L"logFont")(log.at(i).str).draw(origin + Point(gap, h - gap - FontAsset(L"logFont").ascent));
		}
		else
		{
			icon[log.at(i).icon].img.resize(icon[log.at(i).icon].size)
				.draw(origin + Point(gap, h + gap)).drawFrame(0, 2, icon[log.at(i).icon].color);

			h += icon[log.at(i).icon].size.y + gap + 2;

			FontAsset(L"logFont")(log.at(i).str)
				.draw(origin + Point(gap * 2 + icon[log.at(i).icon].size.x, h - gap - FontAsset(L"logFont").ascent));
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
const Size gridSize = Size::One * 100;
Point GridtoXY(Point);
Point GridtoCenterXY(Point);
Point XYtoGrid(Vec2);
Point GridtoXY(int, int);
Point GridtoCenterXY(int, int);
Point XYtoGrid(double, double);

//キャラクター
class Character
{
public:
	Character(Point);
	Character(int, int);
	void draw();

	bool move() { return true; }
	bool attack() { return true; }

	void setPos(Point p) { position = GridtoCenterXY(p); }
	void setRad(int d) { direction = d; }
	Point getPos() { return XYtoGrid(position); }
	double getRad() { return Radians(direction); }

	String getName() { return name; }
	int causeDamage(int damage) { return HP -= damage; }

	//void setHP(int h) { HP = h; }
	//void setATK(int a) { ATK = a; }
	//void setDEF(int d) { DEF = d; }
	int getHP() { return HP; }
	int getATK() { return ATK; }
	int getDEF() { return DEF; }

protected:
	Texture img;
	Point position;
	Color color;
	String name;
	int direction;

	int HP, ATK, DEF;
};

//アイテム
class Item
{
public:
	Item(Point);
	Item(int, int);
	void draw();
	String getName() { return name; }
protected:
	Texture img;
	Point position;
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
		enableDraw(false)
	{
		deleteCharacterPointer();
		deleteItemPointer();
	}

	void setTerrain(int t)
	{
		terrain = t;
		if (terrain == 1)
			enableInvade = true;
	}
	void setEnableDraw(bool b) { enableDraw = b; }

	int getTerrain() { return terrain; }
	bool canBeInvade() { return enableInvade && characterPointer == nullptr; }
	bool canBeDraw() { return enableDraw; }
	bool isUnderCharacter() { return characterPointer != nullptr; }
	bool isUnderItem() { return itemPointer != nullptr; }

	void setCharacterPointer(Character* c) { characterPointer = c; }
	void setItemPointer(Item* i) { itemPointer = i; }

	void deleteCharacterPointer() { characterPointer = nullptr; }
	void deleteItemPointer() { itemPointer = nullptr; }

	Character* getCharacterP() { return characterPointer; }
	Item* getItemP() { return itemPointer; }

private:
	int terrain;
	bool enableInvade;
	bool enableDraw;

	Character* characterPointer;
	Item* itemPointer;
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
		if ((x < 0 || x >= (int)mapGrid.height) || (y < 0 || y >= (int)mapGrid.width))
			return nullptr;
		else
			return mapGrid[x][y].getCharacterP();
	}
	Character* getCharacterPointer(Point p)
	{
		const int x = p.x;
		const int y = p.y;
		if ((x < 0 || x >= (int)mapGrid.height) || (y < 0 || y >= (int)mapGrid.width))
			return nullptr;
		else
			return mapGrid[x][y].getCharacterP();
	}

	Item* getItemPointer(int x, int y)
	{
		if ((x < 0 || x >= (int)mapGrid.height) || (y < 0 || y >= (int)mapGrid.width))
			return nullptr;
		else
			return mapGrid[x][y].getItemP();
	}
	Item* getItemPointer(Point p)
	{
		const int x = p.x;
		const int y = p.y;
		if ((x < 0 || x >= (int)mapGrid.height) || (y < 0 || y >= (int)mapGrid.width))
			return nullptr;
		else
			return mapGrid[x][y].getItemP();
	}

	Point getCenterPoint() { return centerGrid; }
	Size getDrawRange() { return drawRange; }

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

	void registerCharacter()
	{

	}

protected:
	MapData();
	virtual ~MapData() {}
private:
	Grid<GridData> mapGrid;
	GridData outsideGrid;
	Point centerGrid;
	Size drawRange;
	Array<Character> characters;
};
MapData::MapData()
{
	outsideGrid = GridData();
	centerGrid = Point(0, 0);
	drawRange = Size(7, 7);
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
Character::Character(Point pos)
{
	position = GridtoCenterXY(pos);
	direction = 0;
	MapData::getInstance().getOneGridData(XYtoGrid(position)).setCharacterPointer(this);
}
Character::Character(int x, int y) :Character(Point(x, y)) {}
void Character::draw()
{
	if (!MapData::getInstance().getOneGridData(XYtoGrid(position)).canBeDraw())
		return;
	
	const Point drawPosition = GridtoXY(XYtoGrid(position) - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2);

	Circle(GridtoCenterXY(XYtoGrid(drawPosition)) + Point(gridSize.x / 2 * cos(Radians(direction)), gridSize.x / 2 * sin(Radians(direction))), 6).draw(Palette::Black);
	Rect(drawPosition, gridSize)(img).draw().drawFrame(1, 1, color);
	
	FontAsset(L"statusFont")(L"HP " + ToString(HP)).draw(drawPosition + Point(5, gridSize.y / 3 * 0), Palette::Black);
	FontAsset(L"statusFont")(L"ATK " + ToString(ATK)).draw(drawPosition + Point(5, gridSize.y / 3 * 1), Palette::Black);
	FontAsset(L"statusFont")(L"DEF " + ToString(DEF)).draw(drawPosition + Point(5, gridSize.y / 3 * 2), Palette::Black);
}

//プレイヤー
class Player :public Character
{
public:
	Player(Point pos) :Character(pos)
	{
		img = Texture(L"Images/image.png");
		color = Palette::Dodgerblue;
		name = L"Player";
		MapData::getInstance().setCenterPoint(XYtoGrid(position));

		HP = 100;
		ATK = 80;
		DEF = 60;
	}
	Player(int x, int y) :Player(Point(x, y)) {}

	bool move();
	bool attack();
};
bool Player::move()
{
	Point grid = XYtoGrid(position);
	const Point formerGrid = grid;
	const int formerDirection = direction;

	if (Input::KeySpace.clicked)
	{
		LogSystem::getInstance().addLog(L"<shima1>うい");
		MapData::getInstance().setCenterPoint(grid);
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
	if (MapData::getInstance().getOneGridData(grid + p).canBeInvade())
		grid += p;
	position = GridtoCenterXY(grid);

	if (grid != formerGrid || direction != formerDirection)
	{
		MapData::getInstance().getOneGridData(formerGrid).deleteCharacterPointer();
		MapData::getInstance().getOneGridData(grid).setCharacterPointer(this);

		if (MapData::getInstance().getOneGridData(grid).getItemP())
			LogSystem::getInstance().addLog(MapData::getInstance().getItemPointer(grid)->getName() + L"を踏んでしまった。");

		return true;
	}
	return false;
}
bool Player::attack()
{
	if (!Input::KeyEnter.clicked)
		return false;

	const Point frontOfMe = XYtoGrid(position) + Point(cos(Radians(direction)), sin(Radians(direction)));

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

	bool move();
};
bool Enemy::move()
{
	bool moved = false;

	Grid<GridData> mapgrid = MapData::getInstance().getAllGridData();

	Point grid = XYtoGrid(position);
	const Point formerGrid = grid;

	if (mapgrid[grid.x + 1][grid.y].canBeInvade() && mapgrid[grid.x - 1][grid.y].canBeInvade())
	{
		if (Random() < 0.5)
		{
			grid.x += 1;
			direction = 0;
		}
		else
		{
			grid.x -= 1;
			direction = 180;
		}
	}
	else if (mapgrid[grid.x + 1][grid.y].canBeInvade())
		grid.x += 1;
	else if (mapgrid[grid.x - 1][grid.y].canBeInvade())
		grid.x -= 1;

	position = GridtoCenterXY(grid);

	if (grid != formerGrid)
	{
		MapData::getInstance().getOneGridData(formerGrid).deleteCharacterPointer();
		MapData::getInstance().getOneGridData(grid).setCharacterPointer(this);

		moved = true;
	}
	return moved;
}

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
};

//アイテム
Item::Item(Point pos)
{
	position = GridtoCenterXY(pos);
	color = Palette::Palegreen;
}
Item::Item(int x, int y) :Item(Point(x, y)) {}
void Item::draw()
{
	if (MapData::getInstance().getOneGridData(XYtoGrid(position)).canBeDraw())
		Rect(GridtoXY(XYtoGrid(position) - MapData::getInstance().getCenterPoint() + MapData::getInstance().getDrawRange() / 2), gridSize)(img).draw().drawFrame(1, 1, color);
}

class Glasses :public Item
{
public:
	Glasses(Point pos) :Item(pos)
	{
		img = Texture((L"Images/glasses.png"));
		img.resize(gridSize);
		name = L"Ogaki's glasses";
		MapData::getInstance().getOneGridData(XYtoGrid(position)).setItemPointer(this);
	}
	Glasses(int x, int y) :Glasses(Point(x, y)) {}
};

//地形を描く
void drawImage();
void drawOneGridGround(Point, Size, int);

void Main()
{
	MapData::getInstance().loadMap();
	Window::Resize(GridtoXY(MapData::getInstance().getDrawRange()) + Point(logWidth, 0));
	LogSystem::getInstance().setOrigin(Point(Window::Width() - logWidth, 0));

	FontAsset::Register(L"statusFont", 18, Typeface::Medium);
	FontAsset::Register(L"logFont", 12, Typeface::Bold);

	characters.emplace_back(Player(5, 5));
	//characters.emplace_back(Sandbag(4, 3));
	//Glasses glasses(5, 4);

	while (System::Update())
	{
		for (size_t i = 0; i < characters.size(); i++)
		{
			characters[i]->draw();
			characters[i].attack();
		}

		MapData::getInstance().setAllGridEnableDraw();
		drawImage();

		if (Input::MouseL.clicked && MapData::getInstance().getOneGridData(XYtoGrid(Mouse::Pos()) + MapData::getInstance().getCenterPoint() - MapData::getInstance().getDrawRange() / 2).canBeInvade())
			characters.emplace_back(Sandbag(XYtoGrid(Mouse::Pos()) + MapData::getInstance().getCenterPoint() - MapData::getInstance().getDrawRange() / 2));

		if (Input::MouseR.clicked && MapData::getInstance().getOneGridData(XYtoGrid(Mouse::Pos()) + MapData::getInstance().getCenterPoint() - MapData::getInstance().getDrawRange() / 2).isUnderCharacter())
			Println(MapData::getInstance().getCharacterPointer(XYtoGrid(Mouse::Pos()) + MapData::getInstance().getCenterPoint() - MapData::getInstance().getDrawRange() / 2));
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

void drawImage()
{
	const Size range = MapData::getInstance().getDrawRange();
	const Point startingPos = MapData::getInstance().getCenterPoint() - range / 2;
	const Point endPos = startingPos + range;

	for (int y = startingPos.y; y < endPos.y; y++)
	{
		for (int x = startingPos.x; x < endPos.x; x++)
		{
			int k = MapData::getInstance().getOneGridData(x, y).getTerrain();
			MapData::getInstance().getOneGridData(x, y).setEnableDraw(true);
			drawOneGridGround(GridtoXY(x - startingPos.x, y - startingPos.y), gridSize, k);

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter())
				k = 10;
			if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 20;
			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter() && MapData::getInstance().getOneGridData(x, y).isUnderItem())
				k = 30;

			drawOneGridGround(GridtoXY(x - startingPos.x, y - startingPos.y), gridSize, k);
		}
	}

	for (int y = startingPos.y; y < endPos.y; y++)
	{
		for (int x = startingPos.x; x < endPos.x; x++)
		{
			if (MapData::getInstance().getOneGridData(x, y).isUnderItem())
				MapData::getInstance().getItemPointer(x, y)->draw();

			if (MapData::getInstance().getOneGridData(x, y).isUnderCharacter()&&Input::Key3.clicked)
				MapData::getInstance().getCharacterPointer(x, y)->draw();
		}
	}

	if (Input::Key0.clicked)
		ClearPrint();
}
void drawOneGridGround(Point p, Size s, int k)
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
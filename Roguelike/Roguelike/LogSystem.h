#pragma once
#include <Siv3D.hpp>
#include <deque>

struct LogData
{
	LogData(String s = L"There is not data.", int i = -1) :
		str(s),
		icon(i)
	{}

	String str;
	int icon;
};
struct IconData
{
	IconData(FilePath path, String word, Color c) :
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
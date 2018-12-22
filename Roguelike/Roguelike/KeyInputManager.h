#pragma once
#include <Siv3D.hpp>

class KeyInputManager
{
public:
	static KeyInputManager& getInstance()
	{
		static KeyInputManager menuInstance;
		return menuInstance;
	}

	void renew();

	bool KeyUp, KeyDown, KeyRight, KeyLeft, KeyLeaningOnly, KeyNoAttack;
	bool KeyDesition, KeyBack, KeyMenu, KeyPause;

protected:
	KeyInputManager();
	virtual ~KeyInputManager() {}
private:
	void operator = (const KeyInputManager&) {}
	KeyInputManager(const KeyInputManager&) {}
};

#include "KeyInputManager.h"

void KeyInputManager::renew()
{
	KeyUp = (Input::KeyW.pressed || Gamepad(0).povForward.clicked);
	KeyDown = (Input::KeyS.pressed || Gamepad(0).povBackward.clicked);
	KeyLeft = (Input::KeyA.pressed || Gamepad(0).povLeft.clicked);
	KeyRight = (Input::KeyD.pressed || Gamepad(0).povRight.clicked);
}

KeyInputManager::KeyInputManager()
{
}

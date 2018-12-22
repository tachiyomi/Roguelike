#include "KeyInputManager.h"

void KeyInputManager::renew()
{
	KeyUp = (Input::KeyW.clicked || Gamepad(0).povForward.clicked);
	KeyDown = (Input::KeyS.clicked || Gamepad(0).povBackward.clicked);
	KeyLeft = (Input::KeyA.clicked || Gamepad(0).povLeft.clicked);
	KeyRight = (Input::KeyD.clicked || Gamepad(0).povRight.clicked);
	KeyLeaningOnly = (Input::KeyControl.pressed || Gamepad(0).button(3).pressed);
	KeyNoAttack = (Input::KeyShift.pressed || Gamepad(0).button(4).pressed);

	KeyDesition = (Input::KeyEnter.clicked || Gamepad(0).button(0).clicked);
	KeyBack = (Input::KeyBackspace.clicked || Gamepad(0).button(1).clicked);
	KeyMenu = (Input::KeyI.clicked || Gamepad(0).button(2).clicked);
	KeyPause = (Input::KeyP.clicked || Gamepad(0).button(7).clicked);
}

KeyInputManager::KeyInputManager()
{
}

#include "Ability.h"
#include "Character.h"

void Shout::whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B, std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)
{
	LogSystem::getInstance().addLog(+L"‚¨‚ç‚ !");
}

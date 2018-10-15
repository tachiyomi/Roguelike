#include "Ability.h"
#include "Character.h"

void Shout::whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B, std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)
{
	LogSystem::getInstance().addLog(+L"おらあ!");
	LogSystem::getInstance().addLog(+L"攻撃を食らええ!(アビリティ)");
}

void MindBreak::whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B, std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)
{
	B->addAbility(std::make_shared<Sleepy>(3));
}

void Sleepy::turnEnd(std::shared_ptr<Character> A)
{
	turn--;
	if (turn <= 0)
	{
		A->addAbility(std::make_shared<Sleeping>(1));
		LogSystem::getInstance().addLog(A->getName() + L"は寝てしまった。");
	}
	else
		LogSystem::getInstance().addLog(A->getName() + L"はあと" + ToString(turn) + L"ターンで寝てしまう。");
}

void Sleeping::turnStart(std::shared_ptr<Character> A)
{
	if (turn <= 0)
	{
		LogSystem::getInstance().addLog(A->getName() + L"は起きた。");
	}
	else
	{
		LogSystem::getInstance().addLog(A->getName() + L"は寝ていて動けない。");
		A->setStatus(CharacterStatus::EndAction);
	}
	turn--;
}

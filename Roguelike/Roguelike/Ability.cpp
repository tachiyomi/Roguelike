#include "Ability.h"
#include "Character.h"

void Shout::whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B, std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)
{
	LogSystem::getInstance().addLog(+L"���炠!");
	LogSystem::getInstance().addLog(+L"�U����H�炦��!(�A�r���e�B)");
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
		LogSystem::getInstance().addLog(A->getName() + L"�͐Q�Ă��܂����B");
	}
	else
		LogSystem::getInstance().addLog(A->getName() + L"�͂���" + ToString(turn) + L"�^�[���ŐQ�Ă��܂��B");
}

void Sleeping::turnStart(std::shared_ptr<Character> A)
{
	if (turn <= 0)
	{
		LogSystem::getInstance().addLog(A->getName() + L"�͋N�����B");
	}
	else
	{
		LogSystem::getInstance().addLog(A->getName() + L"�͐Q�Ă��ē����Ȃ��B");
		A->setStatus(CharacterStatus::EndAction);
	}
	turn--;
}

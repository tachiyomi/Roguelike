#pragma once
//#include "Character.h"
#include <Siv3D.hpp>
#include <memory>
#include "LogSystem.h"
#include "function.h"

class Character;

class Ability
{
public:
	Ability() {};
	virtual void whenCombat(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB) {};
	virtual void whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB) {
		LogSystem::getInstance().addLog(L"���炠!!!!!!!");
	};
	virtual void whenDefend(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB) {};
	String getName() { return name; };
protected:
	String name;
};

class Shout :public Ability
{
public:
	Shout() { name = L"���̋���"; };
	void whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)override;
};
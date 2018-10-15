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

	virtual void turnStart(std::shared_ptr<Character> A) {};
	virtual void turnEnd(std::shared_ptr<Character> A) {};

	virtual void whenCombat(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB) {};
	virtual void whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB) {};
	virtual void whenDefend(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB) {};

	virtual bool remove() { return false; };
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

class MindBreak :public Ability
{
public:
	MindBreak() { name = L"���_�j��"; };
	void whenAttack(std::shared_ptr<Character> A, std::shared_ptr<Character> B,
		std::shared_ptr<Character> copyA, std::shared_ptr<Character> copyB)override;
};

class Sleepy :public Ability
{
public:
	Sleepy(int i) { name = L"���Ƃ��Ə��"; turn = i; };
	void turnEnd(std::shared_ptr<Character> A)override;
	bool remove()override { return turn <= 0; };
private:
	int turn;
};

class Sleeping :public Ability
{
public:
	Sleeping(int i) { name = L"������"; turn = i; };
	void turnStart(std::shared_ptr<Character> A)override;
	bool remove()override { return turn < 0; };
private:
	int turn;
};
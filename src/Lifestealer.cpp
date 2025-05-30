#include "Lifestealer.h"
#include "Interface.h"
#include "Player.h"
#include <format>
#include <random>

int Lifestealer::attack(Player &target, std::mt19937 &gen)
{
    std::uniform_int_distribution<> damageDist(2, 5);
    int damage = damageDist(gen);

    Interface::addLogMessage("Lifestealer is attacking...     ");

    std::string message = std::format("Lifestealer hits you for: {} damage.", damage);
    Interface::addLogMessage(message);

    target.takeDamage(damage);

    int healAmount = damage / 2;
    int currentHP = this->getHealthPoints();
    int maxHP = this->getMaxHEALTH();
    int newHP = std::min(currentHP + healAmount, maxHP);

    message = std::format("Lifestealer heals itself for: {} HP.", healAmount);
    Interface::addLogMessage(message);

    this->setHealthPoints(newHP);
    Interface::updateEnemySection(*this);

    return damage;
}

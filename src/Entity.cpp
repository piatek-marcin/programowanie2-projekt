#include "Entity.h"
#include "Interface.h"

const std::string &Entity::getClassName() const
{
    return m_className;
}

int Entity::getHealthPoints() const
{
    return healthPoints;
}

const int Entity::getMaxHEALTH() const
{
    return MAX_HEALTH_POINTS;
}

void Entity::takeDamage(int damage)
{
    healthPoints -= damage;
    /*
    if (onUpdate)        // jeśli fun. onUpdate jest zdefiniowana
        onUpdate(*this); // wywołaj onUpdate biorąc jako argument siebie

        -> zamieniono metody interfejsu na static więc dla uproszczenia wyrzucono lambdy i callbacki z kodu, zostają skomentowane jako "notatki"
    */
}

/*
void Entity::setOnUpdateCallback(std::function<void(const Entity &)> callback)
{
    onUpdate = callback;
}
    -> zamieniono metody interfejsu na static więc dla uproszczenia wyrzucono lambdy i callbacki z kodu, zostają skomentowane jako "notatki"
*/
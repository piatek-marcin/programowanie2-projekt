#ifndef ENTITY_H
#define ENTITY_H

#include "StatusEffectManager.h" // buffy i debuffy
#include <iostream>
#include <functional> // callback
#include <random>     // std::mt19937

// klasa bazowa dla wszystkich entity - gracza, przeciwników
class Entity
{
private:
    const std::string m_className; // nazwa danej klasy
    int healthPoints;              // punkty życia
    const int MAX_HEALTH_POINTS;   // maksymalne HP

    // Funkcja zwrotna, aktywowana w momencie update'u poszczególnych składowych obiektu, używana do wypisywania na UI zaktualizowanych informacji
    // dzięki niej Entity nie musi includowac Interface (Interface juz includuje Entity)
    // zmienna przechowująca funkcję zwracającą void, przyjmująca jako argument const Entity&

    // std::function<void(const Entity &)> onUpdate; -> zamieniono metody interfejsu na static więc dla uproszczenia wyrzucono lambdy i callbacki z kodu, zostają skomentowane jako "notatki"

    StatusEffectManager m_statusManager; // obiekt odpowiedzialny za zarządzanie efektami

protected:
    // chroniony setter do HP
    void setHealthPoints(int hp)
    {
        healthPoints = std::clamp(hp, 0, MAX_HEALTH_POINTS);
    }

public:
    Entity(std::string className, int hp, int MAX_HP = 40) : m_className(className), healthPoints(hp), MAX_HEALTH_POINTS(MAX_HP), m_statusManager(this) {};

    // getter nazwy klasy
    virtual const std::string &getClassName() const = 0;

    // Ustawienie funkcji zwrotnej
    // jako argument przyjmuje zmienną zawierającą funkcję zwracającą void, która sama w sobie przyjmuje const Entity& (wewnątrz setOnUpdateCallback ta zmienna, czyli ta funkcja nazywa się callback)

    // virtual void setOnUpdateCallback(std::function<void(const Entity &)> callback); -> zamieniono metody interfejsu na static więc dla uproszczenia wyrzucono lambdy i callbacki z kodu, zostają skomentowane jako "notatki"

    // implementacja otrzymywania obrażeń
    virtual void takeDamage(int damage) = 0;

    // getter HP
    virtual int getHealthPoints() const = 0;

    // getter maksymalnego HP
    virtual const int getMaxHEALTH() const = 0;

    virtual ~Entity() = default;

    // zarządzanie efektami oddelegowane do StatusEffectManager

    // dodaje efekt
    virtual void addStatusEffect(const StatusEffect &effect)
    {
        m_statusManager.addEffect(effect);
    }

    // usuwa efekt
    virtual void removeStatusEffect(StatusEffectType type)
    {
        m_statusManager.removeEffect(type);
    }

    // sprawdza, czy ma dany efekt
    bool hasStatus(StatusEffectType type) const
    {
        return m_statusManager.hasEffect(type);
    }

    // nadpisuje czas trwania wszystkich posiadanych efektu
    virtual void updateEffectTime(int deltaTime)
    {
        m_statusManager.updateEffectTime(deltaTime);
    }

    // getter aktywnych efektow
    std::vector<StatusEffect> getActiveEffects() const
    {
        return m_statusManager.getActiveEffects();
    }

    // rozprzestrzenianie się ognia, jesli atakujący ma onFire to przekazuje go celowi
    void applyOnFireEffect(Entity &target);

    // jeśli entity jest podpalone to otrzymuje damage
    virtual void takeFireDamage();

    // rzeczy do wykonania na poczatku tury, np. takeFireDamage, takeIllumDamage, etc.
    virtual void endTurnActions() = 0;
};

#endif

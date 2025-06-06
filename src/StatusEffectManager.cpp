#include "StatusEffectManager.h"
#include "Interface.h"
#include "Entity.h"
#include <algorithm>

std::string StatusEffectManager::statusEffectTypeToString(const StatusEffectType type) const
{
    switch (type)
    {
    case StatusEffectType::confusion:
        return "Confusion";
    case StatusEffectType::wetness:
        return "Wetness";
    case StatusEffectType::onFire:
        return "On Fire";
    case StatusEffectType::illumination:
        return "Illumination";
    default:
        return "";
    }
}

void StatusEffectManager::addEffect(const StatusEffect &effect)
{
    // jesli nie naklada nic to wyjdz
    if (effect.m_effectType == StatusEffectType::none)
        return;

    // jeśli efekt jest już nałożony to tylko odświeża czas trwania

    // dla każdego efektu w wektorze aktywnych efektów
    for (auto &e : activeEffects)
    {
        // sprawdź, czy zawarty efekt jest tym samym co nakładany
        if (e.m_effectType == effect.m_effectType)
        {
            // jeśli tak to odswież czasu trwania zamiast nakładać na nowo
            e.remainingDuration = effect.remainingDuration;
            return;
        }
    }
    // jesli nie jest nałożony to dodaje efekt
    activeEffects.push_back(effect);

    // update interfejsu
    Interface::addLogMessage(m_managedEntity->getClassName() + " received effect \e[1m" + effect.getStatusEffectName() + "\e[0m. ");

    // eyesore mowi ze go boli
    if (m_managedEntity->getClassName() == "Eyesore" && effect.getStatusEffectName() == "Illumination")
        Interface::addLogMessage(std::format("\e[31maARrrEEE YOU CraAZZY?? ... iThh hURTS\e[0m"));
}

void StatusEffectManager::removeEffect(StatusEffectType type)
{
    // .erase() usuwa elementy z wektora z zakresu [first; last), tu: od pierwszego elementu do usuniecia (zwroconego po przetasowaniu przez remove_if) do ostatniego elementu wektora (takze po przetasowaniu, takim, ze elementy do usuniecia sa na koncu wektora)
    activeEffects.erase(
        // remove_if() przenosi wszystkie elementy danego typu na koniec wektora, zwraca iterator do pierwszego elementu do usunięcia (i skraca zakres wektora [first; last), tak, że last jest ostatnim elementem NIE do usunięcia)
        std::remove_if(activeEffects.begin(), activeEffects.end(),
                       /* funkcja lambda (czyli bez nazwy, anonimowa)
                        [capture] pozwala lambdzie na kopie zmiennych z zewnątrz (takie dziedziczenie) w momencie jej tworzenia
                        (parameters) to przekazanie parametrów - podczas wywołania funkcji
                       */
                       [type](const StatusEffect &e)
                       {
                           // if(e.m_effectType==type) return true;
                           return e.m_effectType == type;
                       }),
        activeEffects.end());
    Interface::addLogMessage(m_managedEntity->getClassName() + " lost effect \e[1m" + statusEffectTypeToString(type) + "\e[0m. ");
}

bool StatusEffectManager::hasEffect(StatusEffectType type) const
{
    // dla wszystkich obiektów w activeEffects
    for (const auto &e : activeEffects)
    {
        // sprawdza czy obecnie ma jakiś efekt danego typu
        if (e.m_effectType == type)
        {
            return true;
        }
    }
    return false;
}

void StatusEffectManager::updateEffectTime(int deltaTime)
{
    // sprawdza czasy dla wszystkich aktywnych efektów
    for (auto &e : activeEffects)
    {
        // zmniejsza pozostały czas o deltaTime
        e.remainingDuration -= deltaTime;
    }

    // przechowuje efekty, których czas trwania <= 0
    std::vector<StatusEffectType> expiredEffects;

    for (const auto &e : activeEffects)
    {
        if (e.remainingDuration <= 0)
        {
            expiredEffects.push_back(e.m_effectType);
        }
    }

    // usuwa efekty dla których skończył się czas (te wyrzucone poza zakres przez remove_if)
    activeEffects.erase(
        // remove_if zostawia wszystkie aktywne efekty i usuwa z zakresu wektora te dla których czas się skończył
        std::remove_if(activeEffects.begin(), activeEffects.end(),
                       // lambda jako parametr bierze obecnie iterowany efekt i sprawdza, czy jego czas trwania jest <=0
                       [](const StatusEffect &e)
                       {
                           return e.remainingDuration <= 0;
                       }),
        activeEffects.end());

    for (StatusEffectType type : expiredEffects)
    {
        Interface::addLogMessage(m_managedEntity->getClassName() + " lost effect \e[1m" + statusEffectTypeToString(type) + "\e[0m. ");
        Interface::updateMessagesSection();
    }
}

std::vector<StatusEffect> StatusEffectManager::getActiveEffects() const
{
    // zwraca aktywne efekty
    return activeEffects;
}

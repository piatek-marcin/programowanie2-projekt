#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Item.h"
#include <memory> // shared_ptr w wektorze items
#include <string>
#include <vector>
#include <random> // std::mt19937

class Enemy;

// klasa gracza
class Player : public Entity
{
public:
    // klasa zagnieżdżona Inventory obsługująca ekwipunek gracza
    class Inventory
    {
    private:
        std::vector<std::shared_ptr<Item>> m_items; // smart pointer który pozwala na przechowywanie różnych podklas w wektorze

        Player *m_owner; // wskaźnik na obiekt gracza będący obiektem zewnętrznym ekwipunku, potrzebny do użycia metod zarządzających interfejsem wewnątrz metod Inventory (normalnie Inventory nie ma dostępu do wskaźnika "this" klasy Player)

    public:
        Inventory(Player *owner) : m_owner(owner) {}; // konstruktor ustawiający na ownera ekwipunku obiekt zewnętrzny (gracza), potrzebny, żeby przesłać obiekt klasy Player do metod Interface wewnątrz Inventory

        void addItem(const std::shared_ptr<Item> &item);       // dodawanie przedmiotu do ekwipunku
        void removeItem(const std::shared_ptr<Item> &item);    // usuwanie przedmiotu z ekwipunku np. przy użyciu
        bool hasItem(const std::shared_ptr<Item> &item) const; // sprawdza czy przedmiot jest w ekwipunku
        void listItems() const;                                // wypisuje wszystkie przedmioty, które gracz obecnie posiada
        // getter itemów
        std::vector<std::shared_ptr<Item>> getItems() const;
    };

private:
    const std::string m_playerName;
    Inventory m_inventory; // prywatny ekwipunek dla gracza

public:
    Player(std::string className, int hp, int MAX_HP = 100, std::string playerName = "Me")
        : Entity(className, hp, MAX_HP), m_playerName(playerName), m_inventory(this) {}

    // getter wybranego imienia gracza
    const std::string &getPlayerName() const;

    // getter nazwy klasy, w przypadku gracza zwraca imie
    const std::string &getClassName() const override;

    // atak symulujacy podwojny rzut kostka 1d6
    void attack(Enemy &target, std::mt19937 &gen);

    // setter HP, implementacja otrzymywania obrażeń
    void takeDamage(int damage) override;

    // getter HP
    int getHealthPoints() const override
    {
        return Entity::getHealthPoints();
    }

    // getter maksymalnegoHP
    const int getMaxHEALTH() const override
    {
        return Entity::getMaxHEALTH();
    }

    virtual void addStatusEffect(const StatusEffect &effect);

    // usuwa efekt
    virtual void removeStatusEffect(StatusEffectType type);

    // nadpisuje czas trwania wszystkich posiadanych efektu
    virtual void updateEffectTime(int deltaTime);

    // bierze decyzję gracza i jeśli kliknięto odpowiedni przycisk to wykonuje daną akcję (atakuje wroga, używa przedmiotu)
    void getPlayerChoice(Enemy &target, std::mt19937 &gen);

    void endTurnActions() override;

    ~Player() = default;

    const Inventory &getInventory() const; // dostęp do ekwipunku dla funkcji nie modifykujących go

    Inventory &getInventory(); // dostęp do ekwipunku
};

#endif

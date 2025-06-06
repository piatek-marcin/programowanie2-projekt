#include "Player.h"
#include "Entity.h"
#include "Interface.h"
#include "Enemy.h"
#include "HealingItem.h"

#include <algorithm>
#include <random> // std::mt19937

// getter imienia
const std::string &Player::getPlayerName() const
{
    return m_playerName;
}
// getter klasy
const std::string &Player::getClassName() const
{
    return Player::getPlayerName();
}

// atak symulujacy podwojny rzut kostka 1d6
void Player::attack(Enemy &target, std::mt19937 &gen)
{
    Interface::addLogMessage("To hit the enemy you must roll at least 3.");
    Interface::addLogMessage("[ \e[33mPress Enter to roll the attack dice\e[0m ]");
    Interface::Pause();

    std::uniform_int_distribution<> dice(1, 6);
    int firstRoll = dice(gen);

    if (firstRoll < 3)
    {
        Interface::addLogMessage("You missed the enemy :c");
        Interface::addLogMessage("[ \e[33mPress Enter to continue \e[0m]");
        Interface::Pause();
        return;
    }

    // jeśli gracz ma efekt wetness to trudniej mu trafić
    if (this->hasStatus(StatusEffectType::wetness))
    {
        firstRoll = std::max(1, firstRoll - 1); // zapewnia że wynik nie będzie mniejszy niż 1
    }

    std::string message = std::format("You rolled: \e[1m{}\e[0m.", firstRoll);
    Interface::addLogMessage(message);

    // jeśli wynik pierwszego rzutu jest mniejszy niż 3 atak nie trafia
    if (firstRoll >= 3)
    {
        Interface::addLogMessage("[ \e[33mNow press Enter to roll the damage dice\e[0m ]");
        Interface::Pause();

        int damageRoll = dice(gen);

        bool hasConfusion = this->hasStatus(StatusEffectType::confusion);
        bool hitSelf = false;

        // jeśli gracz ma efekt confusion to ma szanse na zaatakowanie siebie
        if (hasConfusion)
        {
            int confusionRoll = dice(gen);
            // jeżeli wynik na kostce < 4 to gracz trafia siebie
            hitSelf = (confusionRoll < 4);
        }

        if (hitSelf)
        {
            // message log
            message = std::format("You are confused which makes you hit yourself for \e[1m{}\e[0m damage.", damageRoll);
            Interface::addLogMessage(message);
            Interface::addLogMessage("[ \e[33mPress Enter to continue\e[0m ]");
            Interface::Pause();

            // Atak samego siebie
            this->takeDamage(damageRoll);
        }
        else // Jak nie siebie to przeciwnika
        {
            target.takeDamage(damageRoll);

            applyOnFireEffect(target); // naklada podpalenie na przeciwnika jezeli sam jest podpalony

            // UI
            Interface::addLogMessage(std::format("You hit the enemy for \e[1m{}\e[0m damage!", damageRoll));
            Interface::updateEnemySection(target);
            Interface::addLogMessage("[ \e[33mPress Enter to continue\e[0m ]");
            Interface::Pause();
        }
        return;
    }
}

void Player::takeDamage(int damage)
{
    Entity::takeDamage(damage);
    Interface::updatePlayerSection(*this);
}

// dostęp do ekwipunku BEZ MODYFIKACJI
const Player::Inventory &Player::getInventory() const
{
    return m_inventory;
}

// dostęp do ekwipunku z modifykacjami
Player::Inventory &Player::getInventory()
{
    return m_inventory;
}

// dodawanie przedmiotu
void Player::Inventory::addItem(const std::shared_ptr<Item> &item)
{
    m_items.push_back(item);
    Interface::updateInventorySection(*m_owner);
    Interface::updateOptionsSection(*m_owner);
    Interface::addLogMessage(std::format("You found a \e[1m{}\e[0m!", item->getItemName()));
}

// usuwanie przedmiotu
void Player::Inventory::removeItem(const std::shared_ptr<Item> &item)
{
    for (auto it = m_items.begin(); it != m_items.end(); ++it)
    {
        // UWAGA! sprawdza czy obiekt ma taką samą nazwę (czy jest TAKI sam), a nie czy jest TEN sam! (*it == item)
        // jeśli sprawdzałoby, czy jest TEN sam to usunęło by tylko jeśli ADRES BYŁBY TAKI SAM.
        if (**it == *item)
        {
            m_items.erase(it);
            Interface::updateInventorySection(*m_owner);
            Interface::updateOptionsSection(*m_owner);
            break; // przerywamy po usunięciu (czyli usunie tylko 1 item danego typu!)
        }
    }
}

// sprawdzanie czy przedmiot jest w ekwipunku
bool Player::Inventory::hasItem(const std::shared_ptr<Item> &item) const
{
    for (const auto &i : m_items)
    {
        if (*i == *item)
        {
            return true;
        }
    }
    return false;
}

// wypisanie wszystkich przedmiotów
void Player::Inventory::listItems() const
{
    std::cout << "Inventory items:\n";
    for (const auto &item : m_items)
    {
        std::cout << "- " << item->getItemName() << "\n";
    }
}

std::vector<std::shared_ptr<Item>> Player::Inventory::getItems() const
{
    return m_items;
}

// bierze decyzję gracza i jeśli kliknięto odpowiedni przycisk to wykonuje daną akcję (atakuje wroga, używa przedmiotu)
void Player::getPlayerChoice(Enemy &target, std::mt19937 &gen)
{
    Interface::addLogMessage("[ \e[33mChoose one of the options on the left \e[0m]");
    char input;
    while (true)
    {
        input = _getch();

        // Scrollowanie dzienniczka
        if (input == 'j' || input == 'J')
        {
            Interface::scrollMessagesUp();
            continue; // scrollowanie nie przerywa
        }

        if (input == 'k' || input == 'K')
        {
            Interface::scrollMessagesDown();
            continue; // scrollowanie nie przerywa
        }

        // Przyciski 1-9: wybór opcji dialogowych
        if (input >= '1' && input <= '9')
        {
            int choice = input - '0';

            if (choice == 1)
            {
                Interface::removeLastMessage();
                this->attack(target, gen);
                return;
            }
            else if (choice >= 2)
            {
                int index = choice - 2; // zamiana na index zaczynający od 0

                const auto &items = this->getInventory().getItems();

                Interface::removeLastMessage(); // usuniecie "Choose one of the options..."

                // jesli indeks przedmiotu (obliczany z kliknietego klawisza) nie wykracza poza wektor
                if (items[index]->getItemType() == "Healing")
                {
                    items[index]->useItem(*this); // przeładowanie z parametrem gracza
                    return;
                }
                else
                {
                    items[index]->useItem(target); // przeładowanie z entity
                    return;
                }
            }
        }
    }
}

void Player::endTurnActions()
{
    Entity::endTurnActions();
}

void Player::addStatusEffect(const StatusEffect &effect)
{
    Entity::addStatusEffect(effect);
    Interface::updatePlayerSection(*this);
}

// usuwa efekt
void Player::removeStatusEffect(StatusEffectType type)
{
    Entity::removeStatusEffect(type);
    Interface::updatePlayerSection(*this);
}

// nadpisuje czas trwania wszystkich posiadanych efektu
void Player::updateEffectTime(int deltaTime)
{
    Entity::updateEffectTime(deltaTime);
    Interface::updatePlayerSection(*this);
}
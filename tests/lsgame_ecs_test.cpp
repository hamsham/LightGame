#include <iostream>

#include "lightsky/setup/Macros.h" // LS_STRINGIFY

#include "lightsky/game/ECSDatabase.hpp"

namespace game = ls::game;



class PrintStdoutComponent final : public game::Component
{
  public:
    ~PrintStdoutComponent() {}

    PrintStdoutComponent()
    {
        std::cout << "constructed"<< std::endl;
    }

    virtual void update_entity(const game::Entity& e) noexcept
    {
        std::cout << "\tIterating over entity " << ": " << e.id << std::endl;
    }
};

LS_GAME_REGISTER_COMPONENT(PrintStdoutComponent)



class PrintErrComponent final : public game::Component
{
  public:
    ~PrintErrComponent() {}

    PrintErrComponent()
    {
        std::cerr << "constructed"<< std::endl;
    }

    virtual void update_entity(const game::Entity& e) noexcept
    {
        std::cerr << "\tIterating over entity " << ": " << e.id << std::endl;
    }
};

LS_GAME_REGISTER_COMPONENT(PrintErrComponent)



void update_components(game::ECSDatabase& db) noexcept
{
    std::cout << "Updating components:" << std::endl;
    db.component<PrintStdoutComponent>()->update();
    db.component<PrintErrComponent>()->update();
}



bool construct_components(game::ECSDatabase& db) noexcept
{
    game::ComponentCreateStatus addStatus = db.construct_component<PrintStdoutComponent>();
    if (addStatus != game::ComponentCreateStatus::REGISTER_OK)
    {
        std::cerr << "Unable to construct a STDOUT component." << std::endl;
        return false;
    }
    std::cout << "Successfully registered a STDOUT component within the ECS Database." << std::endl;

    addStatus = db.construct_component<PrintErrComponent>();
    if (addStatus != game::ComponentCreateStatus::REGISTER_OK)
    {
        std::cerr << "Unable to construct a STDERR component." << std::endl;
        return false;
    }
    std::cout << "Successfully registered a STDERR component within the ECS Database." << std::endl;

    return true;
}



int main()
{
    game::ECSDatabase db = {};
    game::Entity e0 = db.create_entity();

    if (db.num_components(e0) > 0)
    {
        std::cerr << "Unexpected components found for entity " << e0.id << std::endl;
        return -1;
    }
    std::cout << "Successfully initialized an entity." << std::endl;

    if (!construct_components(db))
    {
        std::cerr << "Unable to construct test components." << std::endl;
        return -2;
    }

    game::ComponentAddStatus addStatus = db.component<PrintStdoutComponent>()->insert(e0);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add an entity to the STDOUT component." << std::endl;
        return -3;
    }
    std::cout << "Successfully added an entity to the STDOUT component." << std::endl;

    addStatus = db.component<PrintErrComponent>()->insert(e0);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add an entity to the STDERR component." << std::endl;
        return -3;
    }
    std::cout << "Successfully added an entity to the STDERR component." << std::endl;

    update_components(db);

    game::ComponentRemoveStatus removeStatus = db.component<PrintStdoutComponent>()->erase(e0);
    if (removeStatus != game::ComponentRemoveStatus::REMOVE_OK)
    {
        std::cerr << "Unable to remove entity from the print component " << e0.id << std::endl;
        return -4;
    }
    std::cout << "Successfully removed an entity from a component." << std::endl;

    if (db.component<PrintStdoutComponent>()->size() != 0)
    {
        std::cerr << "Orphaned entities in the print component." << std::endl;
        return -5;
    }

    db.destroy_entity(e0);

    return 0;
}

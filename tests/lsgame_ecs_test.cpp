#include <iostream>

#include "lightsky/setup/Macros.h" // LS_STRINGIFY

#include "lightsky/utils/Assertions.h"

#include "lightsky/game/ECSDatabase.hpp"

namespace game = ls::game;



class PrintStdoutComponent final : public game::Component
{
  public:
    ~PrintStdoutComponent() {}

    PrintStdoutComponent()
    {
        std::cout << "constructed STDOUT" << std::endl;
    }

    virtual void update_entity(const game::Entity& e) noexcept
    {
        std::cout << "\tIterating over STDOUT entity " << ": " << e.id << std::endl;
    }
};

LS_GAME_REGISTER_COMPONENT(PrintStdoutComponent)



class PrintErrComponent final : public game::Component
{
  public:
    ~PrintErrComponent() {}

    PrintErrComponent()
    {
        std::cout << "constructed STDERR" << std::endl;
    }

    virtual void update_entity(const game::Entity& e) noexcept
    {
        std::cout << "\tIterating over STDERR entity " << ": " << e.id << std::endl;
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



bool assign_entities(game::ECSDatabase& db, game::Entity& e0, game::Entity& e1, game::Entity& e2) noexcept
{
    game::ComponentAddStatus addStatus = db.component<PrintStdoutComponent>()->insert(e0);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add entity " << e0.id << " to the STDOUT component." << std::endl;
        return false;
    }
    std::cout << "Successfully added entity " << e0.id << " to the STDOUT component." << std::endl;

    addStatus = db.component<PrintErrComponent>()->insert(e0);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add entity " << e0.id << " to the STDERR component." << std::endl;
        return false;
    }
    std::cout << "Successfully added entity " << e0.id << " to the STDERR component." << std::endl;

    addStatus = db.component<PrintStdoutComponent>()->insert(e1);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add entity " << e1.id << " to the STDOUT component." << std::endl;
        return false;
    }
    std::cout << "Successfully added entity " << e1.id << " to the STDOUT component." << std::endl;

    addStatus = db.component<PrintErrComponent>()->insert(e2);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add entity " << e2.id << " to the STDERR component." << std::endl;
        return false;
    }
    std::cout << "Successfully added entity " << e2.id << " to the STDERR component." << std::endl;

    return true;
}



int main()
{
    game::ECSDatabase db = {};
    game::Entity e0 = db.create_entity();
    game::Entity e1 = db.create_entity();
    game::Entity e2 = db.create_entity();

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

    if (!assign_entities(db, e0, e1, e2))
    {
        return -3;
    }

    update_components(db);

    LS_ASSERT(e1.id == 1);
    db.destroy_entity(e1);
    LS_ASSERT(false == db.component<PrintStdoutComponent>()->contains(e1));
    LS_ASSERT(false == db.component<PrintStdoutComponent>()->contains(game::Entity{1}));
    LS_ASSERT(e1.id == game::ECSDatabase::INVALID_ENTITY);

    game::ComponentRemoveStatus removeStatus = db.component<PrintStdoutComponent>()->erase(e1);
    if (removeStatus == game::ComponentRemoveStatus::REMOVE_ERR_ENTITY_MISSING)
    {
        std::cout << "Successfully removed entity e1 from a component (now " << e1.id << ")." << std::endl;
    }
    else
    {
        std::cerr << "An error occurred while removing entity e1 from the print component " << e1.id << std::endl;
        return -4;
    }

    game::Entity e3 = db.create_entity();
    game::ComponentAddStatus addStatus = db.component<PrintErrComponent>()->insert(e3);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add entity " << e3.id << " to the STDERR component." << std::endl;
        return -5;
    }
    std::cout << "Successfully added entity " << e3.id << " to the STDERR component." << std::endl;

    update_components(db);

    if (db.component<PrintStdoutComponent>()->size() != 1)
    {
        std::cerr << "Orphaned entities in the print component." << std::endl;
        return -6;
    }

    return 0;
}

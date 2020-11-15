#include <iostream>

#include "lightsky/game/ECSDatabase.hpp"

namespace game = ls::game;



class PrintComponent
{
  private:
    std::unordered_set<game::Entity> mEntities;

  public:
    virtual ~PrintComponent() noexcept = default;

    PrintComponent() :
        mEntities{}
    {}

    game::ComponentAddStatus insert(const game::Entity& e) noexcept
    {
        auto&& iter = mEntities.find(e);
        if (iter == mEntities.end())
        {
            mEntities.insert(e);
            return game::ComponentAddStatus::ADD_OK;
        }

        return game::ComponentAddStatus::ADD_ERR_ENTITY_EXISTS;
    }

    template <typename... Args>
    game::ComponentAddStatus insert(const game::Entity& e, Args&&...) noexcept
    {
        (void)e;
        return game::ComponentAddStatus::ADD_ERR_INVALID_ARGS;
    }

    game::ComponentRemoveStatus erase(const game::Entity& e) noexcept
    {
        auto&& iter = mEntities.find(e);
        if (iter != mEntities.end())
        {
            mEntities.erase(e);
            return game::ComponentRemoveStatus::REMOVE_OK;
        }

        return game::ComponentRemoveStatus::REMOVE_ERR_ENTITY_MISSING;
    }

    bool contains(const game::Entity& e) const noexcept
    {
        return mEntities.count(e) != 0;
    }

    size_t size() const noexcept
    {
        return mEntities.size();
    }

    void clear() noexcept
    {
        mEntities.clear();
    }

    // (Const) iterate over all entries in *this. Each component type should
    // specify its own iterator.
    template <typename IterCallback>
    void iterate(const IterCallback& cb) const noexcept
    {
        for (const game::Entity& e : mEntities)
        {
            cb(*this, e);
        }
    }

    // Iterate over all entries in *this. Each component type should specify
    // its own iterator
    template <typename IterCallback>
    void iterate(const IterCallback& cb) noexcept
    {
        for (game::Entity e : mEntities)
        {
            cb(*this, e);
        }
    }

    // Iterate and view and entity's data in *this
    template <typename IterCallback>
    void view(const game::Entity& e, const IterCallback& cb) const noexcept
    {
        auto&& iter = mEntities.find(e);
        if (iter != mEntities.end())
        {
            cb(*this, *iter);
        }
    }

    // Iterate and modify and entity's data in *this
    template <typename IterCallback>
    void modify(const game::Entity& e, const IterCallback& cb) noexcept
    {
        auto&& iter = mEntities.find(e);
        if (iter != mEntities.end())
        {
            cb(*this, *iter);
        }
    }
};



class PrintStdoutComponent : public PrintComponent
{
  public:
    PrintStdoutComponent() :
        PrintComponent{}
    {
        std::cout << "constructed"<< std::endl;
    }
};



class PrintStderrComponent : public PrintComponent
{
  public:
    PrintStderrComponent() :
        PrintComponent{}
    {
        std::cerr << "constructed"<< std::endl;
    }
};



typedef game::ECSDatabase<PrintStdoutComponent, PrintStderrComponent> EntityDb;



void update_components(EntityDb& db) noexcept
{
    unsigned count;

    count = 0;
    db.iterate<PrintStdoutComponent>([&](PrintComponent&, const game::Entity& e)->void {
        std::cout << "\tIterating over entity " << count << ": " << e.id << std::endl;
        ++count;
    });

    count = 0;
    db.iterate<PrintStderrComponent>([&](PrintComponent&, const game::Entity& e)->void {
        std::cerr << "\tIterating over entity " << count << ": " << e.id << std::endl;
        ++count;
    });
}



int main()
{
    EntityDb db = {};
    game::Entity e0 = db.create_entity();
    if (db.has_components(e0))
    {
        std::cerr << "Unexpected components found for entity " << e0.id << std::endl;
        return -1;
    }
    std::cout << "Successfully initialized an entity." << std::endl;

    game::ComponentAddStatus addStatus = db.add<PrintStdoutComponent>(e0);
    if (addStatus != game::ComponentAddStatus::ADD_OK)
    {
        std::cerr << "Unable to add a print component to entity " << e0.id << std::endl;
        return -2;
    }
    std::cout << "Successfully added a component to an entity." << std::endl;

    db.iterate<PrintStdoutComponent>([&](const PrintComponent&, const game::Entity& e)
    {
        std::cout << "Iterating over entity " << e.id << std::endl;
    });

    std::cout << "Updating components:" << std::endl;
    update_components(db);

    game::ComponentRemoveStatus removeStatus = db.remove<PrintStdoutComponent>(e0);
    if (removeStatus != game::ComponentRemoveStatus::REMOVE_OK)
    {
        std::cerr << "Unable to remove the print component from entity " << e0.id << std::endl;
        return -3;
    }
    std::cout << "Successfully removed a component to an entity." << std::endl;

    if (db.component<PrintStdoutComponent>().size() != 0)
    {
        std::cerr << "Orphaned entities in the print component." << std::endl;
        return -4;
    }

    db.destroy_entity(e0);

    return 0;
}

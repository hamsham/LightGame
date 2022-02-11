
#ifndef LS_GAME_COMPONENT_HPP
#define LS_GAME_COMPONENT_HPP

#include <cstdlib> // size_t
#include <unordered_set>

#include "lightsky/game/Entity.hpp"



namespace ls
{
namespace game
{



enum class ComponentAddStatus : unsigned
{
    ADD_ERR_NO_MEMORY,
    ADD_ERR_ENTITY_EXISTS,
    ADD_ERR_INVALID_ARGS,
    ADD_ERR_OTHER,
    ADD_OK
};

enum class ComponentRemoveStatus : unsigned
{
    REMOVE_ERR_NO_MEMORY,
    REMOVE_ERR_ENTITY_MISSING,
    REMOVE_ERR_OTHER,
    REMOVE_OK
};



class Component
{
    friend class ECSDatabase;

  private:
    static std::size_t _increment_component_id() noexcept;

    template <typename T>
    static std::size_t registration_id() noexcept;

  protected:
    std::unordered_set<Entity> mEntities;

  public:
    virtual ~Component() noexcept = 0;

    Component() noexcept;

    Component(const Component&) = delete;

    Component(Component&&) noexcept;

    Component& operator=(const Component&) = delete;

    Component& operator=(Component&&) noexcept;

    ComponentAddStatus insert(const Entity& e) noexcept;

    ComponentRemoveStatus erase(const Entity& e) noexcept;

    bool contains(const Entity& e) const noexcept;

    size_t size() const noexcept;

    void clear() noexcept;

    virtual void update_entity(const Entity& e) noexcept = 0;

    virtual void update() noexcept;
};



#ifndef LS_GAME_REGISTER_COMPONENT
    #define LS_GAME_REGISTER_COMPONENT( ComponentType ) \
        template <> std::size_t ls::game::Component::registration_id<ComponentType>() noexcept \
        { \
            static const std::size_t id{Component::_increment_component_id()}; \
            return id; \
        }
#endif



inline bool Component::contains(const Entity& e) const noexcept
{
    return mEntities.count(e) > 0;
}



inline size_t Component::size() const noexcept
{
    return mEntities.size();
}



inline void Component::clear() noexcept
{
    mEntities.clear();
}



} // end game namespace
} // end ls namespace

#endif /* LS_GAME_COMPONENT_HPP */


#ifndef LS_GAME_COMPONENT_HPP
#define LS_GAME_COMPONENT_HPP

#include <cstdlib> // size_t
#include <utility> // std::forward

namespace ls
{
namespace game
{



struct Entity;



enum class ComponentAddStatus
{
    ADD_ERR_NO_MEMORY,
    ADD_ERR_ENTITY_EXISTS,
    ADD_ERR_INVALID_ARGS,
    ADD_ERR_OTHER,
    ADD_OK
};

enum class ComponentRemoveStatus
{
    REMOVE_ERR_NO_MEMORY,
    REMOVE_ERR_ENTITY_MISSING,
    REMOVE_ERR_OTHER,
    REMOVE_OK
};



template <typename ComponentType>
class Component
{
  private:
    ComponentType mComponentImpl;

  public:
    ~Component() noexcept = default;

    Component() :
        mComponentImpl{}
    {}

    inline ComponentAddStatus insert(const Entity& e) noexcept
    {
        return mComponentImpl.insert(e);
    }

    template <typename ...Args>
    inline ComponentAddStatus insert(const Entity& e, Args&&... args) noexcept
    {
        return mComponentImpl.insert(e, std::forward<Args>(args)...);
    }

    inline ComponentRemoveStatus erase(const Entity& e) noexcept
    {
        return mComponentImpl.erase(e);
    }

    inline bool contains(const Entity& e) const noexcept
    {
        return mComponentImpl.contains(e);
    }

    inline size_t size() const noexcept
    {
        return mComponentImpl.size();
    }

    inline void clear() noexcept
    {
        mComponentImpl.clear();
    }

    // (Const) iterate over all entries in *this. Each component type should
    // specify its own iterator.
    template <typename IterCallback>
    inline void iterate(const IterCallback& cb) const noexcept
    {
        mComponentImpl.template iterate<IterCallback>(cb);
    }

    // Iterate over all entries in *this. Each component type should specify
    // its own iterator
    template <typename IterCallback>
    inline void iterate(const IterCallback& cb) noexcept
    {
        mComponentImpl.template iterate<IterCallback>(cb);
    }

    // Iterate and view and entity's data in *this
    template <typename IterCallback>
    inline void view(const Entity& e, const IterCallback& cb) const noexcept
    {
        mComponentImpl.template view<IterCallback>(e, cb);
    }

    // Iterate and modify and entity's data in *this
    template <typename IterCallback>
    inline void modify(const Entity& e, const IterCallback& cb) noexcept
    {
        mComponentImpl.template modify<IterCallback>(e, cb);
    }

    // internal component-wise update of all entities
    inline void update() noexcept
    {
        mComponentImpl.update();
    }
};



} // end game namespace
} // end ls namespace

#endif /* LS_GAME_COMPONENT_HPP */

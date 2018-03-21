#pragma once
#include <iostream>
#include <vector>
#include <chrono>

namespace ecs
{
  typedef size_t ID;
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-const-variable"
  const ID NULL_ID = (0-1);//(size_t)(0-1) == SIZE_MAX;
  const ID END = NULL_ID;
  const ID BEGIN = 0;
  #pragma GCC diagnostic pop
  typedef std::chrono::nanoseconds Duration;
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
  typedef std::chrono::high_resolution_clock Clock;

  /********************ENTITIES and COMPONENTS********************/

  class Entity
  {
    template<typename T, typename... Targs>
    friend class RealIterator;
    public:

    static Entity createEntity();
    void removeEntity();
    template<typename T, typename... Args>
    void createComponent(const Args&... args);
    template<typename T>
    void removeComponent();
    template<typename T>
    T & getComponent();
    template<typename T>
    bool hasComponents();
    template<typename T1, typename T2, typename... Targs>
    bool hasComponents();

    template<typename T, typename... Targs>
    class Iterator
    {
      private:

      ID counter;
      ID to;
      ID current = NULL_ID;

      public:

      Iterator();
      Iterator(ID from, ID to);
      bool hasNext();
      Entity next();
      ID getID();
    };

    ID getID()
    {
      return entityID;
    }

    private:

    Entity(ID entityID) : entityID(entityID) {}
    ID entityID;

    struct EntityEntry
    {
      std::vector<void (*)(ID)> removeComponentFunctions = std::vector<void (*)(ID)>();
      bool exists = true;
    };

    static std::vector<EntityEntry> entityEntryArray;
    static std::vector<ID> freeEntityIDs;

    template<typename T>
    static std::vector<T> componentArray;
    template<typename T>
    static std::vector<ID> componentToEntityIDs;
    template<typename T>
    static std::vector<ID> entityToComponentIDs;

    static void removeEntity(ID entityID);
    template<typename T, typename... Args>
    static void createComponent(ID entityID, const Args&... args);
    template<typename T>
    static void removeComponent(ID entityID);
    template<typename T>
    static T & getComponent(ID entityID);
    template<typename T>
    static bool hasComponents(ID entityID);
    template<typename T1, typename T2, typename... Targs>
    static bool hasComponents(ID entityID);
  };
  template<typename T>
  std::vector<T> Entity::componentArray = std::vector<T>();
  template<typename T>
  std::vector<ID> Entity::componentToEntityIDs = std::vector<ID>();
  template<typename T>
  std::vector<ID> Entity::entityToComponentIDs = std::vector<ID>();

  std::vector<ID> Entity::freeEntityIDs = std::vector<ID>();
  std::vector<Entity::EntityEntry> Entity::entityEntryArray = std::vector<EntityEntry>();

  template<typename T, typename... Targs>
  class RealIterator
  {

    public:
    ID entityID;

    RealIterator() : entityID(NULL_ID) {}
    explicit RealIterator(ID entityID) : entityID(entityID) {}
    explicit RealIterator(Entity entity) : entityID(entity.entityID) {}

    RealIterator<T, Targs...> begin()
    {
      ID i = 0;
      while(end().entityID > i)
      {
        if(!Entity::hasComponents<T, Targs...>(i))
        {
          i+=1;
          continue;
        }
        return RealIterator<T, Targs...>(i);
      }
      return end();
    }
    RealIterator<T, Targs...> end()
    {
      return RealIterator<T, Targs...>(Entity::entityEntryArray.size());
    }

    void operator= (const RealIterator<T, Targs...>& a)
    {
      entityID = a.entityID;
    }
    bool operator== (const RealIterator<T, Targs...>& a)
    {
      return entityID == a.entityID;
    }
    bool operator!= (const RealIterator<T, Targs...>& a)
    {
      return entityID != a.entityID;
    }
    Entity operator*()
    {
      return Entity(entityID);
    }
    Entity operator->()
    {
      return Entity(entityID);
    }
    RealIterator<T, Targs...> operator++()         //=> Prefix Increment
    {
      entityID+=1;
      while(end().entityID > entityID)
      {
        if(!Entity::hasComponents<T, Targs...>(entityID))
        {
          entityID+=1;
          continue;
        }
        return RealIterator<T, Targs...>(entityID);
      }
      entityID = end().entityID;
      return end();
    }
    RealIterator<T, Targs...> operator++(int)      //=> Postfix Increment
    {
      RealIterator<T, Targs...> ret = RealIterator<T, Targs...>(entityID);
      entityID+=1;
      while(end().entityID > entityID)
      {
        if(!Entity::hasComponents<T, Targs...>(entityID))
        {
          entityID+=1;
          continue;
        }
        return ret;
      }
      entityID = end().entityID;
      return end();
    }
  };

  Entity Entity::createEntity()
  {
    ID tempID;
    if(freeEntityIDs.empty())
    {
      tempID = entityEntryArray.size();
      entityEntryArray.emplace_back();
    }
    else
    {
      tempID = freeEntityIDs.back();
      freeEntityIDs.pop_back();
      entityEntryArray[tempID].exists = true;
    }
    return Entity(tempID);
  }
  void Entity::removeEntity()
  {
    removeEntity(this->entityID);
  }
  template<typename T, typename... Args>
  void Entity::createComponent(const Args&... args)
  {
    createComponent<T>(this->entityID, args...);
  }
  template<typename T>
  void Entity::removeComponent()
  {
    removeComponent<T>(this->entityID);
  }
  template<typename T>
  T & Entity::getComponent()
  {
    return getComponent<T>(this->entityID);
  }
  template<typename T>
  bool Entity::hasComponents()
  {
    return hasComponents<T>(this->entityID);
  }
  template<typename T1, typename T2, typename... Targs>
  bool Entity::hasComponents()
  {
    return hasComponents<T1, T2, Targs...>(this->entityID);
  }

  void Entity::removeEntity(ID entityID)
  {
    freeEntityIDs.push_back(entityID);
    for(void (*f)(ID) : entityEntryArray[entityID].removeComponentFunctions)
    {
      f(entityID);
    }
    entityEntryArray[entityID].removeComponentFunctions = std::vector<void (*)(ID)>();
    entityEntryArray[entityID].exists = false;
  }
  template<typename T, typename... Args>
  void Entity::createComponent(ID entityID, const Args&... args)
  {
    if(entityID >= entityToComponentIDs<T>.size())
    {
      entityToComponentIDs<T>.resize(entityID+1, NULL_ID);
      entityToComponentIDs<T>[entityID] = componentArray<T>.size();
      componentToEntityIDs<T>.push_back(entityID);
      componentArray<T>.emplace_back(args...);
    }
    else if(entityID < entityToComponentIDs<T>.size())
    {
      bool foundBiggerID = false;
      for(ID i = entityID + 1; i < entityToComponentIDs<T>.size(); i++)
      {
        if(entityToComponentIDs<T>[i] != NULL_ID)
        {
          if(foundBiggerID == false)
          {
            componentArray<T>.emplace(componentArray<T>.begin() + entityToComponentIDs<T>[i], args...);
            componentToEntityIDs<T>.insert(componentToEntityIDs<T>.begin() + entityToComponentIDs<T>[i], entityID);
            entityToComponentIDs<T>[entityID] = entityToComponentIDs<T>[i];
            foundBiggerID = true;
          }
          entityToComponentIDs<T>[i] += 1;
        }
      }
    }
    entityEntryArray[entityID].removeComponentFunctions.push_back(&removeComponent<T>);
  }
  template<typename T>
  void Entity::removeComponent(ID entityID)
  {
    if(!hasComponents<T>(entityID))
    {
      return;
    }
    componentArray<T>.erase(componentArray<T>.begin() + entityToComponentIDs<T>[entityID]);
    componentToEntityIDs<T>.erase(componentToEntityIDs<T>.begin() + entityToComponentIDs<T>[entityID]);
    for(ID i = entityID; i<entityToComponentIDs<T>.size(); i++)
    {
      if(entityToComponentIDs<T>[i] != NULL_ID)
      {
        entityToComponentIDs<T>[i] -= 1;
      }
    }
    entityToComponentIDs<T>[entityID] = NULL_ID;
  }
  template<typename T>
  T & Entity::getComponent(ID entityID)
  {
    return componentArray<T>[entityToComponentIDs<T>[entityID]];
  }
  template<typename T>
  bool Entity::hasComponents(ID entityID)
  {
    return entityToComponentIDs<T>.size() > entityID && entityToComponentIDs<T>[entityID] != NULL_ID;
  }
  template<typename T1, typename T2, typename... Targs>
  bool Entity::hasComponents(ID entityID)
  {
    return hasComponents<T1>(entityID) && hasComponents<T2, Targs...>(entityID);
  }
  template<>
  bool Entity::hasComponents<void>(ID entityID)
  {
    return entityEntryArray.size() > entityID && entityEntryArray[entityID].exists;
  }

  template<typename T, typename... Targs>
  Entity::Iterator<T, Targs...>::Iterator()
  {
    counter = 0;
    this->to = entityEntryArray.size();
  }
  template<typename T, typename... Targs>
  Entity::Iterator<T, Targs...>::Iterator(ID from, ID to)
  {
    counter = from;
    this->to = to == END ? entityEntryArray.size() : to;
  }
  template<typename T, typename... Targs>
  bool Entity::Iterator<T, Targs...>::hasNext()
  {
    while(to>counter)
    {
      if(!hasComponents<T, Targs...>(counter))
      {
        counter+=1;
        continue;
      }
      return true;
    }
    return false;
  }
  template<typename T, typename... Targs>
  Entity Entity::Iterator<T, Targs...>::next()
  {
    if(hasNext())
    {
      current = counter;
      counter+=1;
      return Entity(current);
    }
    return Entity(NULL_ID);
  }
  template<typename T, typename... Targs>
  ID Entity::Iterator<T, Targs...>::getID()
  {
    return current;
  }

  /********************SYSTEMS********************/

  class SystemManager
  {
    public:

    static void addSystem(void (*update)(), const Duration& deltaTime);
    template<typename T>
    static void addSystem(void (*update)(T));
    static void runSystems();
    template<typename T>
    static void throwEvent(const T& event);

    private:

    struct TimeBasedSystem
    {
      TimeBasedSystem(void (*update)(), Duration deltaTime) :
        update(update),
        deltaTime(deltaTime),
        lastUpdateCallTime(TimePoint())
      {
      }
      void (*update)();
      const Duration deltaTime;
      TimePoint lastUpdateCallTime;
    };
    struct EventRegisterHelper
    {
      explicit EventRegisterHelper(void (*f)())
      {
        runEventBasedSystemsList.push_back(f);
      }
    };
    template<typename T>
    static void runEventBasedSystems();

    static std::vector<TimeBasedSystem> timeBasedSystems;
    static std::vector<void(*)()> runEventBasedSystemsList;
    template<typename T>
    static std::vector<void(*)(T)> eventBasedSystem;
    template<typename T>
    static std::vector<T> eventQueue;
  };
  std::vector<SystemManager::TimeBasedSystem> SystemManager::timeBasedSystems = std::vector<SystemManager::TimeBasedSystem>();
  std::vector<void(*)()> SystemManager::runEventBasedSystemsList = std::vector<void(*)()>();
  template<typename T>
  std::vector<void(*)(T)> SystemManager::eventBasedSystem = std::vector<void(*)(T)>();
  template<typename T>
  std::vector<T> SystemManager::eventQueue = std::vector<T>();

  void SystemManager::addSystem(void (*update)(), const Duration& deltaTime)
  {
    timeBasedSystems.emplace_back(update, deltaTime);
  }
  template<typename T>
  void SystemManager::addSystem(void (*update)(T))
  {
    eventBasedSystem<T>.push_back(update);
  }
  void SystemManager::runSystems()
  {
    for(TimeBasedSystem & system : timeBasedSystems)
    {
      TimePoint now = Clock::now();
      Duration currentDeltaTime = now - system.lastUpdateCallTime;
      if(system.deltaTime <= currentDeltaTime)
      {
        system.update();
        system.lastUpdateCallTime = now;
      }
    }
    for(auto f : runEventBasedSystemsList)
    {
      f();
    }
  }
  template<typename T>
  void SystemManager::throwEvent(const T& event)
  {
    //register event
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-variable"
    const static EventRegisterHelper _(&runEventBasedSystems<T>);
    #pragma GCC diagnostic pop

    eventQueue<T>.push_back(event);
  }
  template<typename T>
  void SystemManager::runEventBasedSystems()
  {
    for(T & event : eventQueue<T>)
    {
      for(auto f : eventBasedSystem<T>)
      {
        f(event);
      }
    }
    eventQueue<T>.clear();
  }
}

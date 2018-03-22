#pragma once
#include <iostream>
#include <vector>
#include <chrono>

namespace ecs
{
  typedef size_t ID;
  const ID NULL_ID = (0-1);//(size_t)(0-1) == SIZE_MAX;
  typedef std::chrono::nanoseconds Duration;
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
  typedef std::chrono::high_resolution_clock Clock;

  /********************ENTITIES and COMPONENTS********************/

  class Entity
  {
    template<typename T, typename... Targs>
    friend class Iterator;
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
  std::vector<ID> Entity::entityToComponentIDs = std::vector<ID>();

  std::vector<ID> Entity::freeEntityIDs = std::vector<ID>();
  std::vector<Entity::EntityEntry> Entity::entityEntryArray = std::vector<EntityEntry>();

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
    if(hasComponents<T>(entityID))
    {
      return;
    }
    if(entityID >= entityToComponentIDs<T>.size())
    {
      entityToComponentIDs<T>.resize(entityID+1, NULL_ID);
      entityToComponentIDs<T>[entityID] = componentArray<T>.size();
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
  class Iterator
  {
    private:

    ID entityID;

    public:

    Iterator() : entityID(NULL_ID) {}
    explicit Iterator(ID entityID) : entityID(entityID) {}
    explicit Iterator(Entity entity) : entityID(entity.entityID) {}

    Iterator<T, Targs...> begin();
    Iterator<T, Targs...> end();

    void operator= (const Iterator<T, Targs...>& a);
    bool operator== (const Iterator<T, Targs...>& a);
    bool operator!= (const Iterator<T, Targs...>& a);
    Entity operator*();
    Entity operator->();
    Iterator<T, Targs...> operator++();
  };

  template<typename T, typename... Targs>
  Iterator<T, Targs...> Iterator<T, Targs...>::begin()
  {
    ID i = 0;
    while(end().entityID > i)
    {
      if(!Entity::hasComponents<T, Targs...>(i))
      {
        i+=1;
        continue;
      }
      return Iterator<T, Targs...>(i);
    }
    return end();
  }
  template<typename T, typename... Targs>
  Iterator<T, Targs...> Iterator<T, Targs...>::end()
  {
    return Iterator<T, Targs...>(Entity::entityEntryArray.size());
  }
  template<typename T, typename... Targs>
  void Iterator<T, Targs...>::operator= (const Iterator<T, Targs...>& a)
  {
    entityID = a.entityID;
  }
  template<typename T, typename... Targs>
  bool Iterator<T, Targs...>::operator== (const Iterator<T, Targs...>& a)
  {
    return entityID == a.entityID;
  }
  template<typename T, typename... Targs>
  bool Iterator<T, Targs...>::operator!= (const Iterator<T, Targs...>& a)
  {
    return entityID != a.entityID;
  }
  template<typename T, typename... Targs>
  Entity Iterator<T, Targs...>::operator*()
  {
    return Entity(entityID);
  }
  template<typename T, typename... Targs>
  Entity Iterator<T, Targs...>::operator->()
  {
    return Entity(entityID);
  }
  template<typename T, typename... Targs>
  Iterator<T, Targs...> Iterator<T, Targs...>::operator++() // Prefix Increment
  {
    entityID+=1;
    while(end().entityID > entityID)
    {
      if(Entity::hasComponents<T, Targs...>(entityID))
      {
        return Iterator<T, Targs...>(entityID);
      }
      entityID+=1;
    }
    entityID = end().entityID;
    return end();
  }

  /********************SYSTEMS********************/

  class SystemManager
  {
    public:

    static void addSystem(void (*update)(), const Duration& deltaTime);
    template<typename T>
    static void addSystem(void (*update)(const T&));
    static void removeSystem(void (*update)());
    template<typename T>
    static void removeSystem(void (*update)(const T&));
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
      Duration deltaTime;
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
    static std::vector<void(*)(const T&)> eventBasedSystem;
    template<typename T>
    static std::vector<T> eventQueue;
  };
  std::vector<SystemManager::TimeBasedSystem> SystemManager::timeBasedSystems = std::vector<SystemManager::TimeBasedSystem>();
  std::vector<void(*)()> SystemManager::runEventBasedSystemsList = std::vector<void(*)()>();
  template<typename T>
  std::vector<void(*)(const T&)> SystemManager::eventBasedSystem = std::vector<void(*)(const T&)>();
  template<typename T>
  std::vector<T> SystemManager::eventQueue = std::vector<T>();

  void SystemManager::addSystem(void (*update)(), const Duration& deltaTime)
  {
    timeBasedSystems.emplace_back(update, deltaTime);
  }
  template<typename T>
  void SystemManager::addSystem(void (*update)(const T&))
  {
    eventBasedSystem<T>.push_back(update);
  }
  void SystemManager::removeSystem(void (*update)())
  {
    for(size_t i = 0; i<timeBasedSystems.size(); ++i)
    {
      if(timeBasedSystems[i].update == update)
      {
        timeBasedSystems.erase(timeBasedSystems.begin() + i);
        --i;
      }
    }
  }
  template<typename T>
  void SystemManager::removeSystem(void (*update)(const T&))
  {
    for(size_t i = 0; i<eventBasedSystem<T>.size(); ++i)
    {
      if(eventBasedSystem<T>[i] == update)
      {
        eventBasedSystem<T>.erase(eventBasedSystem<T>.begin() + i);
        --i;
      }
    }
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
    //register event type
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

/*MIT License

Copyright (c) 2018 tsoj

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

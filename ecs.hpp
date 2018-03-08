#pragma once
#include <iostream>
#include <vector>
#include <chrono>

typedef size_t ID;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
const ID NULL_ID = (0-1);//(size_t)(0-1) == SIZE_MAX;
const ID END = NULL_ID;
const ID BEGIN = 0;
#pragma GCC diagnostic pop

/********************ENTITIES, COMPONENTS********************/

class EC
{
  public:

  static ID createEntity();
  static void removeEntity(ID entityID);

  template<typename T>
  static void createComponent(ID entityID, T component);
  template<typename T>
  static void removeComponent(ID entityID);
  template<typename T>
  static T & getComponent(ID entityID);
  template<typename T>
  static bool hasComponents(ID entityID);
  template<typename T1, typename T2, typename... Targs>
  static bool hasComponents(ID entityID);

  template<typename T, typename... Targs>
  class Iterator
  {
    private:

    ID counter;
    ID to;
    ID current = NULL_ID;

      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wunused-parameter"
      template<typename U>
      static bool initialHasComponents(ID entityID)
      {
        return true;
      }
      #pragma GCC diagnostic pop
      template<typename U1, typename U2, typename... Uargs>
      static bool initialHasComponents(ID entityID)
      {
        return hasComponents<U2, Uargs...>(entityID);
      }

    public:

    Iterator();
    Iterator(ID from, ID to);
    bool hasNext();
    ID next();
    ID getCurrentID();
  };

  private:

  EC(){}

  struct Entity
  {
    std::vector<void (*)(ID)> removeComponentFunctions = std::vector<void (*)(ID)>();
    bool exists = true;
  };

  static std::vector<Entity> entityArray;
  static std::vector<ID> freeIDs;

  template<typename T>
  static std::vector<T> componentArray;
  template<typename T>
  static std::vector<ID> componentToEntityIDs;
  template<typename T>
  static std::vector<ID> entityToComponentIDs;
};
template<typename T>
std::vector<T> EC::componentArray = std::vector<T>();
template<typename T>
std::vector<ID> EC::componentToEntityIDs = std::vector<ID>();
template<typename T>
std::vector<ID> EC::entityToComponentIDs = std::vector<ID>();

std::vector<ID> EC::freeIDs = std::vector<ID>();
std::vector<EC::Entity> EC::entityArray = std::vector<Entity>();

ID EC::createEntity()
{
  ID tempID;
  if(freeIDs.empty())
  {
    tempID = entityArray.size();
    entityArray.emplace_back();
  }
  else
  {
    tempID = freeIDs.back();
    freeIDs.pop_back();
    entityArray[tempID].exists = true;
  }
  return tempID;
}
void EC::removeEntity(ID entityID)
{
  freeIDs.push_back(entityID);
  for(void (*f)(ID) : entityArray[entityID].removeComponentFunctions)
  {
    f(entityID);
  }
  entityArray[entityID].removeComponentFunctions = std::vector<void (*)(ID)>();
  entityArray[entityID].exists = false;
}

template<typename T>
void EC::createComponent(ID entityID, T component)
{
  Entity & entity = entityArray[entityID];
  if(entityID >= entityToComponentIDs<T>.size())
  {
    entityToComponentIDs<T>.resize(entityID+1, NULL_ID);
    entityToComponentIDs<T>[entityID] = componentArray<T>.size();
    componentToEntityIDs<T>.push_back(entityID);
    componentArray<T>.push_back(component);
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
          componentArray<T>.insert(componentArray<T>.begin() + entityToComponentIDs<T>[i], component);
          componentToEntityIDs<T>.insert(componentToEntityIDs<T>.begin() + entityToComponentIDs<T>[i], entityID);
          entityToComponentIDs<T>[entityID] = entityToComponentIDs<T>[i];
          foundBiggerID = true;
        }
        entityToComponentIDs<T>[i] += 1;
      }
    }
  }
  entity.removeComponentFunctions.push_back(&removeComponent<T>);
}
template<typename T>
void EC::removeComponent(ID entityID)
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
T & EC::getComponent(ID entityID)
{
  return componentArray<T>[entityToComponentIDs<T>[entityID]];
}
template<typename T>
bool EC::hasComponents(ID entityID)
{
  return entityToComponentIDs<T>.size() > entityID && entityToComponentIDs<T>[entityID] != NULL_ID;
}
template<typename T1, typename T2, typename... Targs>
bool EC::hasComponents(ID entityID)
{
  return hasComponents<T1>(entityID) && hasComponents<T2, Targs...>(entityID);
}
template<>
bool EC::hasComponents<void>(ID entityID)
{
  return entityArray.size() > entityID && entityArray[entityID].exists;
}

template<typename T, typename... Targs>
EC::Iterator<T, Targs...>::Iterator()
{
  counter = 0;
  this->to = entityArray.size();
}
template<typename T, typename... Targs>
EC::Iterator<T, Targs...>::Iterator(ID from, ID to)
{
  counter = from;
  this->to = to == END ? entityArray.size() : to;
}
template<typename T, typename... Targs>
bool EC::Iterator<T, Targs...>::hasNext()
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
ID EC::Iterator<T, Targs...>::next()
{
  if(hasNext())
  {
    current = counter;
    counter+=1;
    return current;
  }
  return NULL_ID;
}
template<typename T, typename... Targs>
ID EC::Iterator<T, Targs...>::getCurrentID()
{
  return current;
}

/********************SYSTEMS********************/

class SystemManager
{
  public:

  static void addSystem(void (*update)(), std::chrono::milliseconds deltaTime);
  template<typename T>
  static void addSystem(void (*update)(T));
  static void runSystems();
  template<typename T>
  static void throwEvent(T event);

  private:

  struct TimeBasedSystem
  {
    TimeBasedSystem(void (*update)(), std::chrono::milliseconds deltaTime) : update(update), deltaTime(deltaTime){}
    void (*update)();
    std::chrono::milliseconds deltaTime;
  };
  struct eventRegisterHelper
  {
    explicit eventRegisterHelper(void (*f)())
    {
      runEventBasedSystemsList.emplace_back(f);
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

void SystemManager::addSystem(void (*update)(), std::chrono::milliseconds deltaTime)
{
  timeBasedSystems.emplace_back(update, deltaTime);
}
template<typename T>
void SystemManager::addSystem(void (*update)(T))
{
  eventBasedSystem<T>.emplace_back(update);
}
void SystemManager::runSystems()
{
  for(TimeBasedSystem system : timeBasedSystems)
  {
    if(true/*deltaTime condition*/)
    {
      system.update();
    }
  }
  for(auto f : runEventBasedSystemsList)
  {
    f();
  }
}
template<typename T>
void SystemManager::throwEvent(T event)
{
  //register event
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-variable"
  static eventRegisterHelper _(&runEventBasedSystems<T>);
  #pragma GCC diagnostic pop

  eventQueue<T>.push_back(event);
}
template<typename T>
void SystemManager::runEventBasedSystems()
{
  for(T & event : eventQueue<T>)
  {
    for(auto system : eventBasedSystem<T>)
    {
      system(event);
    }
  }
  eventQueue<T>.clear();
}
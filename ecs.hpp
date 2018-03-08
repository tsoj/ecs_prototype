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

/********************ENTITYS, COMPONENTS********************/

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

  template<typename T>
  class Component
  {
    friend class EC;

    private:

    static std::vector<T> componentArray;
    static std::vector<ID> componentToEntityIDs;
    static std::vector<ID> entityToComponentIDs;
  };

  private:

  EC(){}

  class Entity
  {
    public:

    static std::vector<Entity> entityArray;
    static std::vector<ID> freeIDs;

    Entity()
    {
      exists = true;
    }

    std::vector<void (*)(ID)> removeComponentFunctions = std::vector<void (*)(ID)>();
    bool exists = false;
  };

};
template<typename T>
std::vector<T> EC::Component<T>::componentArray = std::vector<T>();
template<typename T>
std::vector<ID> EC::Component<T>::componentToEntityIDs = std::vector<ID>();
template<typename T>
std::vector<ID> EC::Component<T>::entityToComponentIDs = std::vector<ID>();

std::vector<ID> EC::Entity::freeIDs = std::vector<ID>();
std::vector<EC::Entity> EC::Entity::entityArray = std::vector<Entity>();

ID EC::createEntity()
{
  ID tempID;
  if(Entity::freeIDs.empty())
  {
    tempID = Entity::entityArray.size();
    Entity::entityArray.emplace_back();
  }
  else
  {
    tempID = Entity::freeIDs.back();
    Entity::freeIDs.pop_back();
    Entity::entityArray[tempID].exists = true;
  }
  return tempID;
}
void EC::removeEntity(ID entityID)
{
  Entity::freeIDs.push_back(entityID);
  for(void (*f)(ID) : Entity::entityArray[entityID].removeComponentFunctions)
  {
    f(entityID);
  }
  Entity::entityArray[entityID].removeComponentFunctions = std::vector<void (*)(ID)>();
  Entity::entityArray[entityID].exists = false;
}

template<typename T>
void EC::createComponent(ID entityID, T component)
{
  Entity & entity = Entity::entityArray[entityID];
  if(entityID >= Component<T>::entityToComponentIDs.size())
  {
    Component<T>::entityToComponentIDs.resize(entityID+1, NULL_ID);
    Component<T>::entityToComponentIDs[entityID] = Component<T>::componentArray.size();
    Component<T>::componentToEntityIDs.push_back(entityID);
    Component<T>::componentArray.push_back(component);
  }
  else if(entityID < Component<T>::entityToComponentIDs.size())
  {
    bool foundBiggerID = false;
    for(ID i = entityID + 1; i < Component<T>::entityToComponentIDs.size(); i++)
    {
      if(Component<T>::entityToComponentIDs[i] != NULL_ID)
      {
        if(foundBiggerID == false)
        {
          Component<T>::componentArray.insert(Component<T>::componentArray.begin() + Component<T>::entityToComponentIDs[i], component);
          Component<T>::componentToEntityIDs.insert(Component<T>::componentToEntityIDs.begin() + Component<T>::entityToComponentIDs[i], entityID);
          Component<T>::entityToComponentIDs[entityID] = Component<T>::entityToComponentIDs[i];
          foundBiggerID = true;
        }
        Component<T>::entityToComponentIDs[i] += 1;
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
  Component<T>::componentArray.erase(Component<T>::componentArray.begin() + Component<T>::entityToComponentIDs[entityID]);
  Component<T>::componentToEntityIDs.erase(Component<T>::componentToEntityIDs.begin() + Component<T>::entityToComponentIDs[entityID]);
  for(ID i = entityID; i<Component<T>::entityToComponentIDs.size(); i++)
  {
    if(Component<T>::entityToComponentIDs[i] != NULL_ID)
    {
      Component<T>::entityToComponentIDs[i] -= 1;
    }
  }
  Component<T>::entityToComponentIDs[entityID] = NULL_ID;
}
template<typename T>
T & EC::getComponent(ID entityID)
{
  return Component<T>::componentArray[Component<T>::entityToComponentIDs[entityID]];
}
template<typename T>
bool EC::hasComponents(ID entityID)
{
  return Component<T>::entityToComponentIDs.size() > entityID && Component<T>::entityToComponentIDs[entityID] != NULL_ID;
}
template<typename T1, typename T2, typename... Targs>
bool EC::hasComponents(ID entityID)
{
  return hasComponents<T1>(entityID) && hasComponents<T2, Targs...>(entityID);
}
template<>
bool EC::hasComponents<void>(ID entityID)
{
  return Entity::entityArray.size() > entityID && Entity::entityArray[entityID].exists;
}

template<typename T, typename... Targs>
EC::Iterator<T, Targs...>::Iterator()
{
  counter = 0;
  this->to = Entity::entityArray.size();
}
template<typename T, typename... Targs>
EC::Iterator<T, Targs...>::Iterator(ID from, ID to)
{
  counter = from;
  this->to = to == END ? Entity::entityArray.size() : to;
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

#define COMPONENT(name) struct name : public EC::Component<name>

/********************SYSTEMS********************/

class SystemManager
{
  public:

  static void addSystem(void (*update)(), std::chrono::milliseconds deltaTime)
  {
    timeBasedSystems.emplace_back(update, deltaTime);
  }
  template<typename T>
  static void addSystem(void (*update)(T))
  {
    eventBasedSystem<T>.emplace_back(update);
  }
  template<typename T>
  static void registerEvent()
  {
    runEventBasedSystemsList.emplace_back(&runEventBasedSystems<T>);
  }
  static void runSystems()
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

  private:

  struct TimeBasedSystem
  {
    TimeBasedSystem(void (*update)(), std::chrono::milliseconds deltaTime) : update(update), deltaTime(deltaTime){}
    void (*update)();
    std::chrono::milliseconds deltaTime;
  };
  template<typename T>
  struct EventBasedSystem
  {
    EventBasedSystem(void (*update)(T)) : update(update){}
    void (*update)(T);
  };
  template<typename T>
  static void runEventBasedSystems()
  {
    while(eventBasedSystem<T>.size > 0)
    {
      EventBasedSystem<T> & event = eventBasedSystem<T>.back();
      for(EventBasedSystem<T> system : eventBasedSystem<T>)
      {
        system.update(event);
      }
      eventBasedSystem<T>.pop_back();
    }
  }

  static std::vector<TimeBasedSystem> timeBasedSystems;
  static std::vector<void(*)()> runEventBasedSystemsList;
  template<typename T>
  static std::vector<EventBasedSystem<T>> eventBasedSystem;
  template<typename T>
  static std::vector<T> eventQueue;
};

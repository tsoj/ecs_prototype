#pragma once
#include <iostream>
#include <vector>

typedef size_t ID;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
const ID NULL_ID = (0-1);//(size_t)(0-1) == SIZE_MAX;
const ID END = NULL_ID;
const ID BEGIN = 0;
#pragma GCC diagnostic pop

#define COMPONENT(name) struct name : public EC::Component<name>

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
  static bool hasComponent(ID entityID);

  template<typename T, typename... Targs>
  class Iterator
  {
    private:

    ID counter;
    ID to;

    public:

    Iterator();
    Iterator(ID from, ID to);
    ID next();
    bool hasNext();
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

  template<typename T>
  static bool has(ID entityID)
  {
    return Component<T>::entityToComponentIDs.size() > entityID && Component<T>::entityToComponentIDs[entityID] != NULL_ID;
  }
  template<typename T1, typename T2, typename... Targs>
  static bool has(ID entityID)
  {
    return has<T1>(entityID) & has<T2, Targs...>(entityID);
  }
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  template<typename T>
  static bool initialHas(ID entityID)
  {
    return true;
  }
  #pragma GCC diagnostic pop
  template<typename T1, typename T2, typename... Targs>
  static bool initialHas(ID entityID)
  {
    return has<T2, Targs...>(entityID);
  }
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
  if(!hasComponent<T>(entityID))
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
bool EC::hasComponent(ID entityID)
{
  return Component<T>::entityToComponentIDs.size() > entityID && Component<T>::entityToComponentIDs[entityID] != NULL_ID;
}

template<typename T, typename... Targs>
EC::Iterator<T, Targs...>::Iterator()
{
  counter = 0;
  this->to = Component<T>::componentArray.size();
}
template<typename T, typename... Targs>
EC::Iterator<T, Targs...>::Iterator(ID from, ID to)
{
  counter = from;
  this->to = to == END ? Component<T>::componentArray.size() : to;
}
template<typename T, typename... Targs>
ID EC::Iterator<T, Targs...>::next()
{
  while(to>counter)
  {
    ID currentEntityID = Component<T>::componentToEntityIDs[counter];
    counter+=1;
    if(!initialHas<T, Targs...>(currentEntityID))
    {
      continue;
    }
    return currentEntityID;
  }
  return NULL_ID;
}
template<typename T, typename... Targs>
bool EC::Iterator<T, Targs...>::hasNext()
{
  while(to>counter)
  {
    ID currentEntityID = Component<T>::componentToEntityIDs[counter];
    if(!initialHas<T, Targs...>(currentEntityID))
    {
      counter+=1;
      continue;
    }
    return true;
  }
  return false;
}
template<typename T, typename... Targs>
ID EC::Iterator<T, Targs...>::getCurrentID()
{
  return counter-1;
}

template<>
EC::Iterator<void>::Iterator()
{
  counter = 0;
  this->to = Entity::entityArray.size();
}
template<>
EC::Iterator<void>::Iterator(ID from, ID to)
{
  counter = from;
  this->to = to == END ? Entity::entityArray.size() : to;
}
template<>
ID EC::Iterator<void>::next()
{
  while(to>counter)
  {
    ID currentEntityID = counter;
    counter+=1;
    if(Entity::entityArray[currentEntityID].exists==false)
    {
      continue;
    }
    return currentEntityID;
  }
  return NULL_ID;
}
template<>
bool EC::Iterator<void>::hasNext()
{
  while(to>counter)
  {
    ID currentEntityID = counter;
    if(Entity::entityArray[currentEntityID].exists==false)
    {
      counter+=1;
      continue;
    }
    return true;
  }
  return false;
}

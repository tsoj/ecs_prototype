#include "entity.hpp"

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
std::vector<ID> Entity::freeIDs = std::vector<ID>();
std::vector<Entity> Entity::entityArray = std::vector<Entity>();

ID createEntity()
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
void removeEntity(ID entityID)
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
void createComponent(ID entityID, T component)
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
void removeComponent(ID entityID)
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
T & getComponent(ID entityID)
{
  return Component<T>::componentArray[Component<T>::entityToComponentIDs[entityID]];
}
template<typename T>
bool hasComponent(ID entityID)
{
  return Component<T>::entityToComponentIDs.size() > entityID && Component<T>::entityToComponentIDs[entityID] != NULL_ID;
}


template<typename T>
bool has(ID entityID)
{
  return Component<T>::entityToComponentIDs.size() > entityID && Component<T>::entityToComponentIDs[entityID] != NULL_ID;
}
template<typename T1, typename T2, typename... Targs>
bool has(ID entityID)
{
  return has<T1>(entityID) & has<T2, Targs...>(entityID);
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template<typename T>
bool initialHas(ID entityID)
{
  return true;
}
#pragma GCC diagnostic pop
template<typename T1, typename T2, typename... Targs>
bool initialHas(ID entityID)
{
  return has<T2, Targs...>(entityID);
}

template<typename T, typename... Targs>
Iterator<T, Targs...>::Iterator()
{
  counter = 0;
  this->to = Component<T>::componentArray.size();
}
template<typename T, typename... Targs>
Iterator<T, Targs...>::Iterator(ID from, ID to)
{
  counter = from;
  this->to = to == END ? Component<T>::componentArray.size() : to;
}
template<typename T, typename... Targs>
ID Iterator<T, Targs...>::next()
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
bool Iterator<T, Targs...>::hasNext()
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
ID Iterator<T, Targs...>::getCurrentID()
{
  return counter-1;
}

template<>
Iterator<void>::Iterator()
{
  counter = 0;
  this->to = Entity::entityArray.size();
}
template<>
Iterator<void>::Iterator(ID from, ID to)
{
  counter = from;
  this->to = to == END ? Entity::entityArray.size() : to;
}
template<>
ID Iterator<void>::next()
{
  while(to>counter)
  {
    ID currentEntityID = counter;
    counter+=1;
    if(currentEntityID==NULL_ID)
    {
      continue;
    }
    return currentEntityID;
  }
  return NULL_ID;
}
template<>
bool Iterator<void>::hasNext()
{
  while(to>counter)
  {
    ID currentEntityID = counter;
    if(currentEntityID==NULL_ID)
    {
      counter+=1;
      continue;
    }
    return true;
  }
  return false;
}

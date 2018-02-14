#pragma once
#include <iostream>
#include <vector>

typedef size_t ID;
const ID NULL_ID = (0-1);//(size_t)(0-1) == SIZE_MAX;

template<typename T>
class Component;
class Entity;
class EntityPtr;
template<typename T>
class Iterator;

class EntityPtr
{
  template<typename T>
  friend class Component;
  friend class Entity;
  template<typename T>
  friend class Iterator;

  private:

  EntityPtr(ID id) : id(id) {}
  ID id;
  Entity & getReference();

  public:

  Entity * operator->() const;
};

class Entity
{
  template<typename T>
  friend class Component;
  friend class EntityPtr;

  private:

  static std::vector<Entity> entityArray;
  static std::vector<ID> freeIDs;

  std::vector<void (*)(EntityPtr &)> removeFunctions = std::vector<void (*)(EntityPtr &)>();
  Entity(ID id) : id(id) {}
  ID id;

  public:

  static EntityPtr create()
  {
    ID tempID;
    if(freeIDs.empty())
    {
      tempID = entityArray.size();
      entityArray.push_back(Entity(tempID));
    }
    else
    {
      tempID = freeIDs.back();
      freeIDs.pop_back();
      entityArray[tempID] = tempID;
    }
    return EntityPtr(tempID);
  }
  void remove()
  {
    freeIDs.push_back(id);
    EntityPtr tempPtr = EntityPtr(id);
    for(void (*f)(EntityPtr&) : removeFunctions)
    {
      f(tempPtr);
    }
    removeFunctions = std::vector<void (*)(EntityPtr &)>();
    id = NULL_ID;
  }
};
std::vector<ID> Entity::freeIDs = std::vector<ID>();
std::vector<Entity> Entity::entityArray = std::vector<Entity>();

Entity * EntityPtr::operator->() const
{
  return  &Entity::entityArray[id];
}
Entity & EntityPtr::getReference()
{
  return Entity::entityArray[id];
}

#define COMPONENT(name) struct name : public Component<name>

template<typename T>
class Component
{
  friend class Iterator<T>;

  private:

  static std::vector<T> componentArray;
  static std::vector<ID> componentToEntityIDs;
  static std::vector<ID> entityToComponentIDs;

  public:

  static void create(EntityPtr & entityPtr, T t)
  {
    Entity & entity = entityPtr.getReference();
    if(entity.id >= entityToComponentIDs.size())
    {
      entityToComponentIDs.resize(entity.id+1, NULL_ID);
      entityToComponentIDs[entity.id] = componentArray.size();
      componentToEntityIDs.push_back(entity.id);
      componentArray.push_back(t);
    }
    else if(entity.id < entityToComponentIDs.size())
    {

      bool found = false;
      for(ID i = entity.id + 1; i<entityToComponentIDs.size(); i++)
      {
        if(entityToComponentIDs[i] != NULL_ID)
        {
          if(!found)
          {
            std::cout << entityToComponentIDs[i] << std::endl;
            componentArray.insert(componentArray.begin() + entityToComponentIDs[i], t);
            componentToEntityIDs.insert(componentToEntityIDs.begin() + entityToComponentIDs[i], entity.id);
            entityToComponentIDs[entity.id] = entityToComponentIDs[i];
            found = true;
          }
          entityToComponentIDs[i] += 1;
        }
      }
    }
    entity.removeFunctions.push_back(&remove);
  }
  static void remove(EntityPtr & entityPtr)
  {
    Entity & entity = entityPtr.getReference();
    if(!has(entityPtr))
    {
      return;
    }
    componentArray.erase(componentArray.begin() + entityToComponentIDs[entity.id]);
    componentToEntityIDs.erase(componentToEntityIDs.begin() + entityToComponentIDs[entity.id]);
    for(ID i = entity.id; i<entityToComponentIDs.size(); i++)
    {
      if(entityToComponentIDs[i] != NULL_ID)
      {
        entityToComponentIDs[i] -= 1;
      }
    }
  }
  static T & get(EntityPtr & entityPtr)
  {
    return componentArray[entityToComponentIDs[entityPtr->id]];
  }
  static bool has(EntityPtr & entityPtr)
  {
    return entityToComponentIDs[entityPtr->id] != NULL_ID;
  }
};
template<typename T>
std::vector<T> Component<T>::componentArray = std::vector<T>();
template<typename T>
std::vector<ID> Component<T>::componentToEntityIDs = std::vector<ID>();
template<typename T>
std::vector<ID> Component<T>::entityToComponentIDs = std::vector<ID>();

template<typename T>
class Iterator
{
  private:

  ID counter;
  ID to;

  public:

  static const ID END = NULL_ID;
  static const ID BEGIN = 0;

  Iterator()
  {
    counter = 0;
    this->to = Component<T>::componentArray.size();
  }
  Iterator(ID from, ID to)
  {
    counter = from;
    this->to = to == END ? Component<T>::componentArray.size() : to;
  }
  EntityPtr next()
  {
    while(to>counter)
    {
      counter+=1;
      return EntityPtr(Component<T>::componentToEntityIDs[counter-1]);
    }
    return EntityPtr(NULL_ID);
  }
  bool hasNext()
  {
    return to>counter;
  }
};

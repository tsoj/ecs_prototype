#pragma once
#include <iostream>
#include <vector>

typedef int ID;
const ID NULL_ID = -1;

#define COMPONENT(name)\
public:\
struct name;\
Component<name> name ## _component = Component<name>(&entity_id);\
struct name

template<typename ComponentData>
class Component
{
public:
  friend class Entity;
  static std::vector<ComponentData> array;
  static std::vector<ID> entity_ids;
  static std::vector<ID> free_ids;
  ID component_id = NULL_ID;
  const ID * entity_id;
public:
  Component(ID * entity_id)
  {
    this->entity_id = entity_id;
  }
  void set(ComponentData a)
  {
    if(free_ids.size()>0)
    {
      component_id = free_ids.back();
      free_ids.pop_back();
      array[component_id] = a;
      entity_ids[component_id] = *entity_id;
    }
    else
    {
      component_id = array.size();
      array.push_back(a);
      entity_ids.push_back(*entity_id);
    }
  }
  ComponentData get()
  {
    return array[component_id];
  }
  bool has()
  {
    return component_id!=NULL_ID;
  }
  void remove()
  {
    entity_ids[component_id] = NULL_ID;
    free_ids.push_back(component_id);
  }
  class Iterator
  {
  public:
    Iterator()
    {
      counter = 0;
    }
  private:
    int counter;
  public:
    ID next_entity_id()
    {
      while(true)
      {
        if(counter<entity_ids.size())
        {
          counter +=1;
          if(entity_ids[counter - 1] != NULL_ID)
          {
            return entity_ids[counter - 1];
          }
        }
        else
        {
          break;
        }
      }
      return NULL_ID;
    }
  };
};
template<typename ComponentData>
std::vector<ComponentData> Component<ComponentData>::array = std::vector<ComponentData>();
template<typename ComponentData>
std::vector<ID> Component<ComponentData>::entity_ids = std::vector<ID>();
template<typename ComponentData>
std::vector<ID> Component<ComponentData>::free_ids = std::vector<ID>();

class Entity
{
public:
  COMPONENT(Position)
  {
    double x;
    double y;
  };

private:
  static std::vector<Entity*> entity_array;
  static std::vector<ID> free_entity_ids;
  ID entity_id = NULL_ID;

public:
  Entity()
  {
    if(Entity::free_entity_ids.size()>0)
    {
      entity_id = free_entity_ids.back();
      free_entity_ids.pop_back();
      entity_array[entity_id] = this;
    }
    else
    {
      entity_id = entity_array.size();
      entity_array.push_back(this);
    }
  }
  ~Entity()
  {
    remove();
  }
  static Entity* get_entity(ID entity_id)
  {
    return entity_array[entity_id];
  }
  void remove()
  {
    entity_array[entity_id] = nullptr;
    free_entity_ids.push_back(entity_id);
  }
};
std::vector<Entity*> Entity::entity_array = std::vector<Entity*>();
std::vector<ID> Entity::free_entity_ids = std::vector<ID>();

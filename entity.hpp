#pragma once
#include <iostream>
#include <vector>

typedef long ID;
const ID NULL_ID = -1;

#define COMPONENT(name)\
public:\
struct name;\
Component<name> c_ ## name = Component<name>();\
void init_ ## name(name a)\
{\
  c_ ## name.init(entity_id, a);\
  removeables_offset.push_back((char*)(&c_ ## name) - (char*)this);\
}\
struct name

class Entity
{
  private:

  class Removeable
  {
    public:
    virtual void remove()
    {
      std::cout << "f***" << std::endl;
    }
  };

  template<typename T>
  class Component : public Removeable
  {
    friend class Entity;
    friend class Iterator;

    private:

    ID entity_id = NULL_ID;
    ID component_id = NULL_ID;
    static std::vector<T> component_array;
    static std::vector<ID> entity_ids;
    static std::vector<ID> component_gaps;
    void init(ID entity_id, T a)
    {
      this->entity_id = entity_id;
      if(component_gaps.empty())
      {
        component_id = component_array.size();
        component_array.push_back(a);
        entity_ids.push_back(entity_id);
      }
      else
      {
        component_id = component_gaps.back();
        component_gaps.pop_back();
        component_array[component_id] = a;
      }
    }

    public:

    T & get()
    {
      return component_array[component_id];
    }
    ID get_id()
    {
      return component_id;
    }
    bool has()
    {
      return (component_id != NULL_ID);
    }
    void remove()
    {
      entity_ids[component_id] = NULL_ID;
      component_gaps.push_back(component_id);
      component_id = NULL_ID;
    }
  };
  public:
  template<typename Data>
  class Iterator
  {
    private:

    ID counter = NULL_ID;
    ID to = NULL_ID;

    public:

    const ID END = NULL_ID;
    const ID BEGIN = 0;

    Iterator()
    {
      counter = 0;
      this->to = Component<Data>::entity_ids.size();
    }
    Iterator(ID from, ID to)
    {
      counter = from;
      this->to = to==END ? Component<Data>::entity_ids.size() : to;
    }
    ID next()
    {
      while(to>counter)
      {
        if(Component<Data>::entity_ids[counter]==NULL_ID)
        {
          counter+=1;
          continue;
        }
        counter+=1;
        return Entity::get(Component<Data>::entity_ids[counter-1]).get_id();
      }
      return NULL_ID;
    }
    bool has_next()
    {
      while(to>counter)
      {
        if(Component<Data>::entity_ids[counter]==NULL_ID)
        {
          counter+=1;
          continue;
        }
        return true;
      }
      return false;
    }
  };

  COMPONENT(Position)
  {
    double x;
    double y;
  };
  COMPONENT(Mass)
  {
    double m;
  };

  private:

  static std::vector<Entity> entity_array;
  static std::vector<ID> entity_gaps;
  ID entity_id = NULL_ID;
  std::vector<long> removeables_offset = std::vector<long>();

  public:

  Entity(ID entity_id)
  {
    this->entity_id = entity_id;
  }
  ID get_id()
  {
    return entity_id;
  }
  void remove()
  {
    for(int i = 0; i < removeables_offset.size(); i++)
    {
      ((Removeable*)((long)this + removeables_offset[i]))->remove();
    }
    entity_gaps.push_back(entity_id);
    this->~Entity();
  }
  static ID create()
  {
    ID entity_id;
    if(entity_gaps.empty())
    {
      entity_id = entity_array.size();
      entity_array.emplace_back(entity_id);
    }
    else
    {
      entity_id = entity_gaps.back();
      entity_gaps.pop_back();
      entity_array[entity_id] = Entity(entity_id);
    }
    return entity_id;
  }
  static Entity & get(ID id)
  {
    return entity_array[id];
  }
};

std::vector<Entity> Entity::entity_array = std::vector<Entity>();
std::vector<ID> Entity::entity_gaps = std::vector<ID>();
template<typename T>
std::vector<T> Entity::Component<T>::component_array = std::vector<T>();
template<typename T>
std::vector<ID> Entity::Component<T>::entity_ids = std::vector<ID>();
template<typename T>
std::vector<ID> Entity::Component<T>::component_gaps = std::vector<ID>();

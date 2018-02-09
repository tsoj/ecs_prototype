#pragma once
#include <iostream>
#include <vector>

typedef long Index;
const Index NULL_INDEX = -1;

#define COMPONENT(name)\
public:\
struct name;\
Component<name> c_ ## name = Component<name>();\
void init_ ## name(name a)\
{\
  c_ ## name.init(entity_index, a);\
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
      std::cout << "****" << std::endl;
    }
  };

  template<typename T>
  class Component : public Removeable
  {
    friend class Entity;
    private:
    Index entity_index = NULL_INDEX;
    Index component_index = NULL_INDEX;
    public:
    static std::vector<T> component_array;
    static std::vector<Index> entity_indexes;
    static std::vector<Index> component_gaps;
    private:
    void init(Index entity_index, T a)
    {
      this->entity_index = entity_index;
      if(component_gaps.empty())
      {
        component_index = component_array.size();
        component_array.push_back(a);
        entity_indexes.push_back(entity_index);
      }
      else
      {
        component_index = component_gaps.back();
        component_gaps.pop_back();
        component_array[component_index] = a;
      }
    }
    public:
    T & get()
    {
      return component_array[component_index];
    }
    bool has()
    {
      return (component_index != NULL_INDEX);
    }
    void remove()
    {
      std::cout << "size: "<<sizeof(T)<<std::endl;
      entity_indexes[component_index] = NULL_INDEX;
      component_gaps.push_back(component_index);
      component_index = NULL_INDEX;
    }
  };
  public:
  template<typename Data>
  class Iterator
  {
    private:
    Index counter = NULL_INDEX;
    Index to = NULL_INDEX;
    public:
    Iterator()
    {
      counter = 0;
      this->to = Component<Data>::entity_indexes.size();
    }
    Iterator(Index from)
    {
      counter = from;
      this->to = Component<Data>::entity_indexes.size();
    }
    Iterator(Index from, Index to)
    {
      counter = from;
      this->to = to;
    }
    Entity* next_entity_reference()
    {
      while(Component<Data>::entity_indexes.size()>counter && to>counter)
      {
        if(Component<Data>::entity_indexes[counter]==NULL_INDEX)
        {
          counter+=1;
          continue;
        }
        counter+=1;
        return &Entity::get(Component<Data>::entity_indexes[counter-1]);
      }
      return nullptr;
    }
    bool has_next()
    {
      //TODO: what if next element is empty?
      while(Component<Data>::entity_indexes.size()>counter && to>counter)
      {
        if(Component<Data>::entity_indexes[counter]==NULL_INDEX)
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
  static std::vector<Index> entity_gaps;
  Index entity_index = NULL_INDEX;
  std::vector<long> removeables_offset = std::vector<long>();
  public:
  Entity(Index entity_index)
  {
    this->entity_index = entity_index;
  }
  static Index create()
  {
    Index temp_index;
    if(entity_gaps.empty())
    {
      temp_index = entity_array.size();
      entity_array.emplace_back(temp_index);
    }
    else
    {
      temp_index = entity_gaps.back();
      entity_gaps.pop_back();
      entity_array[temp_index] = Entity(temp_index);
    }
    return temp_index;
  }
  static Entity & get(Index index)
  {
    return entity_array[index];
  }
  void remove()
  {
    for(int i = 0; i < removeables_offset.size(); i++)
    {
      ((Removeable*)((long)this + removeables_offset[i]))->remove();
    }
    entity_gaps.push_back(entity_index);
    this->~Entity();
  }
};

std::vector<Entity> Entity::entity_array = std::vector<Entity>();
std::vector<Index> Entity::entity_gaps = std::vector<Index>();
template<typename T>
std::vector<T> Entity::Component<T>::component_array = std::vector<T>();
template<typename T>
std::vector<Index> Entity::Component<T>::entity_indexes = std::vector<Index>();
template<typename T>
std::vector<Index> Entity::Component<T>::component_gaps = std::vector<Index>();

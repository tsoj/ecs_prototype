#pragma once
#include <iostream>
#include <vector>

typedef long Index;
const Index NULL_INDEX = -1;

#define COMPONENT(name)\
public:\
struct name;\
Component<name> name ## _component = Component<name>(this);\
struct name

class Entity
{
  private:
  template<typename T>
  class Component
  {
    private:
    Entity* entity_pointer;
    Index component_index = NULL_INDEX;
    public:
    static std::vector<T> component_array;
    static std::vector<Entity*> entity_pointers;
    static std::vector<Index> component_gaps;
    Component(Entity* entity_pointer)
    {
      this->entity_pointer = entity_pointer;
    }
    void set(T a)
    {
      if(component_gaps.empty())
      {
        component_index = component_array.size();
        component_array.push_back(a);
        entity_pointers.push_back(entity_pointer);
      }
      else
      {
        component_index = component_gaps.back();
        component_gaps.pop_back();
        component_array[component_index] = a;
      }
      std::cout << component_index << " "<<this<<std::endl;
    }
    T & get()
    {
      std::cout << component_index << " "<<this<<std::endl;
      return component_array[component_index];
    }
    bool has()
    {
      return (component_index != NULL_INDEX);
    }
    void remove()
    {
      entity_pointers[component_index] = nullptr;
      component_gaps.push_back(component_index);
      component_index = NULL_INDEX;
    }
    static void res()
    {
      //component_array.reserve(50);
    }
    static void print()
    {
      std::cout << "Entity Address:\tComponent Address:\tIndex:"<<std::endl;
      for(int i = 0; i < component_array.size(); i++)
      {
        std::cout << entity_pointers[i] << "\t" << std::endl;
      }
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
      this->to = Component<Data>::entity_pointers.size();
    }
    Iterator(Index from)
    {
      counter = from;
      this->to = Component<Data>::entity_pointers.size();
    }
    Iterator(Index from, Index to)
    {
      counter = from;
      this->to = to;
    }
    Entity* next_entity_reference()
    {
      while(Component<Data>::entity_pointers.size()>counter && to>counter)
      {
        if(Component<Data>::entity_pointers[counter]==nullptr)
        {
          counter+=1;
          continue;
        }
        counter+=1;
        return Component<Data>::entity_pointers[counter-1];
      }
      return nullptr;
    }
    bool has_next()
    {
      return Component<Data>::entity_pointers.size()>counter && to>counter;
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
  static std::vector<Entity*> entity_gaps;
  public:
  Entity()
  {
    k=1;
  }
  int k=0;
  static Entity & create()
  {
    Entity* entity_ptr;
    if(entity_gaps.empty())
    {
      entity_array.emplace_back();
      entity_ptr = & entity_array.back();
    }
    else
    {
      entity_ptr = entity_gaps.back();
      entity_gaps.pop_back();
      *entity_ptr = Entity();
    }
    return *entity_ptr;
  }
  void print_this()
  {
  }
  void remove()
  {
    entity_gaps.push_back(this);
    this->~Entity();
  }
  static void res()
  {
    Component<Position>::res();
  }
};


std::vector<Entity> Entity::entity_array = std::vector<Entity>();
std::vector<Entity*> Entity::entity_gaps = std::vector<Entity*>();
template<typename T>
std::vector<T> Entity::Component<T>::component_array = std::vector<T>();
template<typename T>
std::vector<Entity*> Entity::Component<T>::entity_pointers = std::vector<Entity*>();
template<typename T>
std::vector<Index> Entity::Component<T>::component_gaps = std::vector<Index>();

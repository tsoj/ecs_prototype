#pragma once
#include <iostream>
#include <vector>

typedef int ID;
const ID NULL_ID = -1;

#define COMPONENT(name)\
  public:\
struct name;\
Component<name> name ## _component = Component<name>(this);\
struct name

class Entity
{
  private:
  template<typename Data>
  class Component
  {
    public:
    static std::vector<Data> data_array;
    static std::vector<Entity*> entity_pointers;
    static std::vector<ID> free_component_ids;

    ID component_id = NULL_ID;
    Entity * entity_pointer;
    Component(Entity * entity_pointer)
    {
      this->entity_pointer = entity_pointer;
    }
    void set(Data a)
    {
      if(free_component_ids.empty())
      {
        component_id = data_array.size();
        data_array.push_back(a);
        entity_pointers.push_back(entity_pointer);
      }
      else
      {
        component_id = free_component_ids.back();
        free_component_ids.pop_back();
        data_array[component_id] = a;
        entity_pointers[component_id] = entity_pointer;
      }
      entity_pointer->component_pointers.push_back(this);
    }
    Data & get_reference()
    {
      return data_array[component_id];
    }
    ID get_id()
    {
      return component_id;
    }
    bool has()
    {
      return (component_id!=NULL_ID);
    }
    void remove()
    {
      entity_pointers[component_id] = nullptr;
      component_id = NULL_ID;
      free_component_ids.push_back(component_id);
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
    Iterator()
    {
      counter = 0;
      this->to = Component<Data>::entity_pointers.size();
    }
    Iterator(ID from)
    {
      counter = from;
      this->to = Component<Data>::entity_pointers.size();
    }
    Iterator(ID from, ID to)
    {
      counter = from;
      this->to = to;
    }
    Entity* next_entity_pointer()
    {
      while(Component<Data>::entity_pointers.size()>counter && to>counter)
      {
        if(Component<Data>::entity_pointers[counter]==nullptr)
        {
          counter+=1;
          continue;
        }
        counter+=1;
        std::cout << counter<<" "<<Component<Data>::entity_pointers[counter-1]->get_id()<<std::endl;
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

  COMPONENT(NullComponent)
  {
  };

  private:
  friend class EntityIterator;
  template<typename Data>
  friend class Component;
  static std::vector<Entity> entity_array;
  static std::vector<ID> free_entity_ids;
  ID entity_id = NULL_ID;
  std::vector<void*> component_pointers = std::vector<void*>();
  Entity(ID entity_id)
  {
    this->entity_id = entity_id;
  }
  public:
  static ID create()
  {
    ID temp;
    if(free_entity_ids.empty())
    {
      temp = entity_array.size();
      entity_array.push_back(temp);
    }
    else
    {
      temp = free_entity_ids.back();
      free_entity_ids.pop_back();
      entity_array[temp] = Entity(temp);
    }
    return temp;
  }
  ID get_id()
  {
    return entity_id;
  }
  static Entity& get(ID entity_id)
  {
    return entity_array[entity_id];
  }
  void remove()
  {
    NullComponent_component.set(NullComponent{});
    for(int i = 0; i<component_pointers.size(); i++)
    {
      ((Component<NullComponent>*)component_pointers[i])->remove();
    }
    free_entity_ids.push_back(entity_id);
    entity_id = NULL_ID;
    this->~Entity();
  }
};
class EntityIterator
{
  private:
  ID counter = NULL_ID;
  ID to = NULL_ID;
  public:
  EntityIterator()
  {
    counter = 0;
    this->to = Entity::entity_array.size();
  }
  EntityIterator(ID from)
  {
    counter = from;
    this->to = Entity::entity_array.size();
  }
  EntityIterator(ID from, ID to)
  {
    counter = from;
    this->to = to;
  }
  Entity* next_entity_pointer()
  {
    while(Entity::entity_array.size()>counter && to>counter)
    {
      if(Entity::entity_array[counter].entity_id == NULL_ID)
      {
        counter+=1;
        continue;
      }
      counter+=1;
      return &Entity::entity_array[counter-1];
    }
    return nullptr;
  }
  bool has_next()
  {
    return Entity::entity_array.size()>counter && to>counter;
  }

};

std::vector<Entity> Entity::entity_array = std::vector<Entity>();
std::vector<ID> Entity::free_entity_ids = std::vector<ID>();

template<typename Data>
std::vector<Data> Entity::Component<Data>::data_array = std::vector<Data>();
template<typename Data>
std::vector<Entity*> Entity::Component<Data>::entity_pointers = std::vector<Entity*>();
template<typename Data>
std::vector<ID> Entity::Component<Data>::free_component_ids = std::vector<ID>();

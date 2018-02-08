#pragma once
#include <iostream>
#include <vector>

typedef int ID;
const ID NULL_ID = -1;

#define COMPONENT(name)\
public:\
struct name;\
Component<name> name ## _component = Component<name>(entity_id);\
struct name

template<typename ComponentData>
class Component;

class Entity
{
public:
  template<typename ComponentData>
  class Component
  {
  private:
    friend class Entity;
    static std::vector<ComponentData> array;
    static std::vector<ID> entity_ids;
    static std::vector<ID> free_ids;
    ID component_id = NULL_ID;
    const Entity * const entity;
  public:
    Component(const Entity * const entity)
    {
      this->entity = entity;
    }
    void set(ComponentData a)
    {
      if(free_ids.size()>0)
      {
        component_id = free_ids.back();
        free_ids.pop_back();
        array[component_id] = a;
        entity_ids[component_id] = entity->entity_id;
      }
      else
      {
        component_id = array.size();
        array.push_back(a);
        entity_ids.push_back(entity->entity_id);
      }
      entity->remove_functions.push_back(RemoveFunctionsEntry(remove, this));
    }
    ComponentData get()
    {
      return array[component_id];
    }
    bool has()
    {
      return component_id!=NULL_ID;
    }
    static void remove(void* component)
    {
      ((Component<ComponentData>*)component)->remove();
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

private:
  template<typename ComponentData>
  friend class Component;
  static std::vector<Entity*> entity_array;
  static std::vector<ID> free_entity_ids;

  class RemoveFunctionsEntry
  {
    void (*function_ptr)(void*);
    void* instance_ptr;
  public:
    RemoveFunctionsEntry(void (*function_ptr)(void*), void* instance_ptr)
    {
      this->function_ptr = function_ptr;
      this->instance_ptr = instance_ptr;
    }
    void run()
    {
      (*function_ptr)(instance_ptr);
    }
  };
  std::vector<RemoveFunctionsEntry> remove_functions = std::vector<RemoveFunctionsEntry>();
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
    for(int i = 0; i<remove_functions.size(); i++)
    {
      remove_functions[i].run();
    }
    entity_array[entity_id] = nullptr;
    free_entity_ids.push_back(entity_id);
  }
  static Entity* get_entity(ID entity_id)
  {
    return entity_array[entity_id];
  }
  void remove()
  {
    this->~Entity();
  }
};
std::vector<Entity*> Entity::entity_array = std::vector<Entity*>();
std::vector<ID> Entity::free_entity_ids = std::vector<ID>();

template<typename ComponentData>
std::vector<ComponentData> Entity::Component<ComponentData>::array = std::vector<ComponentData>();
template<typename ComponentData>
std::vector<ID> Entity::Component<ComponentData>::entity_ids = std::vector<ID>();
template<typename ComponentData>
std::vector<ID> Entity::Component<ComponentData>::free_ids = std::vector<ID>();

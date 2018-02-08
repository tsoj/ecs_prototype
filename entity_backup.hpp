#pragma once
#include <vector>
typedef int ID;
const ID NULL_ID = -1;

#define COMPONENT(name)\
public:\
  struct name;\
private:\
  static std::vector<name> name ## _array;\
  static std::vector<ID> name ## _entity_id;\
  static std::vector<ID> name ## _free_ids;\
  ID name ## _id = NULL_ID;\
public:\
  void set_ ## name(name a)\
  {\
    if(name ## _free_ids.size()>0)\
    {\
      name ## _id = name ## _free_ids.back();\
      name ## _free_ids.pop_back();\
      name ## _array[name ## _id] = a;\
    }\
    else\
    {\
      name ## _id = name ## _array.size();\
      name ## _array.push_back(a);\
    }\
    name ## _entity_id[name ## _id] = entity_id;\
  }\
  name get_ ## name()\
  {\
    return name ## _array[name ## _id];\
  }\
  void delete_ ## name()\
  {\
    name ## _free_ids.push_back(name ## _id);\
  }\
  struct name


class Entity
{
private:
  static std::vector<Entity*> entity_list;
  static std::vector<ID> free_entity_ids;
  ID entity_id = NULL_ID;

  COMPONENT(Position)
  {
    float x;
    float y;
  };
public:
  Entity()
  {
    if(Entity::free_entity_ids.size()>0)
    {
      entity_id = free_entity_ids.back();
      free_entity_ids.pop_back();
      entity_list[entity_id] = this;
    }
    else
    {
      entity_id = entity_list.size();
      entity_list.push_back(this);
    }
  }

};

std::vector<Entity*> Entity::entity_list = std::vector<Entity*>();
std::vector<ID> Entity::free_entity_ids = std::vector<ID>();

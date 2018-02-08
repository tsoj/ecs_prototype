#pragma once
#include <iostream>
#include <vector>

typedef int ID;
const ID NULL_ID = -1;

class Entity
{
private:
  static std::vector<Entity*> entity_array;
  static std::vector<ID> free_entity_ids;
  ID entity_id = NULL_ID;

public:
  struct Position
  {
    float x;
    float y;
  };
private:
  static std::vector<Position> position_array;
  static std::vector<ID> position_entity_id;
  static std::vector<ID> position_free_ids;
  ID position_id = NULL_ID;
public:
  void set_position(Position a)
  {
    if(position_free_ids.size()>0)
    {
      position_id = position_free_ids.back();
      position_free_ids.pop_back();
      position_array[position_id] = a;
      position_entity_id[position_id] = entity_id;
    }
    else
    {
      position_id = position_array.size();
      position_array.push_back(a);
      position_entity_id.push_back(entity_id);
    }
  }
  Position get_position()
  {
    return position_array[position_id];
  }
  void delete_position()
  {
    position_entity_id[position_id] = NULL_ID;
    position_free_ids.push_back(position_id);
  }


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

  }
  void delete_entity()
  {
    this->delete_position();
    entity_array[entity_id] = nullptr;
    free_entity_ids.push_back(entity_id);
    this->~Entity();
  }

};

std::vector<Entity*> Entity::entity_array = std::vector<Entity*>();
std::vector<ID> Entity::free_entity_ids = std::vector<ID>();

std::vector<Entity::Position> Entity::position_array = std::vector<Entity::Position>();
std::vector<ID> Entity::position_entity_id = std::vector<ID>();
std::vector<ID> Entity::position_free_ids = std::vector<ID>();

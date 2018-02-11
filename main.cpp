#include <iostream>
#include "entity.hpp"

void gravity_system()
{
  Iterator<Entity::Mass> iter = Iterator<Entity::Mass>();
  while(iter.has_next())
  {
    EntityPointer current_entity = iter.next();

    if(!current_entity->c_Position.has())
    {
      continue;
    }
    ID mass_id = current_entity->c_Mass.get_id();
    Iterator<Entity::Mass> iter_2 = Iterator<Entity::Mass>(mass_id+1, END);
    while(iter_2.has_next())
    {
      EntityPointer current_entity_2 = iter_2.next();

      if(!current_entity_2->c_Position.has())
      {
        continue;
      }
      double dx = current_entity_2->c_Position->x - current_entity->c_Position->x;
      double dy = current_entity_2->c_Position->x - current_entity->c_Position->y;
      double distance_squared = dx*dx + dy*dy;
      if(distance_squared<0)
      {
        distance_squared*=-1;
      }
      double F = 6.67259e-11 * ((current_entity_2->c_Mass->m * current_entity->c_Mass->m) / (distance_squared));
      std::cout<<"Force between " << current_entity->get_id() <<" and "<< current_entity_2->get_id() << ": "<< F<<std::endl;
    }
  }
}
void output_system()
{
  Iterator<Entity> iter = Iterator<Entity>();
  while(iter.has_next())
  {
    EntityPointer current_entity = iter.next();

    std::cout << "-------------------"<<std::endl;
    std::cout << "Entity ID: " << current_entity->get_id() << std::endl;
    if(current_entity->c_Position.has())
    {
      std::cout << "Position:" << std::endl;
      std::cout << "\tx: " << current_entity->c_Position->x <<std::endl;
      std::cout << "\ty: " << current_entity->c_Position->y << std::endl;
    }
    if(current_entity->c_Mass.has())
    {
      std::cout << "Mass: " << current_entity->c_Mass->m << std::endl;
    }
    std::cout << "-------------------"<<std::endl;
  }
}

int main()
{
  std::cout << "Hello" << std::endl;

  EntityPointer o = Entity::create();
  o->init_Position(Entity::Position{33.3, 12.03});
  o->init_Mass(Entity::Mass{5.5});
  EntityPointer p = Entity::create();
  p->init_Position(Entity::Position{1000.0, 2000.0});
  p->init_Mass(Entity::Mass{800000.0});
  EntityPointer q = Entity::create();
  q->init_Position(Entity::Position{1.01, 540.045});
  q->init_Mass(Entity::Mass{0.3});

  /*Iterator<Entity::Position> iter = Iterator<Entity::Position>();
  while(iter.has_next())
  {
    EntityPointer current_entity = iter.next();
    std::cout << current_entity->c_Position->x << std::endl;
    current_entity->remove();
  }*/
  
  for(;;)
  {
    output_system();
    gravity_system();
  }
  std::cout << "Good Bye" << std::endl;
  return 0;
}

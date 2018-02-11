#include <iostream>
#include "entity.hpp"

/*void gravity_system()
{
  Iterator<Entity::Mass> iter = Iterator<Entity::Mass>();
  while(iter.has_next())
  {
    ID current_entity = iter.next();

    if(!Entity::get(current_entity).c_Position.has())
    {
      continue;
    }
    ID mass_id = Entity::get(current_entity).c_Mass.get_id();
    Iterator<Entity::Mass> iter_2 = Iterator<Entity::Mass>(mass_id+1, END);
    while(true)
    {
      ID current_entity_2 = iter_2.next();

      if(!Entity::get(current_entity_2).c_Position.has())
      {
        continue;
      }d D
      double dx = current_entity_2->Position_component.get_reference().x - current_entity->Position_component.get_reference().x;
      double dy = current_entity_2->Position_component.get_reference().y - current_entity->Position_component.get_reference().y;
      double distance_squared = dx*dx + dy*dy;
      if(distance_squared<0)
      {
        distance_squared*=-1;
      }
      double F = 6.67259e-11 * ((current_entity_2->Mass_component.get_reference().m * current_entity->Mass_component.get_reference().m) / (distance_squared));
      std::cout<<"Force between " << current_entity->get_id() <<" and "<< current_entity_2->get_id() << ": "<< F<<std::endl;
    }
  }
}
void output_system()
{
  EntityIterator iter = EntityIterator();
  while(true)
  {
    Entity* current_entity = iter.next_entity_pointer();
    if(current_entity == nullptr)
    {
      break;
    }
    std::cout << "-------------------"<<std::endl;
    std::cout << "Entity ID: " << current_entity->get_id() << std::endl;
    if(current_entity->Position_component.has())
    {
      std::cout << "Position:" << std::endl;
      std::cout << "\tx: " << current_entity->Position_component.get_reference().x <<std::endl;
      std::cout << "\ty: " << current_entity->Position_component.get_reference().y << std::endl;
    }
    if(current_entity->Mass_component.has())
    {
      std::cout << "Mass: " << current_entity->Mass_component.get_reference().m << std::endl;
    }
    std::cout << "-------------------"<<std::endl;
  }
}
*/
int main()
{
  std::cout << "Hello" << std::endl;

  EntityPointer o = Entity::create();
  o->init_Position(Entity::Position{33.3, 12.03});
  EntityPointer p = Entity::create();
  p->init_Position(Entity::Position{1.01, 540.045});
  p->init_Mass(Entity::Mass{0.3});

  Iterator<Entity::Position> iter = Iterator<Entity::Position>();
  while(iter.has_next())
  {
    EntityPointer current_entity = iter.next();
    std::cout << current_entity->c_Position.get().x << std::endl;
    current_entity->remove();
  }
  std::cout << "Good Bye" << std::endl;
  return 0;
}

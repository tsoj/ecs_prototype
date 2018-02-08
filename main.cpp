#include <iostream>
#include "entity.hpp"

/*void gravity_system()
{
  Entity::Iterator<Entity::Mass> iter = Entity::Iterator<Entity::Mass>();
  while(iter.has_next())
  {
    Entity & current_entity = iter.next_entity_reference()();
    //std::cout<<"!! ID: "<<current_entity->get_id()<<std::endl;
    if(current_entity == nullptr)
    {
      break;
    }
    if(!current_entity->Position_component.has())
    {
      continue;
    }
    ID mass_id = current_entity->Mass_component.get_id();
    Entity::Iterator<Entity::Mass> iter_2 = Entity::Iterator<Entity::Mass>(mass_id+1);
    while(true)
    {
      Entity* current_entity_2 = iter_2.next_entity_pointer();
      if(current_entity_2 == nullptr)
      {
        break;
      }
      if(!current_entity_2->Position_component.has())
      {
        continue;
      }
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
}*/
/*void output_system()
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
  Entity::res();
  Entity & o = Entity::create();
  o.Position_component.set(Entity::Position{0.5, 700.5});
  Entity & p = Entity::create();
  p.Position_component.set(Entity::Position{0.95, 0.01});
  Entity::Iterator<Entity::Position> iter = Entity::Iterator<Entity::Position>();
  while(iter.has_next())
  {
    Entity * current_entity = iter.next_entity_reference();
    std::cout << current_entity->Position_component.get().x <<std::endl;
  }
  /*Entity::get(id).Position_component.set(Entity::Position{0.3333, 111.11});
  Entity::get(id).Mass_component.set(Entity::Mass{1.0});
  id = Entity::create();
  Entity::get(id).Position_component.set(Entity::Position{0.12, 9.2});
  Entity::get(id).Mass_component.set(Entity::Mass{2.45});
  id = Entity::create();
  Entity::get(id).Position_component.set(Entity::Position{50.05, 3.2});
  Entity::get(id).Mass_component.set(Entity::Mass{71.3});*/
  /*for(int i = 0; i< 2; i++)
  {
    //gravity_system();
    //output_system();
    if(i==0)
    {
      e.remove();
      Entity h = Entity();
      h.Mass_component.set(Entity::Mass{4.4});
      Entity a = Entity();
      a.Mass_component.set(Entity::Mass{4.5});
      a.~Entity();
    }
  }*/

  std::cout << "Good Bye" << std::endl;
  return 0;
}

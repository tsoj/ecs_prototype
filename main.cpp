#include <iostream>
#include "entity.hpp"

int main()
{
  std::cout << "Hello" << std::endl;

  Index o = Entity::create();
  Entity::get(o).init_Position(Entity::Position{33.3, 12.03});
  Index p = Entity::create();
  Entity::get(p).init_Position(Entity::Position{1.01, 540.045});
  Entity::get(p).init_Mass(Entity::Mass{0.3});

  Entity::Iterator<Entity::Position> iter = Entity::Iterator<Entity::Position>();
  while(iter.has_next())
  {
    std::cout << iter.next_entity_reference()->c_Position.get().x << std::endl;
    Entity::get(p).remove();
  }
  std::cout << "Good Bye" << std::endl;
  return 0;
}

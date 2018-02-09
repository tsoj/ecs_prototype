#include <iostream>
#include "entity.hpp"

int main()
{
  std::cout << "Hello" << std::endl;

  Entity & o = Entity::create();
  o.init_Position(Entity::Position{0.2, 0.3});
  Entity & p = Entity::create();
  p.init_Position(Entity::Position{1.01, 540.045});

  Entity::Iterator<Entity::Position> iter = Entity::Iterator<Entity::Position>();
  while(iter.has_next())
  {
    std::cout << iter.next_entity_reference()->cPosition.get().x << std::endl;
  }
  std::cout << "Good Bye" << std::endl;
  return 0;
}

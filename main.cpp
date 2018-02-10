#include <iostream>
#include "entity.hpp"

int main()
{
  std::cout << "Hello" << std::endl;

  ID o = Entity::create();
  Entity::get(o).init_Position(Entity::Position{33.3, 12.03});
  ID p = Entity::create();
  Entity::get(p).init_Position(Entity::Position{1.01, 540.045});
  Entity::get(p).init_Mass(Entity::Mass{0.3});

  Iterator<Entity::Position> iter = Iterator<Entity::Position>();
  while(iter.has_next())
  {
    ID id = iter.next();
    std::cout << Entity::get(id).c_Position.get().x << std::endl;
    Entity::get(id).remove();
  }
  std::cout << "Good Bye" << std::endl;
  return 0;
}

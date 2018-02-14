#include <iostream>
#include "entity.hpp"

COMPONENT(Position)
{
  Position(double x, double y) : x(x), y(y) {}

  double x;
  double y;
};

COMPONENT(Mass)
{
  Mass(double m) : m(m) {}

  double m;
};

int main()
{
  std::cout << "Hello\n" << std::endl;
  EntityPtr a = Entity::create();
  EntityPtr b = Entity::create();
  EntityPtr c = Entity::create();
  EntityPtr z = Entity::create();

  Position::create(a, Position(0.2, 0.3));
  Position::create(b, Position(500.0, 600.0));
  Position::create(c, Position(42.0, 7890.3));

  Iterator<Position> iter = Iterator<Position>();
  while(iter.hasNext())
  {
    EntityPtr current = iter.next();
    std::cout << Position::get(current).x << std::endl;
  }

  a->remove();
  a = Entity::create();
  Position::create(z, Position(2000.0, 3000.0));
  Position::create(a, Position(1000.0, 1000.0));

  std::cout << std::endl;

  iter = Iterator<Position>();
  while(iter.hasNext())
  {
    EntityPtr current = iter.next();
    std::cout << Position::get(current).x << std::endl;
  }

  std::cout << "\nGood Bye" << std::endl;
  return 0;
}

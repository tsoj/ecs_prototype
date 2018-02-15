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

void outputSystem()
{
  Iterator<Entity> iter = Iterator<Entity>();
  while(iter.hasNext())
  {
    EntityPtr current = iter.next();
    std::cout << "-----------------------" << std::endl;
    std::cout << "ID: \t" << current.getID() << std::endl;
    if(Position::has(current))
    {
      std::cout << "Position:" << std::endl;
      std::cout << " x: \t" << Position::get(current).x << std::endl;
      std::cout << " y: \t" << Position::get(current).y << std::endl;
    }
    if(Mass::has(current))
    {
      std::cout << "Mass: \t" << Mass::get(current).m << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
  }

}

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

  outputSystem();

  std::cout << std::endl;
  a->remove();
  Position::create(z, Position(2000.0, 3000.0));
  a = Entity::create();
  Position::create(a, Position(1000.0, 1000.0));
  Mass::create(z, Mass(1000.0));
  Position::remove(c);

  outputSystem();

  std::cout << "\nGood Bye" << std::endl;
  return 0;
}

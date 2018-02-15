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

void gravitySystem()
{
  Iterator<Mass, Position> iter = Iterator<Mass, Position>();
  while(iter.hasNext())
  {
    EntityPtr entity = iter.next();
    Iterator<Mass, Position> iter2 = Iterator<Mass, Position>(iter.getCurrentID() + 1, END);
    while(iter2.hasNext())
    {
      EntityPtr entity2 = iter2.next();

      double dx = Position::get(entity2).x - Position::get(entity).x;
      double dy = Position::get(entity2).y - Position::get(entity).y;
      double distanceSquared = dx*dx + dy*dy;
      if(distanceSquared<0)
      {
        distanceSquared*=-1;
      }
      double F = 6.67259e-11 * ((Mass::get(entity2).m * Mass::get(entity).m) / (distanceSquared));
      std::cout<<"Force between " << entity.getID() <<" and "<< entity2.getID() << ": "<< F<<"\n";
    }
  }
}

int main()
{
  std::cout << "Hello\n" << std::endl;
  EntityPtr a = Entity::create();
  EntityPtr b = Entity::create();
  EntityPtr c = Entity::create();
  EntityPtr d = Entity::create();
  EntityPtr e = Entity::create();

  Position::create(a, Position(0.2, 0.3));
  Position::create(b, Position(500.0, 600.0));
  Position::create(c, Position(42.0, 7890.3));
  Position::create(d, Position(6.0, 6.3));
  Mass::create(a, Mass(0.7));
  Mass::create(b, Mass(7.3));
  Mass::create(c, Mass(1200000.0));

  outputSystem();
  gravitySystem();

  std::cout << std::endl;

  d->remove();
  a->remove();
  Position::remove(c);

  Position::create(e, Position(2000.0, 3000.0));
  Mass::create(e, Mass(1000.0));
  a = Entity::create();
  Position::create(a, Position(1000.0, 1000.0));

  outputSystem();
  gravitySystem();

  std::cout << "\nGood Bye" << std::endl;
  return 0;
}

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
  Iterator<void> iter = Iterator<void>();
  while(iter.hasNext())
  {
    ID currentEntityID = iter.next();
    std::cout << "-----------------------" << std::endl;
    std::cout << "ID: \t" << currentEntityID << std::endl;
    if(hasComponent<Position>(currentEntityID))
    {
      std::cout << "Position:" << std::endl;
      std::cout << " x: \t" << getComponent<Position>(currentEntityID).x << std::endl;
      std::cout << " y: \t" << getComponent<Position>(currentEntityID).y << std::endl;
    }
    if(hasComponent<Mass>(currentEntityID))
    {
      std::cout << "Mass: \t" << getComponent<Mass>(currentEntityID).m << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
  }
}

void gravitySystem()
{
  Iterator<Mass, Position> iter = Iterator<Mass, Position>();
  while(iter.hasNext())
  {
    ID currentEntityID = iter.next();
    Iterator<Mass, Position> iter2 = Iterator<Mass, Position>(iter.getCurrentID() + 1, END);
    while(iter2.hasNext())
    {
      ID currentEntityID2 = iter2.next();

      double dx = getComponent<Position>(currentEntityID2).x - getComponent<Position>(currentEntityID).x;
      double dy = getComponent<Position>(currentEntityID2).y - getComponent<Position>(currentEntityID).y;
      double distanceSquared = dx*dx + dy*dy;
      if(distanceSquared<0)
      {
        distanceSquared*=-1;
      }
      double F = 6.67259e-11 * ((getComponent<Mass>(currentEntityID2).m * getComponent<Mass>(currentEntityID).m) / (distanceSquared));
      std::cout<<"Force between " << currentEntityID <<" and "<< currentEntityID2 << ": "<< F<<"\n";
    }
  }
}

int main()
{
  std::cout << "Hello\n" << std::endl;
  ID a = createEntity();
  ID b = createEntity();
  ID c = createEntity();
  ID d = createEntity();
  ID e = createEntity();

  createComponent<Position>(a, Position(0.2, 0.3));
  createComponent<Position>(b, Position(500.0, 600.0));
  createComponent<Position>(c, Position(42.0, 7890.3));
  createComponent<Position>(d, Position(6.0, 6.3));
  createComponent<Mass>(a, Mass(0.7));
  createComponent<Mass>(b, Mass(7.3));
  createComponent<Mass>(c, Mass(1200000.0));

  outputSystem();
  gravitySystem();

  std::cout << std::endl;

  removeEntity(d);
  removeEntity(a);
  removeComponent<Position>(c);

  createComponent<Position>(e, Position(2000.0, 3000.0));
  createComponent<Mass>(e, Mass(1000.0));
  a = createEntity();
  createComponent<Position>(a, Position(1000.0, 1000.0));

  outputSystem();
  gravitySystem();

  std::cout << "\nGood Bye" << std::endl;
  return 0;
}

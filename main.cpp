#include <iostream>
#include "ecs.hpp"

struct Position
{
  Position(double x, double y) : x(x), y(y) {}

  double x;
  double y;
};
struct Mass
{
  Mass(double m) : m(m) {}

  double m;
};

struct SomeEvent
{
  int value = 0;
};

void handleSomeEvent(SomeEvent event)
{
  std::cout << "HANDLING EVENT !!: " << event.value << std::endl;
}

void outputSystem()
{
  EC::Iterator<void> iter = EC::Iterator<void>();
  while(iter.hasNext())
  {
    ID currentEntityID = iter.next();
    std::cout << "-----------------------" << std::endl;
    std::cout << "ID: \t" << currentEntityID << std::endl;
    if(EC::hasComponents<Position>(currentEntityID))
    {
      std::cout << "Position:" << std::endl;
      std::cout << " x: \t" << EC::getComponent<Position>(currentEntityID).x << std::endl;
      std::cout << " y: \t" << EC::getComponent<Position>(currentEntityID).y << std::endl;
    }
    if(EC::hasComponents<Mass>(currentEntityID))
    {
      std::cout << "Mass: \t" << EC::getComponent<Mass>(currentEntityID).m << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
  }
}
void gravitySystem()
{
  EC::Iterator<Mass, Position> iter = EC::Iterator<Mass, Position>();
  while(iter.hasNext())
  {
    ID currentEntityID = iter.next();
    EC::Iterator<Mass, Position> iter2 = EC::Iterator<Mass, Position>(iter.getCurrentID() + 1, END);
    while(iter2.hasNext())
    {
      ID currentEntityID2 = iter2.next();

      double dx = EC::getComponent<Position>(currentEntityID2).x - EC::getComponent<Position>(currentEntityID).x;
      double dy = EC::getComponent<Position>(currentEntityID2).y - EC::getComponent<Position>(currentEntityID).y;
      double distanceSquared = dx*dx + dy*dy;
      if(distanceSquared<0)
      {
        distanceSquared*=-1;
      }
      double F = 6.67259e-11 * ((EC::getComponent<Mass>(currentEntityID2).m * EC::getComponent<Mass>(currentEntityID).m) / (distanceSquared));
      std::cout<<"Force between " << currentEntityID <<" and "<< currentEntityID2 << ": "<< F<<"\n";
    }
  }
}


int main()
{
  SystemManager::addSystem(&outputSystem, std::chrono::milliseconds(0));
  SystemManager::addSystem(&gravitySystem, std::chrono::milliseconds(0));
  SystemManager::addSystem(&handleSomeEvent);

  std::cout << "Hello\n" << std::endl;
  ID a = EC::createEntity();
  ID b = EC::createEntity();
  ID c = EC::createEntity();
  ID d = EC::createEntity();
  ID e = EC::createEntity();

  EC::createComponent<Position>(a, Position(0.2, 0.3));
  EC::createComponent<Position>(b, Position(500.0, 600.0));
  EC::createComponent<Position>(c, Position(42.0, 7890.3));
  EC::createComponent<Position>(d, Position(6.0, 6.3));
  EC::createComponent<Mass>(a, Mass(0.7));
  EC::createComponent<Mass>(b, Mass(7.3));
  EC::createComponent<Mass>(c, Mass(1200000.0));

  SystemManager::throwEvent(SomeEvent{12504});
  SystemManager::throwEvent(SomeEvent{12505});
  SystemManager::throwEvent(SomeEvent{12506});
  SystemManager::throwEvent(SomeEvent{12507});
  SystemManager::throwEvent(SomeEvent{12508});
  SystemManager::throwEvent(SomeEvent{12509});

  SystemManager::runSystems();

  std::cout << std::endl;

  EC::removeEntity(d);
  EC::removeEntity(a);
  EC::removeComponent<Position>(c);

  EC::createComponent<Position>(e, Position(2000.0, 3000.0));
  EC::createComponent<Mass>(e, Mass(1000.0));
  a = EC::createEntity();
  EC::createComponent<Position>(a, Position(1000.0, 1000.0));

  SystemManager::runSystems();

  std::cout << "\nGood Bye" << std::endl;
  return 0;
}

### This is a C++14 single header entity-component-system library
#### How to use it
    using namespace ecs;
###### Create an entity:
    Entity a = Entity::createEntity();

###### Destroy an entity:
      a.removeEntity();
This will also automatically destroy all components that were assigned to this entity.
    
###### Assign a component to an entity:
    struct Position
    {
      double x;
      double y;
    };
    a.createComponent<Position>(Position{ 0.2, 0.3 });
You can replace the `Position` struct with any data structure you wish to use.

###### Remove a component from an entity:
    a.removeComponent<Position>();

###### Access a component that is assigned to an entity:
    a.getComponent<Position>().x = 1337.42;
This function returns a reference, so you do not need a `setComponent<T>(...` function. However, this reference is not stable when you assign new components to entities, so it might get invalid over time; use always this function when you want to access a component, unless you are sure, that the reference stays valid as long as you need it.

###### Check if a entity has a component:
    bool a_has_Position = a.hasComponents<Position>();
    bool a_has_Position_and_Mass_and_Velocity = a.hasComponents<Position, Mass, Velocity>();

###### Looping over entities:
    for(auto a : Iterator<Mass, Position>()) // loops over all entities, that have the components Mass and Position
    {
      for(
        auto b_iter = ++Iterator<Mass, Position>(a); // this sets b_iter to an Iterator, that is one entity further than a is
        b_iter != Iterator<Mass, Position>().end();
        ++b_iter
        )
      {
        Entity b = *b_iter;
        // do gravity calculations or whatever
      }
    }
    
#### Limitations
This library does not work with multiple .cpp files (multiple object files, that get linked together). You still will be able to multiple .cpp files but you need to stick to a single .cpp file (including any number of header files) with library calls.


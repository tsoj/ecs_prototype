# This is a C++17 single header entity-component-system library
## How to use it
First of all, everything is in the `ecs` namespace.

##### Create an entity:
    Entity a = Entity::createEntity();

##### Destroy an entity:
    a.removeEntity();

This will also automatically destroy all components that have been assigned to this entity.

##### Assign a component to an entity:
    struct Position
    {
      double x;
      double y;
    };
    a.createComponent<Position>(Position{ 0.2, 0.3 });

You can replace the `Position` struct with any data structure that you wish to use.

##### Remove a component from an entity:
    a.removeComponent<Position>();

##### Access a component that is assigned to an entity:
    a.getComponent<Position>().x = 1337.42; // is undefined when a has no Position component assigned

This function returns a reference so you do not need a `setComponent<T>(... `function, but this reference is not stable when you assign or remove components so it might get invalid over time. If you want to store the return value of this function in a variable be sure that it stays valid as long as you need it.

##### Check if an entity has a component:
    bool a_has_Position = a.hasComponents<Position>();
    bool a_has_Position_and_Mass_and_Velocity = a.hasComponents<Position, Mass, Velocity>();
    bool a_is_a_valid_entity = a.hasComponents<void>();

##### Looping over entities:
    for(auto a : Iterator<Mass, Position>()) // loops over all entities that have the components Mass and Position
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

    for(auto a : Iterator<void>()) // loops over all valid entities
    { /*...*/ }

##### Run all added systems and resolve all thrown events:
    SystemManager::runSystems();

##### Add a system:
    void gravitySystem()
    {
        // loop over entities or whatever
    }
    SystemManager::addSystem(&gravitySystem, std::chrono::milliseconds(10));

This system will only be called by `SystemManager::runSystems()` if the last call of this system was 10 milliseconds or more ago.

##### Throw an event:
    struct SomeEvent
    {
        double value;
    }
    SystemManager::throwEvent(SomeEvent{0.001});

Again, you can replace `SomeEvent` with any structure you want to use.

##### Catch an event:
    void catchEvent(const SomeEvent& event)
    {
        std::cout << "Catched event: " << event.value << std::endl;
    }
    SystemManager::addSystem(&catchEvent);

You can replace `catchEvent()` with any function you want. If you want to catch all events `T` you need to add a function that looks like this:

    void customEventCatcher(const T& event)
    {
        // ...
    }

##### Remove a system:
    SystemManager::removeSystem(&catchEvent);
    SystemManager::removeSystem(&gravitySystem);

If you have added the same system several times, all of them will be removed, even if you have assigned different durations to them.

## Features

- Components are stored in a contiguous memory with no gaps between them. Therefore, this library benefits very well from CPU caching.

- Since this library is build upon templates, it is type-safe.

## Limitations

- Assigning or removing components may require a large memory operation and is therefore not too fast.

- Addresses of components are unstable when components are assigned or removed.

- You cannot create multiple *worlds*, all entities are stored in the same state.

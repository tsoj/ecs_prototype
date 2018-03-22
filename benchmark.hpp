#include "ecs.hpp"
#include "entityplus/entity.h"
#include <chrono>
#include <iostream>

class Timer {
	std::chrono::high_resolution_clock::time_point start;
	const char *str;
public:
	Timer(const char *str): start(std::chrono::high_resolution_clock::now()),
		str(str) {}
	~Timer() {
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << str << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
	}
};

void ecsTest(int entityCount, int iterationCount, int tagProb)
{
  std::cout << "------------------------------------Starting: " << " entityCount: " << entityCount << " iterationCount: " << iterationCount << " tagProb: " << tagProb << std::endl;
	using namespace ecs;
	struct Tag {};
	std::cout << "ecs\n";
	{
		Timer timer("Add entities: ");
		for (int i = 0; i < entityCount; ++i)
    {
			auto ent = Entity::createEntity();
			ent.createComponent<int>(i);
			if (i % tagProb == 0)
				ent.createComponent<Tag>();
		}
	}
	{
		Timer timer("For_each entities: ");
		std::uint64_t sum = 0;
		for (int i = 0; i < iterationCount; ++i)
    {
      for(auto ex : Iterator<Tag, int>())
      {
				sum += i;
			}
		}
		std::cout << sum << "\n";
	}
  std::cout << "------------------------------------Done" << std::endl;
}

void entXTest(int entityCount, int iterationCount, int tagProb) {
	using namespace entityplus;
	entity_manager<component_list<int>, tag_list<struct Tag>> em;
	em.create_grouping<int, Tag>();
	std::cout << "EntityPlus\n";
	{
		Timer timer("Add entities: ");
		for (int i = 0; i < entityCount; ++i) {
			auto ent = em.create_entity();
			ent.add_component<int>(i);
			if (i % tagProb == 0)
				ent.set_tag<Tag>(true);
		}
	}
	{
		Timer timer("For_each entities: ");
		std::uint64_t sum = 0;
		for (int i = 0; i < iterationCount; ++i) {
			em.for_each<Tag, int>([&](auto ent, auto i) {
				sum += i;
			});
		}
		std::cout << sum << "\n";
	}
}

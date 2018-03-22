#include <chrono>
#include <iostream>
#include "ecs.hpp"

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
    	#pragma GCC diagnostic push
    	#pragma GCC diagnostic ignored "-Wunused-variable"
      for(auto ex : Iterator<Tag, int>())
      {
				sum += i;
			}
			#pragma GCC diagnostic pop
		}
		std::cout << sum << "\n";
	}
  std::cout << "------------------------------------Done" << std::endl;
}

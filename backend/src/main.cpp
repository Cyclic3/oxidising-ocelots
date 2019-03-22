#include "api.hpp"

#include <iostream>

using namespace oxidisingocelots;

int main() {
  state_machine_api s;

  s.debug = true;

  s.handle(R"({"action":"init", "players":[1,2,3]})");
  s.handle(R"({"action":"finish"})");

  try {
    std::string line;
    while(std::getline(std::cin, line))
      std::cout << s.handle(line) << std::endl;
  }
  catch (oxidation o) {
    throw;
  }

  return 0;
}

#include "api.hpp"

#include <iostream>

using namespace oxidisingocelots;

int main() {
  state_machine_api s;

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

#include "api.hpp"

#include <iostream>

using namespace oxidisingocelots;

std::string do_one(state_machine_api& s, std::string_view line) {
  auto json = c3::nu::json_decode(line);
  return c3::nu::json_encode(s.handle(json));
}

int main() {
  state_machine_api s;

  do_one(s, R"({"action":"init", "players":[1,2,3]})");

  try {
    std::string line;
    while(std::getline(std::cin, line))
      std::cout << do_one(s, line) << std::endl;
  }
  catch (oxidation o) {
    throw;
  }

  return 0;
}

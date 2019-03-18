#include "state_machine.hpp"

using namespace oxidisingocelots;

int main() {
  state s({
            { 69, "yeet" },
            { 420, "hax0r" }
          });

  try {
    while(true)
      s.draw();
  }
  catch (oxidation o) {
    throw;
  }

  return 0;
}

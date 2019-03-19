#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace oxidisingocelots {
  enum class card : unsigned int8_t {
    OxidisingOcelot = 0,
    Skip = 1,
    Attack = 2,
    Shuffle = 3,
    Defuse = 4,
    Reverse = 5,
    Ambiguous = 6,
    Normal = 7,
    Mark = 8
  };

  extern std::map<card, std::string> card_names;

  extern std::vector<card> initial_deck;

  constexpr int initial_n_dealt_cards = 4;
}


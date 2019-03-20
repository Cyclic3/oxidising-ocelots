#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace oxidisingocelots {
  enum class card : uint8_t {
    OxidisingOcelot = 0,
    Defuse,
    Skip,
    Attack,
    Shuffle,
    Reverse,
    Mark,
    ErraticElephant,
    Steal,
    Mug,
  };

  extern std::map<card, std::string> card_names;

  extern std::vector<card> initial_deck;

  constexpr int initial_n_dealt_cards = 4;
}


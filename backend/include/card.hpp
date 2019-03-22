#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include <c3/nu/data/encoders/base.hpp>

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
    Defer,
    Scry
  };

  extern std::map<card, std::string> card_names;

  extern std::map<card, std::map<std::string, std::string>> card_params;

  extern std::vector<card> initial_deck;

  constexpr int initial_n_dealt_cards = 4;
}


#include <string>

#include "card.hpp"

namespace oxidisingocelots {
  std::map<card, std::string> card_names {
    { card::OxidisingOcelot, "Oxidising Ocelot" },
    { card::Defuse, "Defuse" },
    { card::Skip, "Skip" },
    { card::Attack, "Attack" },
    { card::Shuffle, "Shuffle" },
    { card::Reverse, "Reverse" },
    { card::ErraticElephant, "Erratic Elephant" },
    { card::Steal, "Steal" },
    { card::Mug, "Mug" },
  };

  std::vector<card> initial_deck {
    // Four skips
    card::Skip, card::Skip, card::Skip, card::Skip,
    // Four attacks
    card::Attack, card::Attack, card::Attack, card::Attack,
    // Four attacks
    card::Reverse, card::Reverse, card::Reverse, card::Reverse,
    // Four shuffles
    card::Shuffle, card::Shuffle, card::Shuffle, card::Shuffle,
    // Four elephants
    card::ErraticElephant, card::ErraticElephant, card::ErraticElephant, card::ErraticElephant,
    // Four steal
    card::Steal, card::Steal, card::Steal, card::Steal,
    // Two mug
    //card::Mug, card::Mug
  };
}


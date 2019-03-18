#include "card.hpp"

namespace oxidisingocelots {
  std::map<card, std::string> card_names {
    { card::OxidisingOcelot, "Oxidising Ocelot" },
    { card::Skip, "Skip" },
    { card::Attack, "Attack" },
    { card::Shuffle, "Shuffle" },
    { card::Reverse, "Reverse" },
    { card::Defuse, "Defuse" },
    { card::Reverse, "Reverse" },
    { card::Ambiguous, "Ambiguous" },
    { card::Normal, "Normal" },
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
    // 16 normals
    card::Normal, card::Normal, card::Normal, card::Normal,
    card::Normal, card::Normal, card::Normal, card::Normal,
    card::Normal, card::Normal, card::Normal, card::Normal,
    card::Normal, card::Normal, card::Normal, card::Normal,
    // 4 ambiguous
    card::Ambiguous, card::Ambiguous, card::Ambiguous, card::Ambiguous
  };
}

#include "card.hpp"
#include "player.hpp"
#include "state_machine.hpp"

#include <random>

namespace oxidisingocelots {
  std::random_device rng;

  void state::shuffle() {
    std::shuffle(deck.begin(), deck.end(), rng);
  }

  card player::_take_random_card() {
    if (hand.size() == 0)
      throw std::runtime_error("Empty hand");

    std::uniform_int_distribution<ptrdiff_t> dist(0, hand.size() - 1);

    auto target = hand.begin();
    std::advance(target, dist(rng));

    return hand.extract(target).value();
  }
}

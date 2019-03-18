#include "state_machine.hpp"

namespace oxidisingocelots {
  void state::play(card&& c) {
    switch (c) {
      case (card::Skip): next_player(); break;
      case (card::Attack): next_player(); ++goes_left; break;
      case (card::Shuffle): shuffle(); break;
      case (card::Reverse): is_forward = !is_forward; break;

      default:
        throw std::runtime_error("Invalid card");
    }
  }
}

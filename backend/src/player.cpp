#include "player.hpp"

namespace oxidisingocelots {
  bool player::defuse() {
    if (auto iter = hand.find(card::Defuse); iter != hand.end()) {
      hand.erase(iter);
      return true;
    }
    else return false;
  }
}

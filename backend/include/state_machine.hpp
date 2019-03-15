#pragma once

#include <queue>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <algorithm>
#include <array>

#include "card.hpp"
#include "player.hpp"

namespace oxidisingocelots {
  class state {
  private:
    std::deque<card> deck;
    std::map<player_id, player> players;

  public:
    static std::random_device rng;

  public:
    void shuffle() {
      std::shuffle(deck.begin(), deck.end(), rng);
    }
    card take_top() {
      auto ret = deck.front();
      deck.pop_front();
      return ret;
    }
    card take_bottom() {
      auto ret = deck.back();
      deck.pop_back();
      return ret;
    }
    void push_front(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.begin() + dist, std::move(c));
    }
    void push_back(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.end() - dist, std::move(c));
    }

  public:
    state(std::vector<player> _players,
          int _n_extra_defuses = 2,
          int _n_extra_ocelots = 0,
          std::vector<card> _deck = initial_deck,
          int _n_dealt_cards = initial_n_dealt_cards) : deck{_deck.begin(), _deck.end()} {

      shuffle();

      // Then we act on the players
      for (auto& i : _players) {
        // Add the player
        if (auto player_res = players.emplace(i.id, i); player_res.second) {
          auto& player = player_res.first->second;
          // Deal the defuse
          player.pick_up(card::Defuse);
          // Deal n unforced cards
          for (auto i = 0; i < _n_dealt_cards; ++i)
            player.pick_up(take_top());
        }
      }

      // Then add in the ocelots and the defuses
      deck.insert(deck.end(), players.size() - 1 + _n_extra_ocelots, card::OxidisingOcelot);
      deck.insert(deck.end(), _n_extra_defuses, card::Defuse);
      // Shuffle the final cards
      shuffle();
    }
  };
}


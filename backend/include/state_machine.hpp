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
  class oxidisation {
  public:
    player oxidised;

  public:
    oxidisation(player oxidised) : oxidised(std::move(oxidised)) {}
  };

  class state {
  private:
    std::deque<card> deck;
    std::vector<player> players;
    bool is_forward = true;
    uint pos = 0;
    uint goes_left = 1;

  public:
    static std::random_device rng;

  private:
    card _take_top() {
      auto ret = deck.front();
      deck.pop_front();
      return ret;
    }
    card _take_bottom() {
      auto ret = deck.back();
      deck.pop_back();
      return ret;
    }
    void _push_front(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.begin() + dist, std::move(c));
    }
    void _push_back(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.end() - dist, std::move(c));
    }
    void next_player() {
      if (is_forward)
        ++pos %= players.size();
      else {
        if (pos == 0)
          pos = players.size() - 1;
        else
          --pos;
      }
      goes_left = 1;
    }
    void _oxidise() {
      // Steal the player
      auto player = std::move(players[pos]);
      players.erase(players.begin() + pos);
      // Now we fix the array
      if (is_forward)
        pos %= players.size();
      else
        pos != 0 ? --pos : pos = players.size() - 1;
      // Now kill them
      throw oxidisation(std::move(player));
    }
  public:
    void shuffle() {
      std::shuffle(deck.begin(), deck.end(), rng);
    }
    void play(card&& c);
    card draw(bool top = false) {
      auto ret = top ? _take_top() : _take_bottom();

      if (ret == card::OxidisingOcelot)
        _oxidise();
      else if (--goes_left == 0)
        next_player();

      return ret;
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
        auto& player = players.emplace_back(i);
        // Deal the defuse
        player.pick_up(card::Defuse);
        // Deal n unforced cards
        for (auto i = 0; i < _n_dealt_cards; ++i)
          player.pick_up(_take_top());
      }

      // Then add in the ocelots and the defuses
      deck.insert(deck.end(), players.size() - 1 + _n_extra_ocelots, card::OxidisingOcelot);
      deck.insert(deck.end(), _n_extra_defuses, card::Defuse);
      // Shuffle the final cards
      shuffle();
    }
  };
}


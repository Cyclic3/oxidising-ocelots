#pragma once

#include <queue>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <array>

#include "card.hpp"
#include "player.hpp"

namespace oxidisingocelots {
  class state {
  private:
    std::deque<card> deck;
  public:
    std::vector<player> players;
  private:
    bool is_forward = true;
    bool finish_draw_top = true;
    uint pos = 0;
    uint goes_left = 1;

  public:
    inline player& current_player() { return players[pos]; }
    inline decltype(goes_left) n_goes_left() { return goes_left; }
  private:
    inline card _take_top() {
      auto ret = deck.front();
      deck.pop_front();
      return ret;
    }
    inline card _take_bottom() {
      auto ret = deck.back();
      deck.pop_back();
      return ret;
    }
    inline void _push_front(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.begin() + dist, std::move(c));
    }
    inline void _push_back(card&& c, size_t dist) {
      if (dist > deck.size())
        throw std::runtime_error("Bad deck position");
      deck.emplace(deck.end() - dist, std::move(c));
    }
    inline void next_player() {
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
    void _oxidise(player_id id);

  public:
    void kill();
    void shuffle();
    void play(card&& c);
    void draw_top(player_id id) {
      auto ret = _take_top();

      if (ret == card::OxidisingOcelot)
        _oxidise(id);
      else
        get_player(id).pick_up(std::move(ret));
    }
    void draw_bottom(player_id id) {
      auto ret = _take_top();

      if (ret == card::OxidisingOcelot)
        _oxidise(id);
      else
        get_player(id).pick_up(std::move(ret));
    }
    void finish() {
      if (finish_draw_top)
        draw_top(current_player().id);
      else
        draw_bottom(current_player().id);

      if (--goes_left == 0)
        next_player();
    }
    player& get_player(player_id id) {
      auto iter = std::find_if(players.begin(), players.end(), [&](auto& p) { return p.id == id; });
      if (iter == players.end())
        throw std::runtime_error("Invalid player id");
      else
        return *iter;
    }

  public:
    state(std::vector<player> _players,
          int _n_extra_defuses = 2,
          int _n_extra_ocelots = 0,
          std::vector<card> _deck = initial_deck,
          int _n_dealt_cards = initial_n_dealt_cards);
  };
}


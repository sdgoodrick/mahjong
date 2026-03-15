#include "history.h"

std::optional<Match> History::rewind() {
   if (matches.empty() || cursor == 0)
     return std::nullopt;

   auto match = matches[--cursor];
   return match;
}

std::optional<Match> History::restore() {
  if (matches.empty() || cursor >= matches.size())
    return std::nullopt;

   auto match = matches[cursor++];
   return match;
}

Match::Match(Position a_pos, Tile* a_tile, Position b_pos, Tile* b_tile)
  : a(std::make_pair(a_pos, a_tile)),
    b(std::make_pair(b_pos, b_tile))
{}

#pragma once

enum class Kind {
  Pin,
  Sou,
  Man,
  Dragon,
  Season,
  Flower,
  Wind,
};

enum class DragonKind : unsigned short {
  Green = 0,
  White = 1,
  Red = 2,
};

enum class SeasonKind : unsigned short {
  Autumn = 0,
  Winter = 1,
  Spring = 2,
  Summer = 3,
};

enum class FlowerKind : unsigned short {
  Bamboo = 0,
  Orchid = 1,
  Mum = 2,
  Plum = 3,
};

enum class WindKind : unsigned short {
  North = 0,
  East = 1,
  South = 2,
  West = 3,
};

struct Tile {
  Tile(unsigned short value, Kind kind)
    :value(value), kind(kind) {}

  inline bool operator==(const Tile& rhs) const {
    if (kind == Kind::Flower && rhs.kind == Kind::Flower)
      return true;

    if (kind == Kind::Season && rhs.kind == Kind::Season)
      return true;

    return kind == rhs.kind && value == rhs.value;
  }

  auto operator<=>(const Tile& rhs) const = default;


  unsigned short value;
  Kind kind;
};

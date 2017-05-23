#include <iostream>
#include <limits>

enum class ReversiCell {
  kEmpty, kBlack, kWhite
};

struct Step {
  int row;
  int column;
  int minmax;
};

struct ReversiGame {
  ReversiGame() {
    for (auto &row : field)
      for (auto &cell : row)
        cell = ReversiCell::kEmpty;

    field[3][3] = field[4][4] = ReversiCell::kWhite;
    field[3][4] = field[4][3] = ReversiCell::kBlack;
  }

  ReversiCell field[8][8];
};

ReversiCell Other(ReversiCell color) {
  switch (color) {
    case ReversiCell::kEmpty:
      return ReversiCell::kEmpty;
    case ReversiCell::kBlack:
      return ReversiCell::kWhite;
    case ReversiCell::kWhite:
      return ReversiCell::kBlack;
  }
}

struct Flips {
  unsigned sum;
  unsigned e;
  unsigned ne;
  unsigned n;
  unsigned nw;
  unsigned w;
  unsigned sw;
  unsigned s;
  unsigned se;
};

bool InBounds(int i, int j) {
  return 0 <= i && i < 8 && 0 <= j && j < 8;
}

Flips CountFlips(ReversiGame &game, ReversiCell color, int row, int col) {
  Flips flips{0, 0, 0, 0, 0, 0, 0, 0, 0};

  for (int i = 1;
       InBounds(row, col + i)
       && game.field[row][col + i] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row][col + i] == color) {
      flips.e = i - 1;
      flips.sum += flips.e;
      break;
    }
  }
  for (int i = 1;
       InBounds(row - i, col + i)
       && game.field[row - i][col + i] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row - i][col + i] == color) {
      flips.ne = i - 1;
      flips.sum += flips.ne;
      break;
    }
  }
  for (int i = 1;
       InBounds(row - i, col)
       && game.field[row - i][col] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row - i][col] == color) {
      flips.n = i - 1;
      flips.sum += flips.n;
      break;
    }
  }
  for (int i = 1;
       InBounds(row - i, col - i)
       && game.field[row - i][col - i] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row - i][col - i] == color) {
      flips.nw = i - 1;
      flips.sum += flips.nw;
      break;
    }
  }
  for (int i = 1;
       InBounds(row, col - i)
       && game.field[row][col - i] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row][col - i] == color) {
      flips.w = i - 1;
      flips.sum += flips.w;
      break;
    }
  }
  for (int i = 1;
       InBounds(row + i, col - i)
       && game.field[row + i][col - i] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row + i][col - i] == color) {
      flips.sw = i - 1;
      flips.sum += flips.sw;
      break;
    }
  }
  for (int i = 1;
       InBounds(row + i, col)
       && game.field[row + i][col] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row + i][col] == color) {
      flips.s = i - 1;
      flips.sum += flips.s;
      break;
    }
  }
  for (int i = 1;
       InBounds(row + i, col + i)
       && game.field[row + i][col + i] != ReversiCell::kEmpty;
       ++i) {
    if (game.field[row + i][col + i] == color) {
      flips.se = i - 1;
      flips.sum += flips.se;
      break;
    }
  }

  return flips;
}

void SetColor(ReversiGame &game, ReversiCell color,
              int row, int col, Flips &flips) {
  for (int i = 1; i <= flips.e; ++i) {
    game.field[row][col + i] = color;
  }
  for (int i = 1; i <= flips.ne; ++i) {
    game.field[row - i][col + i] = color;
  }
  for (int i = 1; i <= flips.n; ++i) {
    game.field[row - i][col] = color;
  }
  for (int i = 1; i <= flips.nw; ++i) {
    game.field[row - i][col - i] = color;
  }
  for (int i = 1; i <= flips.w; ++i) {
    game.field[row][col - i] = color;
  }
  for (int i = 1; i <= flips.sw; ++i) {
    game.field[row + i][col - i] = color;
  }
  for (int i = 1; i <= flips.s; ++i) {
    game.field[row + i][col] = color;
  }
  for (int i = 1; i <= flips.se; ++i) {
    game.field[row + i][col + i] = color;
  }
}

const int position_bonus[8][8] = {
  {5, -2, 2, 2, 2, 2, -2, 5},
  {-2, -2, 0, 0, 0, 0, -2, -2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {2, 0, 0, 0, 0, 0, 0, 2},
  {-2, -2, 0, 0, 0, 0, -2, -2},
  {5, -2, 2, 2, 2, 2, -2, 5},
};

Step BestStep(ReversiGame &game, ReversiCell color, unsigned depth = 4) {
  Step best_step{-1, -1, std::numeric_limits<int>::min() + 1};
  bool has_step = false;
  if (!depth) {
    best_step.minmax = 0;
    return best_step;
  }

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (game.field[i][j] != ReversiCell::kEmpty) {
        continue;
      }

      auto flips = CountFlips(game, color, i, j);
      if (!flips.sum) {
        continue;
      }

      has_step = true;
      game.field[i][j] = color;
      SetColor(game, color, i, j, flips);
      int delta = 1 + flips.sum - BestStep(game, Other(color), depth - 1).minmax
                  + position_bonus[i][j];
      if (delta >= best_step.minmax) {
        best_step = {i, j, delta};
      }
      SetColor(game, Other(color), i, j, flips);
      game.field[i][j] = ReversiCell::kEmpty;
    }
  }

  if (!has_step) {
    best_step.minmax = -BestStep(game, Other(color), depth - 1).minmax;
  }

  return best_step;
}

int main() {
  std::string command;
  ReversiCell color;
  ReversiGame game;
 
  std::cin >> command >> command;
  if (command == "black") {
    color = ReversiCell::kBlack;
  } else {
    color = ReversiCell::kWhite;
  }

  std::cin >> command;
  while (command != "bad" && command != "win"
         && command != "lose" && command != "draw") {
    if (command == "move") {
      char row_char, column_char;
      std::cin >> column_char >> row_char;
      int column = column_char - 'a', row = row_char - '1';
      auto flips = CountFlips(game, Other(color), row, column);
      SetColor(game, Other(color), row, column, flips);
      game.field[row][column] = Other(color);
    } else { // turn
      auto step = BestStep(game, color);
      auto flips = CountFlips(game, color, step.row, step.column);
      SetColor(game, color, step.row, step.column, flips);
      game.field[step.row][step.column] = color;
      std::cout << "move " << (char) ('a' + step.column)
                << ' ' << (char) ('1' + step.row) << std::endl;
    }
    std::cin >> command;
  }
    
  return 0;
}

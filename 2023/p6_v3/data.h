#include <iostream>
#include <limits.h>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#ifndef DATA_H_
#define DATA_H_

struct Program;

struct Choice {
  int solver;
  Program *p;

  int cost() const;
};

struct Solution {
  Solution();
  Solution(const Solution &);
  Solution &operator=(const Solution &);

  std::unique_ptr<Solution> incomming;

  std::vector<Choice> path;
  int score = INT_MAX;
  int total_time = INT_MAX;

  bool Better(const Solution &other) const {
    if (path.size() > other.path.size()) {
      return true;
    }
    if (path.size() < other.path.size()) {
      return false;
    }

    // lengths are equal
    return score < other.score;
  }
};

struct Program {
  int cost[3]; // A, B, C
  int index;   // [0, 18)
  Solution solutions[3];
};

Solution MakeSolution(const Choice &c);

void PrintSolution(const Solution &solution);

#endif // DATA_H_
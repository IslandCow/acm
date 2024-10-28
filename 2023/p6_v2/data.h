#include <limits.h>
#include <map>
#include <vector>

#ifndef DATA_H_
#define DATA_H_

struct Solution;

struct Program {
  int cost[3]; // A, B, C
  int index;   // [0, 18)
  std::map<int, Solution> solutions[3];
};

struct Choice {
  int solver;
  Program *p;

  int cost() const { return p->cost[solver]; }
};

struct Solution {
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
    return score > other.score;
  }
};

Solution MakeSolution(const Choice &c) {
  Solution s;
  s.path.push_back(c);
  s.score = c.cost();
  s.total_time = s.score;
  return s;
}

#endif // DATA_H_
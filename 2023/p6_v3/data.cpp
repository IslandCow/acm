#include "data.h"

Solution::Solution() = default;
Solution::Solution(const Solution &other) {
  path = other.path;
  score = other.score;
  total_time = other.total_time;
}

Solution &Solution::operator=(const Solution &other) {
  path = other.path;
  score = other.score;
  total_time = other.total_time;
  return *this;
}

int Choice::cost() const { return p->cost[solver]; }

Solution MakeSolution(const Choice &c) {
  Solution s;
  s.path.push_back(c);
  s.score = c.cost();
  s.total_time = s.score;
  return s;
}

void PrintSolution(const Solution &solution) {
  std::cerr << "[";
  for (const auto &c : solution.path) {
    std::cerr << "(" << c.p->index << "," << c.solver << "), ";
  }
  std::cerr << "]" << std::endl;
}
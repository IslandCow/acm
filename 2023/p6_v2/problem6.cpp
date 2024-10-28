#include <algorithm>
#include <iostream>
#include <limits.h>
#include <utility>
#include <vector>

#include "data.h"
#include "solver.h"

int main() {
  int n;
  int s;
  int t;
  std::cin >> n >> s >> t;

  std::cerr << "n=" << n << " s=" << s << " t=" << t << std::endl;
  std::vector<Program> programs;
  for (int i = 0; i < n; i++) {
    Program p;
    std::cin >> p.cost[0] >> p.cost[1] >> p.cost[2];
    p.index = i;
    programs.push_back(p);
  }
  std::cerr << "Programs read: " << programs.size() << std::endl;

  Solver solver(programs, s, t);
  std::cerr << "Populating graph" << std::endl;
  solver.PopulateGraph();
  std::cerr << "Graph populated" << std::endl;
  Solution best = solver.BestSolution();
  std::cerr << "DONE" << std::endl;

  std::cout << best.path.size() << " " << best.score << std::endl;
  return 0;
}

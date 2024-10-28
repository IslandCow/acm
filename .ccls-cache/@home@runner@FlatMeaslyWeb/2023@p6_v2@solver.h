#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <set>

#include "data.h"

std::array<std::vector<Program *>, 3> Order(std::vector<Program> programs) {
  std::array<std::vector<Program *>, 3> result;
  for (int i = 0; i < 3; i++) {
    for (Program &p : programs) {
      result[i].push_back(&p);
    }
    std::vector<Program *> &p = result[i];
    std::sort(p.begin(), p.end(),
              [i](Program *a, Program *b) { return a->cost[i] < b->cost[i]; });
  }
  return result;
}

Program *Next(const std::vector<Program *> &ordered,
              const std::set<int> visited) {
  for (Program *p : ordered) {
    if (visited.find(p->index) == visited.end()) {
      return p;
    }
  }
  return nullptr;
}

class Solver {
public:
  Solver(std::vector<Program> programs, int delay, int total_time)
      : delay_(delay), total_time_(total_time) {
    programs_ = Order(programs);
  }

  void PopulateGraph() {
    for (int i = 0; i <= 2; i++) {
      std::set<int> visited;
      Program *p = *programs_[i].begin();
      visited.insert(p->index);
      // Add solution for the first problem
      Iterate(p, programs_, visited);
    }
  }

  Solution BestSolution() {
    Solution best;
    for (int i = 0; i <= 2; i++) {
      Program *p = *programs_[i].begin();
      // Find the longest solution that's still legal.
      for (int j = NumProblems() - 1; j >= best.path.size(); j--) {
        Solution &s = p->solutions[i][j];
        if (s.total_time <= total_time_) {
          if (s.Better(best)) {
            best = s;
          }
          break;
        }
      }
    }
    return best;
  }

  void Iterate(Program *cur, std::array<std::vector<Program *>, 3> ordered,
               std::set<int> visited) {
    if (ordered[0].size() == visited.size()) {
      // out of programs
      return;
    }
    for (int i = 0; i < 3; i++) {
      std::vector<Program *> a = ordered[i];
      Program *p = Next(a, visited);
      if (p == nullptr) {
        std::cerr << "Null program!" << std::endl;
        exit(1);
      }
      int cur_cost = p->cost[i];
      for (int j = 0; j <= 2; j++) {
        if (p->solutions[j].empty()) {
          if (p->cost[j] > cur_cost + delay_) {
            // Skip branches that cost more than the delay since they are never
            // optimal.
            continue;
          }

          // Compute the solutions
          std::set<int> v = visited;
          v.insert(p->index);
          Iterate(p, ordered, v);
        }
      }

      // aggregate best solution
      std::vector<Solution> best_solutions;
      for (int len = 1; len <= NumProblems(); len++) {
        std::optional<Solution> best;
        for (int k = 0; k <= 2; k++) {
          auto iter = p->solutions[k].find(len);
          if (iter != p->solutions[k].end()) {
            if (!best.has_value()) {
              best = iter->second;
              continue;
            }
            if (iter->second.Better(*best)) {
              best = iter->second;
            }
          }
        }
        if (best.has_value()) {
          best_solutions.push_back(*best);
        }
      }

      cur->solutions[i] = AddSolutions(i, cur, best_solutions);
    }
  }

  std::map<int, Solution> AddSolutions(int solver, Program *p,
                                       const std::vector<Solution> &partial) {
    std::map<int, Solution> solutions;
    int cost = p->cost[solver];
    Choice c = {solver, p};
    solutions.insert({1, MakeSolution(c)});
    for (auto &part : partial) {
      int penalty = Penalty(solver, *part.path.begin());
      int len = part.path.size();
      std::vector<Choice> path;
      path.push_back(c);
      int new_score = part.score + cost + (cost + penalty) * len;
      int new_total = part.total_time + (cost + penalty);
      path.insert(path.end(), part.path.begin(), part.path.end());
      Solution s = {path, new_score, new_total};
      solutions.insert({len, s});
    }
    return solutions;
  }

  int Penalty(int solver, const Choice &c) {
    if (solver != c.solver) {
      return delay_;
    }
    return 0;
  }

  int NumProblems() const { return programs_[0].size(); }

private:
  std::array<std::vector<Program *>, 3> programs_;
  int delay_;
  int total_time_;
};
#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <set>

#include "data.h"

std::array<std::vector<Program *>, 3> Order(std::vector<Program> &programs) {
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
      : storage_(std::move(programs)), delay_(delay), total_time_(total_time) {
    programs_ = Order(storage_);
    for (auto &ps : programs_) {
      std::cerr << "Ordered programs: ";
      for (auto *p : ps) {
        std::cerr << p->index << " ";
      }
      std::cerr << std::endl;
    }
  }

  void PopulateGraph() {
    std::cerr << "Populate!" << std::endl;
    for (int i = 0; i <= 2; i++) {
      std::cerr << "Start with " << i << std::endl;
      Program *p = *programs_[i].begin();
      Solution initial = MakeSolution({i, p});
      if (initial.total_time > total_time_) {
        continue;
      }
      p->solutions[i] = Iterate(p, initial);
      std::cerr << "Processed " << i << " started at " << p->index << std::endl
                << std::endl;
    }
  }

  Solution BestSolution() {
    Solution best;
    for (int i = 0; i <= 2; i++) {
      Program *p = *programs_[i].begin();
      Solution &s = p->solutions[i];
      std::cerr << "Candidate (" << p->index << "," << i << ") "
                << s.path.size() << " score: " << s.score << " total time "
                << s.total_time << std::endl;
      PrintSolution(s);
      if (s.Better(best)) {
        best = s;
      }
    }
    return best;
  }

  void Add(int current_solver, Solution &s, Choice c) {
    Program *p = c.p;
    int cost_to_use = p->cost[c.solver] + Penalty(current_solver, c);
    s.total_time = cost_to_use + s.total_time;
    s.score = s.score + s.total_time;
    s.path.push_back(c);
  }

  std::vector<Choice> Candidates(int solver, int total_time,
                                 const std::set<int> &visited) {
    std::vector<Choice> candidates;
    int best = INT_MAX;
    for (int a = 0; a <= 2; a++) {
      Program *p = Next(programs_[a], visited);
      if (p == nullptr) {
        continue;
      }
      Choice c = {a, p};
      int cost = c.cost() + Penalty(solver, c);
      best = std::min(best, cost);
      candidates.push_back(c);
    }

    auto iter = candidates.end();
    while (iter != candidates.begin()) {
      iter--;
      if (iter->cost() - best > delay_) {
        candidates.erase(iter);
      }
    }

    return candidates;
  }

  // Returns the best solution that goes through `cur `.
  Solution Iterate(Program *cur, const Solution &head) {
    std::set<int> visited;
    for (const Choice &c : head.path) {
      visited.insert(c.p->index);
    }

    int solver = head.path.back().solver;
    std::vector<Choice> candidates =
        Candidates(solver, head.total_time, visited);
    std::vector<Solution> candidate_solutions;
    for (auto &c : candidates) {
      Solution candidate = head;
      Add(solver, candidate, c);
      PrintSolution(candidate);

      if (candidate.total_time > total_time_) {
        std::cerr << "Path is too expensive. Stop." << std::endl;
        continue;
      }

      std::cerr << "Iterate for option" << std::endl;
      Solution cs = Iterate(c.p, candidate);
      candidate_solutions.push_back(cs);
      //  }
      std::cerr << "Computed " << c.p->index << " for " << c.solver
                << std::endl;
      PrintSolution(cs);
    }

    if (candidate_solutions.empty()) {
      // no candidates
      return head;
    }

    std::cerr << "Pick best from " << candidate_solutions.size()
              << " candidates" << std::endl;
    Solution best = candidate_solutions.front();
    for (Solution &sol : candidate_solutions) {
      PrintSolution(sol);
      if (sol.Better(best)) {
        std::cerr << "Better" << std::endl;
        best = sol;
      }
    }
    std::cerr << "Best is ";
    PrintSolution(best);
    std::cerr << std::endl;
    return best;
  }

  int Penalty(int solver, const Choice &c) {
    if (solver != c.solver) {
      return delay_;
    }
    return 0;
  }

  int NumProblems() const { return programs_[0].size(); }

private:
  std::vector<Program> storage_;
  std::array<std::vector<Program *>, 3> programs_;
  int delay_;
  int total_time_;
};
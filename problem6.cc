#include <algorithm>
#include <iostream>
#include <limits.h>
#include <utility>
#include <vector>

struct Program {
  int scores[3];
  int index;
};

struct Result {
  int problems;
  int score;
};

int score(const std::vector<std::pair<const Program *, int>> &solution,
          int delay) {
  if (solution.empty()) {
    return INT_MAX;
  }
  int total_time = 0;
  int score = 0;
  int current_solver = solution.begin()->second;
  for (const auto &a : solution) {
    if (a.second != current_solver) {
      total_time += delay;
    }
    current_solver = a.second;
    int time = a.first->scores[current_solver];
    total_time += time;
    score += total_time;
    std::cerr << "Problem : " << a.first->index << " Solver: " << current_solver
              << " Time: " << time << " Total: " << total_time << std::endl;
  }
  std::cerr << "Programs solved: " << solution.size() << std::endl;
  std::cerr << "Total time: " << total_time << std::endl;
  std::cerr << "Score: " << score << std::endl;
  return score;
}

std::vector<std::pair<const Program *, int>>
solveInternal(int delay, int remaining_time, int cur_solver,
              std::vector<const Program *> programs) {
  std::cerr << "Remaining time: " << remaining_time << std::endl;
  int most_problems = 0;
  int best_score = INT_MAX;
  std::vector<std::pair<const Program *, int>> best_answer;
  for (const auto *p : programs) {
    std::vector<const Program *> refs(programs.begin(), programs.end());
    auto iter = std::find(refs.begin(), refs.end(), p);
    refs.erase(iter);
    for (int solver : {0, 1, 2}) {
      int time = p->scores[solver];
      if (solver != cur_solver) {
        time += delay;
      }
      if (time > remaining_time) {
        continue;
      }
      auto answer = solveInternal(delay, remaining_time - time, solver, refs);
      answer.insert(answer.begin(), {p, solver});
      if (answer.size() >= most_problems) {
        if (answer.size() > most_problems) {
          best_score = INT_MAX;
        }
        int sc = score(answer, delay);
        if (sc < best_score) {
          std::cerr << "New local best!" << std::endl;
          most_problems = answer.size();
          best_score = sc;
          best_answer = answer;
        }
      }
    }
  }
  return best_answer;
}

std::vector<const Program*> sortForUser(int user, const std::vector<const Program *> &programs){
  std::vector<const Program*> sorted(programs.begin(), programs.end());
  std::sort(sorted.begin(), sorted.end(), [user](const Program *a, const Program *b)
    {
      return a->scores[user] < b->scores[user];
    });
    return sorted;
}

Result solve(int delay, int total_time,
             const std::vector<const Program *> &programs) {
  int most_problems = 0;
  int best_score = INT_MAX;
  std::vector<std::pair<const Program *, int>> best_answer;

  std::vector<const Program*> sorted_times[] = {sortForUser(0, programs),sortForUser(1,programs), sortForUser(2,programs)};

  int solver = 0;
  int min_time = INT_MAX;
  const Program* first = nullptr;
  for (int i=0; i<2; i++) {
    const Program* front = *(sorted_times[i].begin());
    int time = front->scores[i];
    if (time < min_time) {
      min_time = time;
      solver = i;
      first = front;
    }
  }
  
  std::cerr << "SOLUTION!" << std::endl;

  Result r;
  r.problems = best_answer.size();
  r.score = score(best_answer, delay);
  return r;
}

std::vector<const Program *> toPtrs(std::vector<Program> &programs) {
  std::vector<const Program *> ret;
  for (auto &p : programs) {
    ret.push_back(&p);
  }
  return ret;
}

int main() {
  std::cerr << "This is a log" << std::endl;

  int n;
  int s;
  int t;
  std::cin >> n >> s >> t;

  std::cerr << "n=" << n << " s=" << s << " t=" << t << std::endl;
  std::vector<Program> programs;
  for (int i = 0; i < n; i++) {
    Program p;
    std::cin >> p.scores[0] >> p.scores[1] >> p.scores[2];
    p.index = i;
    programs.push_back(p);
  }
  std::cerr << "Programs read: " << programs.size() << std::endl;

  auto result = solve(s, t, toPtrs(programs));
  std::cout << result.problems << " " << result.score << std::endl;
  return 0;
}

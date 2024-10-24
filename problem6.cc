#include <algorithm>
#include <iostream>
#include <limits.h>
#include <set>
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

struct TimeQueue {
  const std::vector<const Program *>::iterator begin;
  const std::vector<const Program *>::iterator end;

  std::vector<const Program *>::iterator cur;

  std::vector<const Program *>::iterator next(const std::set<int> &solved) {
    while (cur != end) {
      cur++;
      if (cur == end) {
	      break;
      }
      auto iter = solved.find((*cur)->index);
      if (iter == solved.end()) {
        break;
      }
    }

    return cur;
  }
};

std::vector<std::pair<const Program *, int>>
solveInternal(int delay, int remaining_time, int cur_solver,
              TimeQueue queues[3], std::set<int> visited);

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
solveI(int delay, int remaining_time, int i, int cur_solver,
       TimeQueue queues[3], std::set<int> visited) {
  auto cur = queues[i].cur;
  if (cur == queues[i].end) {
    return {};
  }

  const Program *front = *(cur);
  std::cerr << "Processing program: " << front->index << " for user " << i
            << std::endl;
  if (visited.find(front->index) != visited.end()) {
    cur = queues[i].next(visited);
    if (cur == queues[i].end) {
      return {};
    }
    front = *(cur);
  }

  int time = front->scores[i];
  if (i != cur_solver) {
    time += delay;
  }
  std::cerr << "Time for problem " << time << std::endl;

  if (remaining_time - time < 0) {
    return {};
  }

  visited.insert(front->index);
  auto iter = queues[i].next(visited);

  std::vector<std::pair<const Program *, int>> partial_result;
  partial_result.push_back({front, i});
  if (iter == queues[i].end) {
    return partial_result;
  }

  std::cerr << "Solve internal again" << std::endl;

  auto solution =
      solveInternal(delay, remaining_time - time, i, queues, visited);
  partial_result.insert(partial_result.end(), solution.begin(), solution.end());
  return partial_result;
}

std::vector<std::pair<const Program *, int>>
bestPartial(int cur_solver, std::vector<std::pair<const Program *, int>> results[3],
            int delay) {
  int min_score = INT_MAX;
  int most_problems = 0;
  int best = 0;
  for (int i = 0; i < 3; i++) {
    if (results[i].size() >= most_problems) {
      if (results[i].size() > most_problems) {
        most_problems = results[i].size();
        min_score = INT_MAX;
      }
      int val = score(results[i], delay);
      if (i != cur_solver) {
        val += delay;
      }
      if (val <= min_score) {
        min_score = val;
        best = i;
      }
    }
  }
  return results[best];
}

std::vector<std::pair<const Program *, int>>
solveInternal(int delay, int remaining_time, int cur_solver,
              TimeQueue queues[3], std::set<int> visited) {
  std::cerr << "Recurse! " << cur_solver
            << " Remaining time: " << remaining_time << std::endl;
  std::vector<std::pair<const Program *, int>> partial_result[3];
  for (int i = 0; i <= 2; i++) {
    partial_result[i] =
        solveI(delay, remaining_time, i, cur_solver, queues, visited);
    std::cerr << "Solved for user " << i << std::endl;
  }

  return bestPartial(cur_solver, partial_result, delay);
}

std::vector<const Program *>
sortForUser(int user, const std::vector<const Program *> &programs) {
  std::vector<const Program *> sorted(programs.begin(), programs.end());
  std::sort(sorted.begin(), sorted.end(),
            [user](const Program *a, const Program *b) {
              return a->scores[user] < b->scores[user];
            });
  return sorted;
}

Result solve(int delay, int total_time,
             const std::vector<const Program *> &programs) {
  std::cerr << "Begin solve";
  std::vector<const Program *> sorted_times[3] = {sortForUser(0, programs),
                                                  sortForUser(1, programs),
                                                  sortForUser(2, programs)};

  std::vector<std::pair<const Program *, int>> answers[3];
  for (int i = 0; i <= 2; i++) {
    std::cerr << "Start with user " << i << std::endl;
    TimeQueue queues[3] = {{.begin = sorted_times[0].begin(),
                            .end = sorted_times[0].end(),
                            .cur = sorted_times[0].begin()},
                           {.begin = sorted_times[1].begin(),
                            .end = sorted_times[1].end(),
                            .cur = sorted_times[1].begin()},
                           {.begin = sorted_times[2].begin(),
                            .end = sorted_times[2].end(),
                            .cur = sorted_times[2].begin()}};

    std::set<int> visited;
    TimeQueue &cur_queue = queues[i];
    const Program *first = *(cur_queue.cur);
    int remaining_time = total_time - first->scores[i];
    visited.insert(first->index);
    cur_queue.next(visited);
    answers[i].push_back({first, i});
    TimeQueue ptr_queues[3] = {queues[0], queues[1], queues[2]};
    auto partial = solveInternal(delay, remaining_time, i, ptr_queues, visited);
    answers[i].insert(answers[i].end(), partial.begin(), partial.end());
  }

  int most_problems = 0;
  int best_score = INT_MAX;
  std::vector<std::pair<const Program *, int>> best_answer;
  for (int i = 0; i <= 2; i++) {
    if (answers[i].size() >= most_problems) {
      if (answers[i].size() > most_problems) {
        best_score = INT_MAX;
      }
      int val = score(answers[i], delay);
      if (val < best_score) {
        best_score = val;
        best_answer = answers[i];
        most_problems = answers[i].size();
      }
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

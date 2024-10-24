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
      auto iter = solved.find((*cur)->index);
      if (iter == solved.end()) {
        break;
      }
    }

    return cur;
  }
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
              TimeQueue *queues[3], std::set<int> &visited) {
  std::cerr << "Recurse! " << cur_solver
            << " Remaining time: " << remaining_time << std::endl;
  int solver = 0;
  int min_time = INT_MAX;
  const Program *first = nullptr;
  for (int i = 0; i < 2; i++) {
    const Program *front = *(queues[i]->cur);
    if (visited.find(front->index) != visited.end()) {
      front = *(queues[i]->next(visited));
    }

    int time = front->scores[i];
    if (i != cur_solver) {
      time += delay;
    }
    if (time < min_time) {
      min_time = time;
      solver = i;
      first = front;
    }
  }
  if (remaining_time - min_time < 0) {
    return {};
  }
  visited.insert(first->index);
  auto iter = queues[solver]->next(visited);

  std::vector<std::pair<const Program *, int>> partial_result = {
      {first, solver}};
  if (iter == queues[solver]->end) {
    return partial_result;
  }

  auto solution =
      solveInternal(delay, remaining_time - min_time, solver, queues, visited);
  partial_result.insert(partial_result.end(), solution.begin(), solution.end());
  return partial_result;
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
    TimeQueue *ptr_queues[3] = {&queues[0], &queues[1], &queues[2]};
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

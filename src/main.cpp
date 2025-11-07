#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <numeric>
#include <cassert>

#define N_ACTIONS 2
#define N_ITERATIONS 10000

double cfr(std::string history, int card_1, int card_2, double pr_1, double pr_2, double pr_c);

struct InfoSet {
    int num_actions;
    
    std::vector<double> regret_sum;
    std::vector<double> strategy;
    std::vector<double> strategy_sum;

    double reach_pr;
    double reach_pr_sum;


    InfoSet(int num_actions = N_ACTIONS)
        : num_actions(num_actions),
        regret_sum(num_actions, 0.),
        strategy(num_actions, 1. / num_actions),
        strategy_sum(num_actions, 0.),
        reach_pr(0.),
        reach_pr_sum(0.) {}

    void next_strategy() {
        for (int i = 0; i < num_actions; ++i)
            strategy_sum[i] += reach_pr * strategy[i];

        calc_strategy();
        reach_pr_sum += reach_pr;
        reach_pr = 0.;
    }

    void calc_strategy() {
        double total = 0.;

        for (int i = 0; i < num_actions; ++i) {
            strategy[i] = std::max(regret_sum[i], 0.);
            total += strategy[i];
        }
        
        if (total > 0) {
            for (int i = 0; i < num_actions; ++i) {
                strategy[i] = strategy[i] / total;
            }
        }
        else {
            for (int i = 0; i < num_actions; ++i) {
                strategy[i] = 1. / (double)num_actions;
            }
        }
    }

    std::vector<double> get_average_strategy() const {
        std::vector<double> average_strategy = strategy;

        double total = 0.;

        for (int i = 0; i < num_actions; ++i) {
            average_strategy[i] = (reach_pr_sum == 0.) ? average_strategy[i] : strategy_sum[i] / reach_pr_sum;
            average_strategy[i] = (average_strategy[i] < 0.001) ? 0. : average_strategy[i];

            total += average_strategy[i];
        }

        if (total == 0.)
            return average_strategy;

        for (int i = 0; i < num_actions; ++i) {
            average_strategy[i] /= total;
        }

        return average_strategy;
    }
};

std::unordered_map<std::string, InfoSet> i_map;
std::vector<std::string> cards = { "J", "Q", "K" };
std::vector<std::string> actions = { "c", "b" };

InfoSet& get_info_set(const std::string& history, int card) {
    std::string key = cards[card] + (history.empty() ? "" : " " + history);

    if (i_map.find(key) == i_map.end())
        i_map[key] = InfoSet();

    return i_map[key];
}

bool is_chance_node(const std::string& history) {
    return history.length() == 0;
}

double chance_util() {
    double expected_value = 0.;
    int n_possibilities = 6;

    for (int i = 0; i < cards.size(); ++i) {
        for (int j = 0; j < cards.size(); ++j) {
            if (i != j) {
                expected_value += cfr("rr", i, j, 1., 1., 1./(double) n_possibilities);
            }
        }
    }

    return expected_value / (double) n_possibilities;
}

bool is_terminal(const std::string& history) {
    return ((history == "rrcc") || (history == "rrcbc") ||
        (history == "rrcbb") || (history == "rrbc") ||
        (history == "rrbb"));
}

double terminal_utils(const std::string& history, int card_1, int card_2) {
    int n = static_cast<int>(history.length());
    int card_player = (n % 2 == 0) ? card_1 : card_2;
    int card_opponent = (n % 2 == 0) ? card_2 : card_1;

    if ((history == "rrcbc") || (history == "rrbc")) {
        return 1.;
    }
    else if (history == "rrcc") {
        return (card_player > card_opponent) ? 1. : -1.;
    }

    assert((history == "rrcbb") || (history == "rrbb"));

    return (card_player > card_opponent) ? 2. : -2.;
}

double cfr(std::string history = "", int card_1 = -1, int card_2 = -1, double pr_1 = 1., double pr_2 = 1., double pr_c = 1.) {
    if (is_chance_node(history))
        return chance_util();

    if (is_terminal(history))
        return terminal_utils(history, card_1, card_2);

    int n = static_cast<int>(history.length());
    bool is_player_1 = (n % 2 == 0);
    auto& info_set = get_info_set(history, is_player_1 ? card_1 : card_2);

    if (is_player_1)
        info_set.reach_pr += pr_1;
    else
        info_set.reach_pr += pr_2;

    std::vector<double> actions_utils(N_ACTIONS, 0.);

    double util = 0.;

    for (int i = 0; i < actions.size(); ++i) {
        if (is_player_1)
            actions_utils[i] = -1. * cfr(history + actions[i], card_1, card_2, pr_1 * info_set.strategy[i], pr_2, pr_c);
        else
            actions_utils[i] = -1. * cfr(history + actions[i], card_1, card_2, pr_1, pr_2 * info_set.strategy[i], pr_c);
    
        util += actions_utils[i] * info_set.strategy[i];
    }

    for (int i = 0; i < actions_utils.size(); ++i)
        info_set.regret_sum[i] += ((is_player_1) ? pr_2 : pr_1) * pr_c * (actions_utils[i] - util);

    return util;
}

void display_result(double expected_game_value) {
    std::cout << "Player 1 expected value: " << expected_game_value << std::endl;
    std::cout << "Player 2 expected value: " << -expected_game_value << std::endl;

    std::cout << std::endl;
    
    std::vector<std::pair<std::string, InfoSet>> sorted_items(
        i_map.begin(), i_map.end()
    );

    std::sort(sorted_items.begin(), sorted_items.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        }
    );

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Player 1 strategies: " << std::endl;

    for (auto& item : sorted_items) {
        if (item.first.length() % 2 == 0) {
            auto avg = item.second.get_average_strategy();
            std::cout << std::left << std::setw(8) << item.first
                << " [" << avg[0] << ", " << avg[1] << "]\n";
        }
    }

    std::cout << std::endl;

    std::cout << "Player 2 strategies: " << std::endl;

    for (auto& item : sorted_items) {
        if (item.first.length() % 2 == 1) {
            auto avg = item.second.get_average_strategy();
            std::cout << std::left << std::setw(8) << item.first
                << " [" << avg[0] << ", " << avg[1] << "]\n";
        }
    }
}

int main() {
    double expected_game_value = 0.;

    for (int i = 0; i < N_ITERATIONS; ++i) {
        expected_game_value += cfr();

        for (auto& elem : i_map)
            elem.second.next_strategy();
    }

    expected_game_value /= (double)N_ITERATIONS;

    display_result(expected_game_value);

    return 0;
}
#ifndef __AGENT_HEAD_FILE_
#define __AGENT_HEAD_FILE_

#include <vector>

#define MAX_NUM_PLAYERS 4
#define NUM_STACKS 4
#define NUM_ROUNDS 10
#define NUM_CARDS 104
#define STACK_DEPTH 5

const int NIMMTS[105] = {0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 5, 1, 1, 1, 2, 1, 1, 1, 1, 3, 1, 5, 1, 1, 2, 1, 1, 1, 1, 3, 1,
                         1, 5, 1, 2, 1, 1, 1, 1, 3, 1, 1, 1, 5, 2, 1, 1, 1, 1, 3, 1, 1, 1, 1, 7, 1, 1, 1, 1, 3, 1, 1, 1,
                         1, 2, 5, 1, 1, 1, 3, 1, 1, 1, 1, 2, 1, 5, 1, 1, 3, 1, 1, 1, 1, 2, 1, 1, 5, 1, 3, 1, 1, 1, 1, 2,
                         1, 1, 1, 5, 3, 1, 1, 1, 1};

class Agent {
public :
    virtual int policy(int player_id, int current_round, std::vector<int> hands, int num_players, int num_cards,
                       std::vector<int> playeds) = 0;

    virtual int policy_min(int player_id, int current_round, std::vector<int> hands, int num_players, int num_cards,
                           std::vector<int> playeds) = 0;
};

#endif
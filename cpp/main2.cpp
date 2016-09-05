#include "agent.h"
#include "agent.cpp"
#include "agent_easiest.h"
#include <ctime>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>

using namespace std;

class Table {
private:
    int cards[110];
    int choice[MAX_NUM_PLAYERS], args[MAX_NUM_PLAYERS];

    struct Env {
        vector<int> stacks[NUM_STACKS];
        vector<int> score;
        vector<int> handcards[MAX_NUM_PLAYERS];
        int num_players;

        Env(int num_players) {
            this->num_players = num_players;
            for (int i = 0; i < NUM_STACKS; ++i) {
                stacks[i].clear();
            }
            score.clear();
            for (int i = 0; i < num_players; ++i) {
                handcards[i].clear();
                score.push_back(0);
            }
        }

        int push(int id, int card, int select = -1) {
            int sid = -1;
            if (select == -1) {
                for (int i = 0; i < NUM_STACKS; ++i)
                    if (stacks[i].back() < card) if (sid == -1 or stacks[i].back() > stacks[sid].back())
                        sid = i;
                if (sid == -1)
                    return -1;
            }
            else sid = select;
            if (stacks[sid].back() > card || stacks[sid].size() == STACK_DEPTH) {
                int punish = 0;
                for (int i = 0; i < stacks[sid].size(); ++i)
                    punish += NIMMTS[stacks[sid][i]];
                stacks[sid].clear();
                score[id] += punish;
            }
            stacks[sid].push_back(card);
            return 0;
        }

        vector<int> show_handcards(int id) {
            vector<int> ret;
            ret = handcards[id];
            sort(ret.begin(), ret.end());
            return ret;
        }

        void to_playeds(vector<int> &vec) {
            for (int i = 0; i < NUM_STACKS; ++i) {
                for (int j = 0; j < STACK_DEPTH; ++j)
                    vec.push_back(j >= stacks[i].size() ? 0 : stacks[i][j]);
            }
        }
    };

    vector<int> sum_score(vector<int> score1, vector<int> score2) {
        vector<int> ret;
        ret.resize(score1.size());
        for (unsigned i = 0; i < score1.size(); ++i)
            ret[i] = score1[i] + score2[i];
        return ret;
    }

public:
    Table() {
        for (int i = 1; i <= NUM_CARDS; ++i)
            cards[i] = i;
    }

    vector<int> run(vector<Agent *> agents) {
        random_shuffle(cards + 1, cards + NUM_CARDS + 1);
        int *p = cards + 1;
        int num_players = agents.size();
        vector<int> playeds;
        Env *env = new Env(num_players);
        for (int i = 0; i < 4; ++i) {
            env->stacks[i].push_back(*(p++));
        }
        for (int i = 0; i < num_players; ++i) {
            for (int j = 0; j < NUM_ROUNDS; ++j)
                env->handcards[i].push_back(*(p++));
        }
        for (int i = 0; i < NUM_ROUNDS; ++i) {
            env->to_playeds(playeds);
            for (int j = 0; j < num_players; ++j) {
                choice[j] = agents[j]->policy(j, i, env->show_handcards(j), num_players, NUM_CARDS, playeds);
                args[j] = j;
            }
            for (int j = 0; j < num_players; ++j)
                for (int k = 1; k < num_players; ++k)
                    if (choice[args[k - 1]] > choice[args[k]])
                        swap(args[k - 1], args[k]);

            for (int j = 0; j < num_players; ++j) {
                playeds.push_back(choice[j]);
                env->handcards[j].erase(find(env->handcards[j].begin(), env->handcards[j].end(), choice[j]));
            }
            for (int j = 0; j < num_players; ++j) {
                int id = args[j];
                int card = choice[id];
                if (env->push(id, card)) {
                    int sid = agents[id]->policy_min(id, i + 1, env->show_handcards(id), num_players, NUM_CARDS, playeds);
                    env->push(id, card, sid);
                }
            }
        }
        return env->score;
    }

    vector<int> run66(vector<Agent *> agents) {
        vector<int> score;
        score.resize(agents.size());
        while (*max_element(score.begin(), score.end()) < 66)
            score = sum_score(score, run(agents));

        return score;
    }
};

int main(int argc, char **argv) {
    srand((unsigned) time(NULL));

    vector<Agent *> agents;
    Table *table = new Table();
    agents.push_back(new EasiestAgent);
    agents.push_back(new NaiveAgent1v1);

    int cnt = 0;
    for (int i = 1; i <= 500; ++i) {
        vector<int> score = table->run66(agents);
        cnt += score[0] < score[1];
        printf("this game [%d %d], total [%d %d]\n", score[0], score[1], cnt, i - cnt);
    }
}

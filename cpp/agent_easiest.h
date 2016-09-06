#include "agent.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
using namespace std;

class EasiestAgent2 : public Agent {
	int stor_arr[1010], *stor;
	int handcards[10], num_handcards;
	int pre_sum[110], tot_cards;
	int stor_min_id_nimmts;
	int bar[20];
	bool shows[110];
	int _;
	struct Env {
		int stacks[NUM_STACKS][STACK_DEPTH + 1], nimmts[NUM_STACKS], scores[MAX_NUM_PLAYERS];

		inline int to(const int &card) {
			int *s;
			int min_top = -1, sid = -1;
			for (int i = 0; i < NUM_STACKS; ++i) {
				s = stacks[i];
				if (s[s[0]] < card  &&  s[s[0]] > min_top)
					min_top = s[s[0]], sid = i;
			}
			return sid;
		}
		inline int min_id() {
			return min_element(nimmts, nimmts + NUM_STACKS) - nimmts;
		}
	};
	Env *env;

	inline double evaluate(Env *env, const int &card, int next_card) {
		int sid = env->to(card);
		double e = 0;
		if (sid == -1)
			e = (1.0 - (double) pre_sum[card - 1] / tot_cards) * env->nimmts[env->min_id()];
		else {
			int *s = env->stacks[sid];
			if (s[0] == STACK_DEPTH)
				e = (1.0 - (double) (pre_sum[card - 1] - pre_sum[s[s[0]]]) / tot_cards) * env->nimmts[sid];
      else {
        e = (-0.1 + pre_sum[card - 1] - pre_sum[s[s[0]]]) / tot_cards / (STACK_DEPTH - s[0]) * env->nimmts[sid];
		for (int i = 0; i < NUM_STACKS; ++i)
			if (env->stacks[i][env->stacks[i][0]] > card)
				next_card = min(next_card, env->stacks[i][env->stacks[i][0]]);
        if (s[0] == 4)
          e -= (double) (pre_sum[next_card - 1] - pre_sum[card]) / tot_cards * (env->nimmts[sid] + NIMMTS[card]);
        else
          e += (double) (pre_sum[next_card - 1] - pre_sum[card]) / tot_cards * (env->nimmts[sid] + NIMMTS[card]);
      }
    }
		return e;
	}
	
	void Init(int pid, int rd, vector <int> __handcards, int num_players, int num_cards, vector <int> __cards, bool small_mode) {
		memset(shows, 0, sizeof(shows));
		memset(pre_sum, 0, sizeof(pre_sum));
		int card, cid = 0;
		num_handcards = NUM_ROUNDS - rd;
		for (int i = 0; i < num_handcards; ++i) {
			handcards[i] = __handcards[i];
			shows[handcards[i]] = 1;
		}
		sort(handcards, handcards + num_handcards);
			
		rd -= small_mode;
		for (int i = 0; i <= rd; ++i) {
			for (int j = 0; j < NUM_STACKS; ++j) {
				env->stacks[j][0] = 0;
				env->nimmts[j] = 0;
				for (int k = 0; k < STACK_DEPTH; ++k) {
					card = __cards[cid++];
					shows[card] = 1;
					env->stacks[j][k + 1] = card;
					env->stacks[j][0] += (card != 0);
					env->nimmts[j] += NIMMTS[card];
				}
			}
			if ((i != rd) || (small_mode == 1)) {
				for (int j = 0; j < num_players; ++j) {
					card = __cards[cid++];
					shows[card] = 1;
				}
			}
		}
		tot_cards = 0;
		for (int i = 1; i <= NUM_CARDS; ++i) {
			pre_sum[i] = pre_sum[i - 1] + (shows[i] == 0);
			tot_cards += shows[i] == 0;
		}
	}
public:
	EasiestAgent2() {
		env = new Env();
	}
	int policy(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards,cards, 0);
		double cur_e = 1e10;
		int show = 0;
		for (int i = 0; i < handcards.size(); ++i) {
			double e = evaluate(env, handcards[i], i == handcards.size() - 1 ? NUM_CARDS + 1 : handcards[i + 1]);
			if (e < cur_e)
				cur_e = e, show = handcards[i];
		}
		return show;
	}
	int policy_min(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards, cards, 1);
		return env->min_id();
	}
};


class EasiestAgent : public Agent {
	int stor_arr[1010], *stor;
	int handcards[10], num_handcards;
	int pre_sum[110], tot_cards;
	int stor_min_id_nimmts;
	int bar[20];
	bool shows[110];
	int _;
	struct Env {
		int stacks[NUM_STACKS][STACK_DEPTH + 1], nimmts[NUM_STACKS], scores[MAX_NUM_PLAYERS];

		inline int to(const int &card) {
			int *s;
			int min_top = -1, sid = -1;
			for (int i = 0; i < NUM_STACKS; ++i) {
				s = stacks[i];
				if (s[s[0]] < card  &&  s[s[0]] > min_top)
					min_top = s[s[0]], sid = i;
			}
			return sid;
		}
		inline int min_id() {
			return min_element(nimmts, nimmts + NUM_STACKS) - nimmts;
		}
	};
	Env *env;

	inline double evaluate(Env *env, const int &card, const int &next_card) {
		int sid = env->to(card);
		double e = 0;
		if (sid == -1)
			e = (1.0 - (double) pre_sum[card - 1] / tot_cards) * env->nimmts[env->min_id()];
		else {
			int *s = env->stacks[sid];
			if (s[0] == STACK_DEPTH)
				e = (1.0 - (double) (pre_sum[card - 1] - pre_sum[s[s[0]]]) / tot_cards) * env->nimmts[sid];
      else {
        e = (-0.1 + pre_sum[card - 1] - pre_sum[s[s[0]]]) / tot_cards / (STACK_DEPTH - s[0]) * env->nimmts[sid];
        if (s[0] == 4)
          e -= (double) (pre_sum[next_card - 1] - pre_sum[card]) / tot_cards * (env->nimmts[sid] + NIMMTS[card]);
        else
          e += (double) (pre_sum[next_card - 1] - pre_sum[card]) / tot_cards * (env->nimmts[sid] + NIMMTS[card]);
      }
    }
		return e;
	}
	
	void Init(int pid, int rd, vector <int> __handcards, int num_players, int num_cards, vector <int> __cards, bool small_mode) {
		memset(shows, 0, sizeof(shows));
		memset(pre_sum, 0, sizeof(pre_sum));
		int card, cid = 0;
		num_handcards = NUM_ROUNDS - rd;
		for (int i = 0; i < num_handcards; ++i) {
			handcards[i] = __handcards[i];
			shows[handcards[i]] = 1;
		}
		sort(handcards, handcards + num_handcards);
			
		rd -= small_mode;
		for (int i = 0; i <= rd; ++i) {
			for (int j = 0; j < NUM_STACKS; ++j) {
				env->stacks[j][0] = 0;
				env->nimmts[j] = 0;
				for (int k = 0; k < STACK_DEPTH; ++k) {
					card = __cards[cid++];
					shows[card] = 1;
					env->stacks[j][k + 1] = card;
					env->stacks[j][0] += (card != 0);
					env->nimmts[j] += NIMMTS[card];
				}
			}
			if ((i != rd) || (small_mode == 1)) {
				for (int j = 0; j < num_players; ++j) {
					card = __cards[cid++];
					shows[card] = 1;
				}
			}
		}
		tot_cards = 0;
		for (int i = 1; i <= NUM_CARDS; ++i) {
			pre_sum[i] = pre_sum[i - 1] + 1;
			tot_cards += 1;
		}
	}
public:
	EasiestAgent() {
		env = new Env();
	}
	int policy(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards,cards, 0);
		double cur_e = 1e10;
		int show = 0;
		for (int i = 0; i < handcards.size(); ++i) {
			double e = evaluate(env, handcards[i], i == handcards.size() - 1 ? NUM_CARDS + 1 : handcards[i + 1]);
			if (e < cur_e)
				cur_e = e, show = handcards[i];
		}
		return show;
	}
	int policy_min(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards, cards, 1);
		return env->min_id();
	}
};



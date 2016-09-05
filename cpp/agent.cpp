#include "agent.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#define NUM_PLAYERS 2
using namespace std;

class NaiveAgent1v1 : public Agent {
private:
	int stor_arr[1010], *stor;
	int handcards[10], num_handcards;
	int pre_sum[110], tot_cards;
	int stor_min_id_nimmts;
	int bar[20];
	bool shows[110];
	int _;
	int stor_segs_card[5][110];
	double stor_segs_prob[5][110];
	struct Env {
		int *stacks[NUM_STACKS], nimmts[NUM_STACKS], scores[NUM_PLAYERS];

		inline int min_id() {
			return min_element(nimmts, nimmts + NUM_STACKS) - nimmts;
		}
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
		inline int push(const int &card, const int &pid, int *stor) {
			int sid = to(card), punish = 0;
			if (sid == -1)
				sid = min_id();
			int *s = stacks[sid];
			if (s[s[0]] > card  ||  s[0] == STACK_DEPTH) {
				stor[0] = 1, stor[1] = card;
				punish = nimmts[sid];
				nimmts[sid] = NIMMTS[card];
			}
			else {
				for (int i = 0; i <= s[0]; ++i)
					stor[i] = s[i];
				nimmts[sid] += NIMMTS[card];
			}
			stacks[sid] = stor;
			scores[pid] += punish;
		}
	};
	Env *env, stor_env[5];

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
	int gen_segs_to(int *card, double *prob, Env *env) {
		stor_min_id_nimmts = env->min_id();
		int cnt = 0, num = 0;
		double mini = 1e10;
    bar[0] = 0;
		for (int i = 0; i < num_handcards; ++i)
			bar[++num] = handcards[i];
		for (int i = 0; i < NUM_STACKS; ++i)
			bar[++num] = env->stacks[i][env->stacks[i][0]];
		bar[++num] = NUM_CARDS + 1;
		sort(bar, bar + num + 1);
		for (int i = 0; i < num; ++i) {
			int mdc = -1;
			for (int j = bar[i] + 1; j < bar[i + 1]; ++j) {
				if (shows[j] == false) {
					mdc = j;
					break;
				}
			}
			if (mdc == -1)
				continue;
			card[cnt] = mdc;
			prob[cnt] = -evaluate(env, mdc, mdc + 13) * (pre_sum[bar[i + 1] - 1] - pre_sum[bar[i]]) / tot_cards;
			mini = min(mini, prob[cnt]);
			++cnt;
		}
		double tot = 0;
		for (int i = 0; i < cnt; ++i) {
			prob[i] = (prob[i] - mini + fabs(mini) * 0.1);
      prob[i] = prob[i] * prob[i];
			tot += prob[i];
		}
		for (int i = 0; i < cnt; ++i)
			prob[i] = prob[i] / tot;
		return cnt;
	}
	double search(int depth, Env *env) {
		double ret = -1e10;
		if (depth == 3  ||  num_handcards == 0) {
			ret = env->scores[1] - env->scores[0];
			stor_min_id_nimmts = env->min_id();
			for (int i = 0; i < num_handcards; ++i)
				ret -= evaluate(env, handcards[i], i == num_handcards - 1 ? NUM_CARDS : handcards[i + 1]) / num_handcards;
			return ret;
		}
		Env *temp = stor_env + depth;
		int show, card;
		int *segs_card = stor_segs_card[depth], r_card;
		double *segs_prob = stor_segs_prob[depth], prob;
		int num_segs = gen_segs_to(segs_card, segs_prob, env);

		for (int i = 0; i < num_handcards; ++i) {
			double e = 0;
			card = handcards[i];
			for (int j = i; j < num_handcards; ++j)
				handcards[j] = handcards[j + 1];
			num_handcards--;
			for (int j = 0; j < num_segs; ++j) {
				r_card = segs_card[j], prob = segs_prob[j];
				*temp = *env;
				if (r_card < card)
					temp->push(r_card, 1, stor), stor += STACK_DEPTH + 1, temp->push(card, 0, stor), stor += STACK_DEPTH + 1;
				else
					temp->push(card, 0, stor), stor += STACK_DEPTH + 1, temp->push(r_card, 1, stor), stor += STACK_DEPTH + 1;

				shows[r_card] = 1, shows[card] = 1;
				e += search(depth + 1, temp) * prob;
				shows[r_card] = 0, shows[card] = 0;
				stor -= STACK_DEPTH + 1;
        stor -= STACK_DEPTH + 1;
			}
			num_handcards++;
			for (int j = num_handcards - 1; j > i; --j)
				handcards[j] = handcards[j - 1];
			handcards[i] = card;
			if (e > ret)
				ret = e, show = card;
		}
		return depth == 0 ? show : ret;
	}
public:
	NaiveAgent1v1() {
		env = new Env();
		stor = stor_arr;
		for (int i = 0; i < NUM_STACKS; ++i) {
			env->stacks[i] = stor;
			stor += STACK_DEPTH + 1;
		}
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
	int policy(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards) {
		Init(pid, rd, handcards, num_players, num_cards,cards, 0);
		int ret = (int) (search(0, env) + 1e-5);
    //printf("got one!");
		return ret;
	}
	int policy_min(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards) {
		Init(pid, rd, handcards, num_players, num_cards, cards, 1);
		return env->min_id();
	}
};
/*
int main(int argc, char **argv) {
	NaiveAgent1v1 *agent = new NaiveAgent1v1();
}
*/




#include "agent.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#define NUM_PLAYERS 2
using namespace std;

class KbdAgent : public Agent {
private:
	int stor_arr[2010], *stor;
	int handcards[10], num_handcards;
	int pre_sum[110], tot_cards;
	int stor_min_id_nimmts;
	int bar[20];
	bool shows[110];
	int _;
	int maxdepth;
	int stor_segs_card[10][110];
	double stor_segs_prob[10][110];
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
	Env *env, stor_env[10];
public:
	KbdAgent() {
		env = new Env();
		stor = stor_arr;
		for (int i = 0; i < NUM_PLAYERS; ++i) {
			env->scores[i] = 0;
		}
		for (int i = 0; i < NUM_STACKS; ++i) {
			env->stacks[i] = stor;
			stor += STACK_DEPTH + 1;
		}
	}
	void Init(int pid, int rd, vector <int> __handcards, int num_players, int num_cards, vector <int> __cards, bool small_mode, vector <int> scores) {
		memset(shows, 0, sizeof(shows));
		memset(pre_sum, 0, sizeof(pre_sum));
		int card, cid = 0;
		num_handcards = NUM_ROUNDS - rd;
		for (int i = 0; i < num_handcards; ++i) {
			handcards[i] = __handcards[i];
		}
		sort(handcards, handcards + num_handcards);
			
		rd -= small_mode;
		for (int i = 0; i <= rd; ++i) {
			for (int j = 0; j < NUM_STACKS; ++j) {
				env->stacks[j][0] = 0;
				env->nimmts[j] = 0;
				for (int k = 0; k < STACK_DEPTH; ++k) {
					card = __cards[cid++];
					env->stacks[j][k + 1] = card;
					env->stacks[j][0] += (card != 0);
					env->nimmts[j] += NIMMTS[card];
				}
			}
			if ((i != rd) || (small_mode == 1)) {
				for (int j = 0; j < num_players; ++j) {
					card = __cards[cid++];
				}
			}
		}
		for (int i = 0; i < num_players; ++i)
			env->scores[i] = scores[i];
	}
	int policy(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards,cards, 0, scores);
		printf("current stacks : \n");
		for (int i = 0; i < NUM_STACKS; ++i) {
			for (int j = 1; j <= env->stacks[i][0]; ++j)
				printf("%d ", env->stacks[i][j]);
			printf("\n");
		}
		printf("your handcards : ");
		for (int i = 0; i < num_handcards; ++i) {
			printf("%d ", handcards[i]);
		}
		printf("\n");
		printf("scores : ");
		for (int i = 0; i < num_players; ++i)
			printf("%d ", scores[i]);
		printf("\n");
		printf("your choice : ");
		while (true) {
			int x;
			scanf("%d", &x);
			for (int i = 0; i < num_handcards; ++i)
				if (x == handcards[i])
					return x;
		}
	}
	int policy_min(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards, cards, 1, scores);
		return env->min_id();
	}
};
/*
int main(int argc, char **argv) {
	NaiveAgent1v1 *agent = new NaiveAgent1v1();
}
*/




#include "agent.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#define NUM_PLAYERS 2
using namespace std;

class XAgent : public Agent {
private:
	int stor_arr[2010], *stor;
	int num_handcards;
	int pre_sum[110], tot_cards;
	int stor_min_id_nimmts;
	int bar[20];
	int _;
	int maxdepth;
	double v_arr[10][110][110];
	int argmax_arr[10][110];
	double sarg_arr[10][110];
	
	struct Handcards {
		int next[110], last[110];
		int g[NUM_PLAYERS];
		void clear() {
			memset(next, 0, sizeof(next));
			memset(last, 0, sizeof(last));
			memset(g, 0, sizeof(g));
		}
		inline void insert(const int &x) {
			last[next[x]] = x;
			next[last[x]] = x;
		}
		inline void erase(const int &x) {
			last[next[x]] = last[x];
			next[last[x]] = next[x];
		}
		void push(int id, int card) {
			last[g[id]] = card;
			next[card] = g[id];
			g[id] = card;
		}
		void addhead() {
			push(0, 107);
			push(1, 108);
		}
	};
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
				stor[0]++;
				stor[stor[0]] = card;
				nimmts[sid] += NIMMTS[card];
			}
			stacks[sid] = stor;
			scores[pid] += punish;
			return punish;
		}
	};
	struct Prob {
		double p[110];
		double tot;
		void clear() {
			tot = 0;
			for (int i = 1; i <= NUM_CARDS; ++i)
				p[i] = 1, tot += 1;
		}
		void to(int card, double v) {
			tot -= p[card] - v;
			p[card] = v;
		}
		void show(XAgent::Env *env, int card) {
			for (int i = 0; i < card; ++i) {
				
			}
		}
		int choice() {
			double e = rand() * tot / (~0U>>1);
			for (int i = 1; i <= NUM_CARDS; ++i) {
				if (p[i] > 1e-5  &&  e < p[i])
					return i;
				e -= p[i];
			}
		}
	};
	Env *env, stor_env[10];
	Handcards *handcards;
	Prob *prob[2];

	double search(int depth, Env *env) {
		double ret = -1e10;
		if (abs(env->scores[0] - env->scores[1]) >= 8  &&  num_handcards - depth >= 5)
			return env->scores[0] - env->scores[1];
		if (depth == maxdepth  ||  num_handcards - depth == 0)
			return env->scores[0] - env->scores[1];
		
		Env *temp = stor_env + depth;
		int *argmax = argmax_arr[depth];
		double *sarg = sarg_arr[depth];
		double (*v)[110] = v_arr[depth];
		for (int i = 1; i <= NUM_CARDS; ++i)
			argmax[i] = 0;
		double meanv = 1e10;
		int mcard = -1;
		for (int i = handcards->next[handcards->g[0]]; i; i = handcards->next[i]) {
			double meani = 0;
			int cnt = 0;
			for (int j = handcards->next[handcards->g[1]]; j; j = handcards->next[j]) {
				handcards->erase(i);
				handcards->erase(j);
				*temp = *env;
				if (i < j) {
					temp->push(i, 0, stor), stor += STACK_DEPTH + 1;
					temp->push(j, 1, stor), stor += STACK_DEPTH + 1;
				}
				else {
					temp->push(j, 1, stor), stor += STACK_DEPTH + 1;
					temp->push(i, 0, stor), stor += STACK_DEPTH + 1;					
				}
				double e = search(depth + 1, temp);
				if (argmax[i] == 0  ||  e > v[i][argmax[i]])
					argmax[i] = j;
				if (argmax[j] == 0  ||  e < v[j][argmax[j]])
					argmax[j] = i;
				v[i][j] = v[j][i] = e;
				stor -= (STACK_DEPTH + 1) * 2;
				handcards->insert(i);
				handcards->insert(j);
				sarg[cnt++] = e;
			}
			sort(sarg, sarg + cnt);
			for (int j = min(4, num_handcards - depth); j--; )
				meani += sarg[cnt - j - 1];
			if (meani < meanv)
				meanv = meani, mcard = i; 
		}
		double mv = -1;
		int card = -1;
		for (int i = handcards->next[handcards->g[0]]; i; i = handcards->next[i]) {
			int j = argmax[i];
			if (v[j][argmax[j]] == v[j][i]  &&  (card == -1  ||  v[j][i] < mv))
				mv = v[j][i], card = i;
		}
		if (card != -1)
			return depth == 0 ? card : mv;
		return depth == 0 ? mcard : meanv / min(4, num_handcards - depth);
	}
public:
	XAgent () {
		env = new Env();
		handcards = new Handcards();
		prob[0] = new Prob();
		stor = stor_arr;
		for (int i = 0; i < NUM_STACKS; ++i) {
			env->stacks[i] = stor;
			stor += STACK_DEPTH + 1;
		}
	}
	void Init(int pid, int rd, vector <int> __handcards, int num_players, int num_cards, vector <int> __cards, bool small_mode) {
		int card, cid = 0;
		
		handcards->clear();
		prob[0]->clear();
		num_handcards = NUM_ROUNDS - rd;
		sort(__handcards.begin(), __handcards.end());
		for (int i = num_handcards; i--; ) {
			card = __handcards[i];
			handcards->push(0, card);
			prob[0]->to(card, 0);
		}
		rd -= small_mode;
		for (int i = 0; i <= rd; ++i) {
			for (int j = 0; j < NUM_STACKS; ++j) {
				env->stacks[j][0] = 0;
				env->nimmts[j] = 0;
				for (int k = 0; k < STACK_DEPTH; ++k) {
					card = __cards[cid++];
					prob[0]->to(card, 0);
					env->stacks[j][k + 1] = card;
					env->stacks[j][0] += (card != 0);
					env->nimmts[j] += NIMMTS[card];
				}
			}
			if ((i != rd) || (small_mode == 1)) {
				for (int j = 0; j < NUM_PLAYERS; ++j) {
					card = __cards[cid++];
					if (j) prob[0]->show(env, card);
				}
			}
		}
		int rival_hands[10];
		for (int i = 0; i < num_handcards; ++i) {
			card = prob[0]->choice();
			rival_hands[i] = card;
			prob[0]->to(card, 0);
		}
		sort(rival_hands, rival_hands + num_handcards);
		for (int i = num_handcards; i--; ) {
			card = rival_hands[i];
			handcards->push(1, card);
		}
		handcards->addhead();
		
		if (false) {
			printf("XAgent handcards : ");
			for (int i = handcards->next[handcards->g[0]]; i; i = handcards->next[i]) {
				printf("%d ", i);
			}
			printf("\n");
			printf("Predict handcards : ");
			for (int i = handcards->next[handcards->g[1]]; i; i = handcards->next[i]) {
				printf("%d ", i);
			}
			printf("\n");
		}
		
		maxdepth = num_handcards;
		if (num_handcards == 7)
			maxdepth = 5;
		if (num_handcards == 8)
			maxdepth = 4;
		if (num_handcards >= 9)
			maxdepth = 3;
	}
	int policy(int pid, int rd, vector <int> __handcards, int num_players, int num_cards, vector <int> __cards, vector <int> scores) {
		Init(pid, rd, __handcards, num_players, num_cards, __cards, 0);
		int ret = (int) (search(0, env) + 1e-5);
		//printf("%d\n", ret);
		return ret;
	}
	int policy_min(int pid, int rd, vector <int> handcards, int num_players, int num_cards, vector <int> cards, vector <int> scores) {
		Init(pid, rd, handcards, num_players, num_cards, cards, 1);
		return env->min_id();
	}
};
/*
int main(int argc, char **argv) {
	NaiveAgent1v1 *agent = new NaiveAgent1v1();
}
*/




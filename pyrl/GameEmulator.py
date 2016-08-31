#coding=utf-8
from common import *
import copy
import random
import BaseAgent
import sys
import numpy as np
sys.path.append('..')
from agents.RandomAgent import RandomAgent
from agents.KeyboardAgent import KeyboardAgent
class GameEmulator(object):
    def __init__(self):
        self.agent_list = []

    def add_agent(self, agent):
        self.agent_list.append(agent)
        self.num_agents = len(self.agent_list)

    def set_agents(self, agents):
        self.agent_list = agents
        self.num_agents = len(self.agent_list)

    def generate_init_env(self):
        random_cards = range(0, num_cards)
        random.shuffle(random_cards)
        card_stacks = [[random_cards[i],] for i in range(num_card_stack)]
        hand_cards = [sorted(random_cards[num_card_stack + i * num_agent_init_card: num_card_stack + (i+1) * num_agent_init_card]) for i in range(self.num_agents)]
        card_status = [0,]*104
        for i in range(num_agent_init_card):
            card_status[hand_cards[0][i]] = 1
        for i in range(num_card_stack):
            card_status[card_stacks[i][0]] = 2
        #0 : unshowed, 1: on hand, 2: on board | discarded
        return {
            'card_stacks': card_stacks,
            'hand_cards' : hand_cards,
            'card_status': card_status,
            'agent_id': -1,
            'scores' :  [0,] * self.num_agents
        }


    def execute_action(self, game_env, action):
        #返回新的game_env和每个人获得的牛头数punishments
        game_env = copy.deepcopy(game_env)
        card_stacks = game_env['card_stacks']
        hand_cards = game_env['hand_cards']
        card_status = game_env['card_status']
        scores = game_env['scores']
        actions = [(action,0)]
        for i in range(1, self.num_agents):
            game_env["agent_id"] = i
            for card in hand_cards[i-1]:
                card_status[card] = 0
            for card in hand_cards[i]:
                card_status[card] = 1
            actions.append((self.agent_list[i].policy(game_env), i))
        tmp = sorted(actions)
        punishments = [0,] * self.num_agents
        for i in range(self.num_agents):
            agent_id = tmp[i][1]
            card = tmp[i][0]
            if card not in hand_cards[agent_id]:
                print "Error: Player %d send card %d which not in his hand_cards" % (agent_id, card)
            hand_cards[agent_id].remove(card)
            card_status[card] = 2
            target_stack = -1
            for j in range(num_card_stack):
                if card > card_stacks[j][-1]:
                    if target_stack == -1 or card_stacks[j][-1] > card_stacks[target_stack][-1]:
                        target_stack = j
            if target_stack == -1:
                target_stack = self.agent_list[agent_id].policy_min(game_env)
            card_stacks[target_stack], punishments[agent_id] = add_card(card_stacks[target_stack], card)
        for card in hand_cards[self.num_agents - 1]:
            card_status[card] = 0
        for card in hand_cards[0]:
            card_status[card] = 1
        game_env['scores'] = [scores[i] + punishments[i] for i in range(self.num_agents)]
        return game_env, punishments

    def generate_midgame_env(self, num_round = num_agent_init_card):
        #指定num_round可以返回特定轮数过后的局面,默认为玩完一整局
        game_env = self.generate_init_env()
        for j in range(num_round):
            game_env["agent_id"] = 0
            action0 = self.agent_list[0].policy(game_env)
            game_env, punishments = self.execute_action(game_env, action0)
        return game_env

    def emulate(self, num_round = 1):
        for i in range(num_round):
           print self.generate_midgame_env(num_agent_init_card)["scores"]

    def test_single(self, num_round = 100):
        cnt_win = [0,] * len(self.agent_list)
        for i in range(num_round):
            scores = self.generate_midgame_env(num_agent_init_card)["scores"]
            print scores
            cnt_win[np.argmin(scores)] += 1
        print cnt_win

    def test_66(self, num_round=100):
        cnt_win = [0, ] * len(self.agent_list)
        for i in range(num_round):
            scores = [0,] * len(self.agent_list)
            while max(scores) < 66:
                score = self.generate_midgame_env(num_agent_init_card)["scores"]
                scores = [scores[j] + score[j] for j in range(len(self.agent_list))]
            cnt_win[np.argmin(scores)] += 1
            print "Round %d :" % i, scores, cnt_win
        print cnt_win


if __name__ == "__main__":
    gameEmulator = GameEmulator()
    #自己的Agent（调用execute_action的那个）编号默认为0，应当被第一个添加进去
    gameEmulator.add_agent(KeyboardAgent())
    gameEmulator.add_agent(RandomAgent())
    gameEmulator.emulate(1)

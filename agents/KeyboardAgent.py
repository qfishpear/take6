import sys;
sys.path.append('..')
from pyrl.BaseAgent import BaseAgent
from IPython import embed
from pyrl.common import *
import random
class KeyboardAgent(BaseAgent):
    def __init__(self):
        pass

    def policy(self, agentEnv):
        card_stacks = agentEnv["card_stacks"]
        handcards = agentEnv["hand_cards"]
        agent_id = agentEnv["agent_id"]
        scores = agentEnv["scores"]
        print "Card Stacks:"
        for stack in card_stacks:
            print stack
        print "Your hand cards:"
        print handcards[agent_id]
        print "Current Scores"
        print scores
        return int(raw_input("Your choice: "))

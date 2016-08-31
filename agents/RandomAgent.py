import sys;
sys.path.append('..')
from pyrl.BaseAgent import BaseAgent
import random
class RandomAgent(BaseAgent):
    def __init__(self):
        pass
    def policy(self, agentEnv):
        handcards = agentEnv["hand_cards"]
        agent_id = agentEnv["agent_id"]
        return random.choice(handcards[agent_id])

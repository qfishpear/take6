from common import *
class BaseAgent(object):
    def __init__(self):
        pass
    def policy(self, agentEnv):
        pass
    def policy_min(self, agentEnv):
        card_stacks = agentEnv["card_stacks"]
        target_stack = 0
        for i in range(num_card_stack):
            if count_nimmts(card_stacks[i]) < count_nimmts(card_stacks[target_stack]):
                target_stack = i
        return target_stack
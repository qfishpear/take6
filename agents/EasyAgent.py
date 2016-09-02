import numpy as np
import sys
sys.path.append( '..' )
import pyrl.common as common
from pyrl.common import count_nimmts
from pyrl.BaseAgent import BaseAgent

class EasyAgent(BaseAgent):
    def policy(self, agentEnv):
        def evaluate(index):
            pass

        STACK_VOL = 5
        INF = 1e10
        NUM_CARDS = common.num_cards

        id = agentEnv['agent_id']
        cards = np.array(agentEnv['hand_cards'][id])
        stacks = np.array(agentEnv['card_stacks'])

        eval = [evaluate(i) for i in range(len(cards))]
        ret = cards[np.argmin([evaluate(i) for i in range(len(cards))])]

        return ret

    def policy_min(self, agentEnv):
        stacks = agentEnv['card_stacks']
        ret = np.argmin([count_nimmts(s) for s in stacks])

        return ret

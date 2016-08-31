import numpy as np
import common
from common import count_nimmts
from BaseAgent import BaseAgent

class LargeToSmallAgent(BaseAgent):
	def policy(self, agentEnv):
		id = agentEnv['agent_id']
		cards = agentEnv['hand_cards'][id]
		ret = cards[-1]
		return ret

class SmallToLargeAgent(BaseAgent):
	def policy(self, agentEnv):
		id = agentEnv['agent_id']
		cards = agentEnv['hand_cards'][id]
		ret = cards[0]
		return ret

class DrTrivalAgent(BaseAgent):
	def policy(self, agentEnv):
		id = agentEnv['agent_id']
		cards = agentEnv['hand_cards'][id]
		ret = cards[-1]
		return ret
	def policy_min(self, agentEnv):
		stacks = agentEnv['card_stacks']
		return np.random.randint(len(stacks))
		#ret = np.argmin(count_nimmts(s) for s in stacks)
		#return ret
		
class EasiestAgent(BaseAgent):
	def policy(self, agentEnv):
		def evaluate(index):
			card = cards[index]
			choices = stacks[np.array([s[-1] < card for s in stacks])]
		
			if len(choices) == 0:
				e = (1.0 - card / NUM_CARDS) * min(count_nimmts(s) for s in stacks)
			else:
				stack = choices[np.argmax([s[-1] for s in choices])]
				if len(stack) == STACK_VOL:
					e = (1.0 - (card - stack[-1]) / NUM_CARDS) * count_nimmts(stack)
				else:
					e = ((-0.1 + card - stack[-1]) / NUM_CARDS) / (STACK_VOL - len(stack))
					next = cards[index + 1] if index < len(cards)-1 else NUM_CARDS
					e += ((len(stack) == STACK_VOL-1)*2-1) * (1.0 - (next - card)) / NUM_CARDS
			return e
		
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
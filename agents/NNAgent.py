import sys
sys.path.append("..")
import numpy as np

from BaseAgent import BaseAgent
from net.train import normalize, get_model
from IPython import embed

class NNAgent(BaseAgent):
    def __init__(self,model_path):
        self.model = get_model()
        self.model.load_weights(model_path)

    def policy(self, agentEnv):
        agent_id = agentEnv["agent_id"]
        hand_card = agentEnv["hand_cards"][agent_id]
        toFeed = []
        for choice in hand_card:
            next_dat = {'state': agentEnv, 'action': choice}
            toFeed.append(normalize(next_dat))
        toFeed = np.array(toFeed)
        Q = self.model.predict(toFeed)
        return hand_card[np.argmax(Q)]





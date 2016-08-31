import sys
sys.path.append("..")
import numpy as np

from pyrl.BaseAgent import BaseAgent
from net.model import normalize, get_model
from IPython import embed

class NNAgent(BaseAgent):
    def __init__(self, model_path = None, model = None):
        self.model = get_model()
        if model_path != None:
            self.model.load_weights(model_path)
        if model != None:
            self.model = model

    def policy(self, agentEnv):
        agent_id = agentEnv["agent_id"]
        hand_card = agentEnv["hand_cards"][agent_id]
        toFeed = []
        for choice in hand_card:
            next_dat = {'state': agentEnv, 'action': choice}
            toFeed.append(normalize(next_dat))
        toFeed = np.array(toFeed)
        Q = self.model.predict(toFeed)
        return hand_card[np.argmin(Q)]





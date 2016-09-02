from GameEmulator import GameEmulator
import sys
sys.path.append( '..' )
from agents.RandomAgent import RandomAgent
from agents.YXJAgent import EasiestAgent,EasiestAgent2
from agents.NNAgent import NNAgent
from agents.KeyboardAgent import KeyboardAgent
from agents.EasyAgent import EasyAgent

if __name__ == "__main__":
    gameEmulator = GameEmulator()
    gameEmulator.add_agent(NNAgent(model_path = "/home/superfish/take6/net/models/"+sys.argv[1]+".h5"))

    #gameEmulator.add_agent(KeyboardAgent())
    #gameEmulator.add_agent(EasyAgent())
    gameEmulator.add_agent(EasiestAgent())
    #gameEmulator.add_agent(RandomAgent())
    gameEmulator.test_66(100)

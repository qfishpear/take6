from GameEmulator import GameEmulator
import sys
sys.path.append( '..' )
from agents.RandomAgent import RandomAgent
from agents.YXJAgent import EasiestAgent
from agents.NNAgent import NNAgent

if __name__ == "__main__":
    gameEmulator = GameEmulator()
    gameEmulator.add_agent(NNAgent("/home/superfish/take6/net/models/md70.h5"))
    gameEmulator.add_agent(EasiestAgent())
    #gameEmulator.add_agent(RandomAgent())
    gameEmulator.test_66(100)

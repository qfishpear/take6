#coding=utf-8
from __future__ import absolute_import
from __future__ import print_function
from six.moves import range
import keras
from keras.optimizers import SGD, Adadelta, Adagrad, RMSprop
import numpy as np;
import os;
import sys;
sys.path.append( '..' )
from PIL import Image;
from pyrl.GameEmulator import GameEmulator
from agents.RandomAgent import RandomAgent
from agents.YXJAgent import EasiestAgent
from agents.NNAgent import NNAgent
import pyrl.common
from pyrl.common import num_agent_init_card
import random;
from pyrl.BaseAgent import BaseAgent
from IPython import embed
import time
from net.model import normalize, get_model

'''
gameEmulator = GameEmulator()
gameEmulator.add_agent(DummyAgent())
gameEmulator.add_agent(RandomAgent())
gameEmulator.add_agent(EasiestAgent())
init_env = gameEmulator.generate_env()
current_env = init_env
for round in range(10):
    card = yourchoice
    current_env, punishments = gameEmulator.execute_action(current_env, card)
    # punishments is a list of length(#agents) of num of nimmts each agent get in this round
'''


#EPOCH = int(sys.argv[1]);
Gamma = 0.9
Epsilon = 0.1

def weight_choice(weight):
    t = random.randint(0, int(sum(weight)) - 1)
    for i, val in enumerate(weight):
        t -= val
        if t < 0:
            return i


data_pool = [];
def load_data(EPOCH):
    global Env, Gamma, Epsilon, data_pool, model;
    old_pool = data_pool;
    random.shuffle(old_pool);
    data = []; label = [];
    #weight_list = [1.5**i for i in range(num_agent_init_card)]
    weight_list = [1 for i in range(num_agent_init_card)]
    data_pool = [{'state':Env.generate_midgame_env(weight_choice(weight_list))} for i in range( 200 )];
    data_pool += random.sample(old_pool, min(len(old_pool),900));
    for i in range(len(data_pool)) :
        hand_cards = data_pool[i]['state']['hand_cards'][0]
        data_pool[i]['action'] = hand_cards[random.randint(0, len(hand_cards)-1)]
    #model = keras.models.model_from_json(open('models/md.json', 'r').read());
    #model.load_weights('models/md' + str(EPOCH-1) + '.h5');
    old_pool = [];
    for dat in data_pool :  ### data is (state_t, action_t) tuple.
        state_next, reward_t = Env.execute_action(dat['state'], dat['action']);
        reward_t[ 0 ] *= -1;
        if state_next['hand_cards'][0] == [] :
            data.append(normalize(dat));
            label.append(reward_t[ 0 ]);[ ]
            continue;
        toFeed = [];

        if random.uniform(0, 1) < Epsilon :
            choicelist = [ state_next['hand_cards'][0][random.randint(0, len(state_next['hand_cards'][0])-1)] ]
        else :
            choicelist = state_next['hand_cards'][0]

        count = 0;
        for choice in choicelist :
            next_dat = {'state':state_next, 'action':choice};
            if count < 2:
                old_pool.append(next_dat);
                count += 1;
            toFeed.append(normalize(next_dat))

        toFeed = np.array(toFeed)
        nextQ = model.predict(toFeed);
        data.append(normalize(dat))
        label.append(reward_t[ 0 ] + Gamma * max(nextQ))
    return data,label,old_pool


if __name__ == "__main__":
    model = get_model()
    model.load_weights("models/best_3.h5")
    Env = GameEmulator();
    Env.add_agent(NNAgent(model = model))
    Env.add_agent(EasiestAgent())
    Env.add_agent(EasiestAgent())

    #model.add(Activation('softmax'))

    #keras.callbacks.EarlyStopping(monitor='val_loss', patience=0, verbose=0)

    prop = RMSprop( lr=0.00001, rho=0.9, epsilon=1e-6 )
    print( "start compilation" )
    model.compile(loss='mean_squared_error', optimizer=prop)

    random.seed(time.time())

    for EPOCH in range( 10000 ) :
        print( "DEALING EPOCH " + str( EPOCH ) );
        if EPOCH != 0 :
            print( "generating data" )
            time1 = time.time()
            data, label, data_pool = load_data(EPOCH)
            time2 = time.time()
            print("use time:", time2 - time1)
            data = np.array(data); label = np.array(label);
            print(data.shape)
            print( "start loading" )
            #model.load_weights('models/md' + str(EPOCH-1) + '.h5');
            print( "start fitting" )
            time1 = time.time()
            his = model.fit(data, label, batch_size=300,nb_epoch=1,shuffle=True,verbose=1,show_accuracy=False,validation_split=0.1)
            time2 = time.time()
            print("use time:", time2 - time1)
            print( "fitting ended" )
        if EPOCH % 10 == 0 :
            with open( "models/md.json", "w" ) as f:
                f.write( model.to_json() )
            model.save_weights( "models/md" + str(EPOCH) + ".h5" )

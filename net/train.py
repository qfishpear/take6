from __future__ import absolute_import
from __future__ import print_function
from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers.core import Dense, Dropout, Activation, Flatten
from keras.layers.advanced_activations import PReLU
from keras.layers.convolutional import Convolution2D, MaxPooling2D
from keras.optimizers import SGD, Adadelta, Adagrad, RMSprop
from keras.utils import np_utils, generic_utils
from six.moves import range
import keras
import numpy as np;
import os;
import sys;
sys.path.append( '..' )
from PIL import Image;
from pyrl.GameEmulator import GameEmulator
from agents.RandomAgent import RandomAgent
import pyrl.common
import random;
from pyrl.BaseAgent import BaseAgent 
from IPython import embed
class NNAgent(BaseAgent):
    def __init__(self):
	pass
    def policy(self, agentEnv):
	#if you only use execute_action, you do not need to implement this function
        pass

'''
gameEmulator = GameEmulator()
gameEmulator.add_agent(NNAgent())
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

Env = GameEmulator();
Env.add_agent(NNAgent());
Env.add_agent(RandomAgent())

def normalize(dat) :
    bits = [];
    status = [[0]*104]*3;
    for card in range(104) :
        status[dat['state']['card_status'][card]][card] = 1;
    bits = bits + status[0] + status[1] + status[2];
    stack = [[0]*104]*4;
    for i in range(4) :
        stack[i][dat['state']['card_stacks'][i][-1]] = 1;
    bits = bits + stack[0] + stack[1] + stack[2] + stack[3];
    height = [[0]*5]*4
    for i in range(4) :
        height[i][len(dat['state']['card_stacks'][i])-1] = 1;
    bits = bits + height[0] + height[1] + height[2] + height[3];
    for i in range(4) :
        bits.append( pyrl.common.count_nimmts(dat['state']['card_stacks'][i]) );
    choice = [0]*104;
    choice[dat['action']] = 1;
    bits = bits + choice;
    return bits;

data_pool = [];
def load_data(EPOCH):
    global Env, Gamma, Epsilon, data_pool, model;
    old_pool = data_pool;
    random.shuffle(old_pool);
    data = []; label = [];
    data_pool = [{'state':Env.generate_env()} for i in range( 50 )];
    data_pool += old_pool[:950];
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
            label.append(reward_t[ 0 ]);
            continue;
        toFeed = [];
        
        if random.uniform(0, 1) < Epsilon :
            choicelist = [ state_next['hand_cards'][0][random.randint(0, len(state_next['hand_cards'][0])-1)] ]
        else :
            choicelist = state_next['hand_cards'][0]
        
        count = 0;
        for choice in choicelist :
            next_dat = {'state':state_next, 'action':choice};
            if count < 2 :
                old_pool.append(next_dat);
                count += 1;
            toFeed.append(normalize(next_dat))
        
        toFeed = np.array(toFeed)
        nextQ = model.predict(toFeed);
        
        data.append(normalize(dat))
        label.append(reward_t[ 0 ] + Gamma * max(nextQ))
    return data,label,old_pool

model = Sequential()

model.add(Dense(1024, init='normal', input_dim = 856))
model.add(Activation('relu'))

model.add(Dense(1024, init='normal'))
model.add(Activation('relu'))

model.add(Dense(512, init='normal'))
model.add(Activation('relu'))

model.add(Dense(1, init='normal'))
#model.add(Activation('softmax'))                                                                                                                                     

#keras.callbacks.EarlyStopping(monitor='val_loss', patience=0, verbose=0)                                                                                            

prop = RMSprop( lr=0.00003, rho=0.9, epsilon=1e-6 )
print( "start compilation" )
model.compile(loss='mean_squared_error', optimizer=prop)

for EPOCH in range( 100 ) :
    print( "DEALING EPOCH " + str( EPOCH ) );
    if EPOCH != 0 :
        print( "generating data" )
        data, label, data_pool = load_data(EPOCH)
        data = np.array(data); label = np.array(label);
        print(data.shape)
        print( "start loading" )
        #model.load_weights('models/md' + str(EPOCH-1) + '.h5');
        print( "start fitting" )
        his = model.fit(data, label, batch_size=300,nb_epoch=1,shuffle=True,verbose=1,show_accuracy=False,validation_split=0.1)
        print( "fitting ended" )
    if EPOCH % 10 == 0 :
        with open( "models/md.json", "w" ) as f:
            f.write( model.to_json() )
        model.save_weights( "models/md" + str(EPOCH) + ".h5" )

#coding=utf-8
from keras.models import Sequential
from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers.core import Dense, Dropout, Activation, Flatten
from keras.layers.advanced_activations import PReLU, LeakyReLU
from keras.layers.convolutional import Convolution2D, MaxPooling2D
from keras.optimizers import SGD, Adadelta, Adagrad, RMSprop
from keras.utils import np_utils, generic_utils
from IPython import embed
import sys;
sys.path.append( '..' )
import pyrl.common
from pyrl.common import count_nimmts, num_nimmt, num_cards
import itertools
import random
import numpy as np
# def normalize(dat) :
#     bits = [];
#     status = [[0 for col in range(104)] for row in range(3)]
#     for card in range(104) :
#         status[dat['state']['card_status'][card]][card] = 1;
#     bits = bits + status[0] + status[1] + status[2];
#     stack = [[0 for col in range(104)] for row in range(4)]
#     for i in range(4) :
#         stack[i][dat['state']['card_stacks'][i][-1]] = 1;
#     bits = bits + stack[0] + stack[1] + stack[2] + stack[3];
#     height = [[0 for col in range(5)] for row in range(4)]
#     for i in range(4) :
#         height[i][len(dat['state']['card_stacks'][i])-1] = 1;
#     bits = bits + height[0] + height[1] + height[2] + height[3];
#     for i in range(4) :
#         bits.append( pyrl.common.count_nimmts(dat['state']['card_stacks'][i]) );
#     choice = [0 for col in range(104)]
#     choice[dat['action']] = 1;
#     bits = bits + choice;
#     return bits;

# def get_model():
#     model = Sequential()
#
#     model.add(Dense(1024, init='normal', input_dim = 856))
#     model.add(Activation('relu'))
#
#     model.add(Dense(1024, init='normal'))
#     model.add(Activation('relu'))
#
#     model.add(Dense(512, init='normal'))
#     model.add(Activation('relu'))
#
#     model.add(Dense(1, init='normal'))
#     return model

random.seed(2333)
num_base_ind = 29
ind_2_100 = random.sample(list(itertools.permutations(range(num_base_ind),2)),50)
ind_3_100 = random.sample(list(itertools.permutations(range(num_base_ind),3)),50)
ind_3_100 = random.sample(list(itertools.permutations(range(num_base_ind),4)),50)

def normalize(dat) :
    bits = [];
    action = dat['action']
    card_stacks = dat['state']['card_stacks']
    card_status = dat['state']['card_status']
    agent_id = dat['state']['agent_id']
    hand_card = dat['state']['hand_cards'][agent_id]
    for i in range(16):
        l_and = True
        l_or = False
        for j in range(4):
            if ((1<<j)&i > 0):
                l_and = l_and and (action < card_stacks[j][-1])
                l_or = l_or or (action < card_stacks[j][-1])
        bits.append(int(l_and))
        bits.append(int(l_or))
    bits.append(1.0 * action / pyrl.common.num_agent_init_card)
    for i in range(4):
        bits.append(1.0 * card_stacks[i][-1] / pyrl.common.num_agent_init_card)
    tmp = []
    for i in range(4):
        for j in range(5):
            tmp.append(j == len(card_stacks[i]))
    target_stack = -1
    for j in range(4):
        if action > card_stacks[j][-1]:
            if target_stack == -1 or card_stacks[j][-1] > card_stacks[target_stack][-1]:
                target_stack = j
    for i in range(4):
        tmp.append(action > card_stacks[i][-1])
        tmp.append(target_stack == i)
    tmp.append(target_stack == -1)

    for i in range(num_base_ind):
        bits.append(int(tmp[i]))
    for i in range(20):
        for j in range(20, num_base_ind):
            bits.append(int(tmp[i] and tmp[j]))
    i2 = ind_2_100
    i3 = ind_3_100
    i4 = ind_3_100
    for i in range(50):
        bits.append(int(tmp[i2[i][0]] and tmp[i2[i][1]]))
        bits.append(int(tmp[i3[i][0]] and tmp[i3[i][1]] and tmp[i3[i][2]]))
        bits.append(int(tmp[i4[i][0]] and tmp[i4[i][1]] and tmp[i4[i][2]] and tmp[i4[i][3]]))

    status_bucket = [[0 for col in range(13)] for row in range(3)]
    #every 13 in a bucket
    for i in range(num_cards):
        status_bucket[card_status[i]][i/8] += 1
    bits = bits + status_bucket[0] + status_bucket[1] + status_bucket[2]

    if target_stack == -1:
        bits.append(min([count_nimmts(s) for s in card_stacks]))
        bits.append(min([count_nimmts(s) for s in card_stacks]))
        bits.append(min([count_nimmts(s) for s in card_stacks]))
    else:
        bits.append(count_nimmts(card_stacks[target_stack]))
        bits.append(count_nimmts(card_stacks[target_stack]) * int(len(card_stacks[target_stack]) >= 5))
        bits.append(count_nimmts(card_stacks[target_stack]) * int(len(card_stacks[target_stack]) >= 4))

    bits.append(count_nimmts(hand_card))
    punish_bucket = [[0 for col in range(7)] for row in range(3)]
    for i in range(num_cards):
        punish_bucket[card_status[i]][num_nimmt(i)-1] += 1
    bits = bits + punish_bucket[0] + punish_bucket[1] + punish_bucket[2]
    #in all there are 459 features
    return bits


def get_model():
    model = Sequential()

    model.add(Dense(1024, init='normal', input_dim = 460))
    model.add(LeakyReLU(0.3))
    model.add(Dropout(0.5))

    model.add(Dense(1024, init='normal'))
    model.add(LeakyReLU(0.3))
    model.add(Dropout(0.5))

    model.add(Dense(512, init='normal'))
    model.add(LeakyReLU(0.3))
    model.add(Dropout(0.5))

    model.add(Dense(1, init='normal'))
    return model

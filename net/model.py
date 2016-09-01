#coding=utf-8
from keras.models import Sequential
from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers.core import Dense, Dropout, Activation, Flatten
from keras.layers.advanced_activations import PReLU, LeakyReLU
from keras.layers.convolutional import Convolution2D, MaxPooling2D
from keras.initializations import normal
from keras.optimizers import SGD, Adadelta, Adagrad, RMSprop
from keras.utils import np_utils, generic_utils
from keras.regularizers import activity_l2, l2
from IPython import embed
import sys;
sys.path.append( '..' )
import pyrl.common
from pyrl.common import count_nimmts, num_nimmt, num_cards
import itertools
import random
import numpy as np
import pdb

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
ind_2 = random.sample(list(itertools.permutations(range(num_base_ind),2)),50)
ind_3 = random.sample(list(itertools.permutations(range(num_base_ind),3)),50)
ind_3 = random.sample(list(itertools.permutations(range(num_base_ind),4)),50)
def toint(t):
    return int(t)*2-1

def normalize(dat):
    return normalize_dr(dat)

def get_model():
    return get_model_dr()

def normalize_dr(dat):
    card = dat['action']
    card_stacks = np.array(dat['state']['card_stacks'])
    #card_status = dat['state']['card_status']
    agent_id = dat['state']['agent_id']
    hand_card = dat['state']['hand_cards'][agent_id]
    tmp = []
    tmp.append(card*1.0/num_cards)
    tmp.append(min(count_nimmts(s) for s in card_stacks))
    choices = card_stacks[np.array([s[-1] < card for s in card_stacks])]
    next = num_cards
    for s in card_stacks:
        if s[-1] > card:
            next = min(next, s[-1])
    if card != hand_card[-1]:
        next = min(next, hand_card.index(card)+1)
    tmp.append(next*1.0/num_cards)
    if len(choices) > 0:
        stack = choices[np.argmax([s[-1] for s in choices])]
        tmp.append(stack[-1])
        tmp.append(count_nimmts(stack))
        tmp.append(count_nimmts([card,]))
        tmp.append(toint(len(stack) == 5))
        tmp.append(toint(len(stack) == 4))
        tmp.append(toint(len(stack) <= 3))
        tmp.append((6-len(stack))/5)
    else:
        tmp += [0,]*7
    l = len(tmp)
    bits = [] + tmp
    bits += [tmp[i] * tmp[j] for i in range(l) \
             for j in range(i, l)]
    bits += [tmp[i] * tmp[j] * tmp[k] for i in range(l) \
             for j in range(i, l) for k in range(j, l)]
    bits += [tmp[i] * tmp[j] * tmp[k] * tmp[p] for i in range(l) \
            for j in range(i, l) for k in range(j, l) for p in range(k, l)]
    return bits

def my_init(shape, name=None):
    return normal(shape, scale=0.0001, name=name)

def get_model_dr():
    model = Sequential()

    model.add(Dense(200, init=my_init, input_dim = 1000))#, W_regularizer=l2(0.000001)))
    model.add(Activation("relu"))

    model.add(Dense(1, init=my_init))#, W_regularizer=l2(0.000001)))
    return model


def normalize_fish(dat):
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
    i2 = ind_2
    i3 = ind_3
    i4 = ind_3
    for i in range(50):
        bits.append(toint(tmp[i2[i][0]] and tmp[i2[i][1]]))
        bits.append(toint(tmp[i3[i][0]] and tmp[i3[i][1]] and tmp[i3[i][2]]))
        bits.append(toint(tmp[i4[i][0]] and tmp[i4[i][1]] and tmp[i4[i][2]] and tmp[i4[i][3]]))

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


def get_model_fish():
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

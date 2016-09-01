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

from RandomAgent import RandomAgent

agent = RandomAgent()
agent_id = input()
num_round = input()
hand_card = raw_input().split(" ")
num_player = input()
num_cards = input()
card_stacks = [raw_input().split(" ").remove(0) for i in range(4)]



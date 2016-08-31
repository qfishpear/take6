from IPython import embed
num_cards = 104
list_capacity = 6
num_card_stack = 4
num_agents = 2
num_agent_init_card = 10;

def num_nimmt(id, mode = 1):
    idd = id + mode
    cnt = 0
    if idd % 11 == 0:
        cnt += 5
    if idd % 5  == 0:
        cnt += 2
    if idd % 10 == 0:
        cnt += 1
    if cnt == 0:
        cnt = 1
    return cnt

def count_nimmts(card_list, mode = 1):
    cnt = 0
    for card in card_list:
        cnt += num_nimmt(card, mode)
    return cnt

def add_card(card_list, card):
    #input: a card list and a card to add
    #output: The new card list and The number of nimmts this action be punished
    if len(card_list) == 0:
        card_list.append(card)
        return card_list, 0
    if len(card_list) == list_capacity - 1 or card < card_list[-1]:
        return [card,], count_nimmts(card_list)
    card_list.append(card)
    return card_list, 0


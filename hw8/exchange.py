import math

visit_history = []
window = 0
last_fault = 0
current = -1
mem = {}

def init(phy_memory_size, window_size):
    global mem, window
    window = window_size
    for i in range(0, phy_memory_size):
        mem[i] = 0

#缺页率
def get_memory(addr):
    global mem, window, visit_history, current, last_fault
    print "now addr:", addr
    visit_history.append(addr)
    current += 1
    print "current ", current, " last ", last_fault
    if mem[addr]:
        print "get addr ok"
    else:
        print "page fault"
        if current - last_fault > window:
            for i in range(0, len(visit_history)-window-1):
                mem[visit_history[i]] = 0

            for i in range(-1, -window-2):
                mem[visit_history[i]] = 1

            visit_history = visit_history[-window-1:]
            print "current history after clean: ", visit_history
        mem[addr] = 1
        last_fault = current

#工作集
def get_memory_workplace(addr):
    global mem, window, visit_history, current
    print "now addr:", addr

    if mem[addr] == 1:
        visit_history.append(addr)
        print "visit ok"
    else:
        visit_history.append(addr)
        mem[addr] = 1
        print "page fault"

    if len(visit_history) > window:
        mem[visit_history[0]] = 0
        visit_history.pop(0)
        for i in range(0, len(visit_history)):
            mem[visit_history[i]] = 1

    print "now workplace:", visit_history

init(10, 4)
get_memory_workplace(1)
get_memory_workplace(2)
get_memory_workplace(3)
get_memory_workplace(1)

get_memory_workplace(4)
get_memory_workplace(4)



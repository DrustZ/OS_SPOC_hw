#coding=utf-8
#!/usr/bin/env python

import threading  
import time  

condition = threading.Condition()  
products = 0  

class Customer(threading.Thread):  
    def __init__(self):  
        threading.Thread.__init__(self)  

    def run(self):  
        global condition, products  
        while True:  
            if condition.acquire():  
                # if products < 10:  
                products += 1;  
                print "Customer(%s):wait for call, now numbers in queue:%s" %(self.name, products)  
                condition.notify()  
                # else:  
                #     print "Customer(%s):already 10, stop deliver, now products:%s" %(self.name, products)  
                #     condition.wait();  
                condition.release()  
                time.sleep(1)  

class Clerk(threading.Thread):  
    def __init__(self):  
        threading.Thread.__init__(self)  

    def run(self):  
        global condition, products  
        while True:  
            if condition.acquire():  
                if products > 0:  
                    products -= 1  
                    print "Clerk(%s):call one number, now numbers in queue:%s" %(self.name, products)  
                    condition.notify()  
                else:  
                    print "Clerk(%s):no number in queue, stop call, products:%s" %(self.name, products)  
                    condition.wait();  
                condition.release()  
                time.sleep(1)  

if __name__ == "__main__":  
    n = 10;
    for p in range(0, 10):  
        p = Customer()  
        p.start()  

    for c in range(0, n):  
        c = Clerk()  
        c.start() 
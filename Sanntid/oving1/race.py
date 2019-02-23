from threading import Thread

i = 0;



def thread1():
	global i
	for x in range(1000000):
		i+= 1

def thread2():
	global i
	for x in range(1000000):
		i-= 1



def main():
	myThread1 = Thread(target = thread1, args = (),)
	myThread2 = Thread(target = thread2, args = (),)
	myThread1.start()
	myThread2.start()
	myThread1.join()
	myThread2.join()
	print(i)
	
main()
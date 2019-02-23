import threading

i = 0;
lock = threading.Lock()


def thread1():
	global i
	for x in range(1000000):
		with lock:
			i += 1

def thread2():
	global i
	for x in range(1000000):
		with lock:
			i -=1



def main():
	myThread1 = threading.Thread(target = thread1, args = (),)
	myThread2 = threading.Thread(target = thread2, args = (),)
	myThread1.start()
	myThread2.start()
	myThread1.join()
	myThread2.join()
	print(i)
	
main()

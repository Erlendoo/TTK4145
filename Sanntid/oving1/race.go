package main

import (
    "fmt"
    "runtime"
    "time"
)

var i int = 0

func thread1(){
	for x := 0; x < 1000000; x++{
		i++
	}	
}

func thread2(){
	for x := 0; x < 1000000; x++{
		i--
	}
}

func main(){
	runtime.GOMAXPROCS(runtime.NumCPU())
	go thread1()
	go thread2()
	time.Sleep(100*time.Microsecond)
	fmt.Println(i)
}

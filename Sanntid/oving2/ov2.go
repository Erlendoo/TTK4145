
package main

import (
    "fmt"
    "runtime"
)

var i int = 0



func thread1(access chan int, done chan bool){
	for x := 0; x < 1000000; x++{
		access <- 1	
		i++
		<- access	
	
	}	
	done <- true
}

func thread2(access chan int, done chan bool){
	for x := 0; x < 1000001; x++{
		access <- 1
		i--
		<- access
	}
	done <- true
}

func main(){
	runtime.GOMAXPROCS(runtime.NumCPU())
	
	access := make(chan int, 1)
	done := make(chan bool)
	//access <- 1
	go thread1(access, done)
	go thread2(access, done)
	<-done
	<-done
	fmt.Println(i)
}

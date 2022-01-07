package main

import (
	"fmt"
	"os"
	"time"
)

func main() {
	fmt.Println("Smoothing...")
	simulationRuntimeSeconds := time.Duration(10) * time.Second
	tickIntervalMilliseconds := time.Duration(250) * time.Millisecond

	go func() {
		time.Sleep(simulationRuntimeSeconds)
		fmt.Println("Exiting")
		os.Exit(0)
	}()

	sampleThrottleInput := []int{0, 25, 0, 50, 0, 100, 0}
	ticksPerThrottleInput := 5

	tick := 0
	for {
		currentThrottleInputIndex := tick / ticksPerThrottleInput
		if currentThrottleInputIndex >= len(sampleThrottleInput)-1 {
			currentThrottleInputIndex = len(sampleThrottleInput) - 1
		}

		currentThrottlePercent := sampleThrottleInput[currentThrottleInputIndex]
		fmt.Printf("Current Throttle: %v%%", currentThrottlePercent)
		fmt.Println()

		time.Sleep(tickIntervalMilliseconds)
		tick++
	}
}
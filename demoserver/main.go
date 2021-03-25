package main

import (
	"fmt"
	"os"

	"github.com/bladedancer/envoy-wasm-stopitertest/demoserver/cmd"
)

func main() {
	if err := cmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}

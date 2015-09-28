// James Jessen
// 10918967
// CptS 464 Proj #3

// Run followers and then the leader

package main

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"net"
	"time"
)

// The name of this general
// Brutus
// Pompus
// Operachorus
const myName = "Brutus"
const leader = false

// Troop Levels
var NumCatapult int
var NumArcher int
var NumCavalry int
var NumSpearmen int
var NumInfantry int

func main() {
	Init()

	for {
		// Listen for leader requesting report
		Listen()

		// Follow orders for transferIn/Out

		time.Sleep(1000 * time.Millisecond)
	}
}

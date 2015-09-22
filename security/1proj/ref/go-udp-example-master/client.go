package main

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"net"
	"time"
)

type MyMessage struct {
	SenderName string

	CatapultCount int
	ArcherCount   int
	CavalryCount  int
	SpearmenCount int
	InfantryCount int

	Timestamp [4]int

	//SnapshotToken int
}

func CheckError(err error) {
	if err != nil {
		fmt.Println("Error: ", err)
	}
}

func main() {
	rand.Seed(time.Now().UnixNano())

	target, err := net.ResolveUDPAddr("udp", ":9000")
	CheckError(err)

	// pass nil for the 'local address'
	con, err := net.DialUDP("udp", nil, target)
	defer con.Close()
	CheckError(err)

	// Caesar
	// Brutus
	// Pompus
	// Operachorus

	i := 0
	for {
		message := &MyMessage{
			SenderName: "Brutus",

			CatapultCount: rand.Intn(99) + 1,
			ArcherCount:   rand.Intn(99) + 1,
			CavalryCount:  rand.Intn(99) + 1,
			SpearmenCount: rand.Intn(99) + 1,
			InfantryCount: rand.Intn(99) + 1,

			Timestamp: [4]int{0, 0, 0, i},
		}

		data, err := json.Marshal(message)
		CheckError(err)

		n, err := con.Write(data)
		CheckError(err)

		fmt.Printf("(i = %d) wrote %d bytes!\n", i, n)
		i++

		time.Sleep(1000 * time.Millisecond)
	}
}

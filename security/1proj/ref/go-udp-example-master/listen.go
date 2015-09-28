package main

import (
	"encoding/json"
	"fmt"
	"net"
)

type MyMessage struct {
	SenderName string

	CatapultCount int
	ArcherCount   int
	CavalryCount  int
	SpearmenCount int
	InfantryCount int

	Timestamp [4]int
}

func CheckError(err error) {
	if err != nil {
		fmt.Println("Error: ", err)
	}
}

func main() {
	// to do udp, you could do it just like the in class demo,
	// but to have more control over the udp connection, i'm going to
	// show a more udp specific way to do things
	uaddr, err := net.ResolveUDPAddr("udp", ":9000")
	CheckError(err)

	list, err := net.ListenUDP("udp", uaddr)
	CheckError(err)

	// loop and read packets
	for {
		// make a buffer of size 4096 to read data into
		mesbuf := make([]byte, 4096)

		nread, from, err := list.ReadFromUDP(mesbuf)
		CheckError(err)

		// truncate buf to size of read message
		mesbuf = mesbuf[:nread]

		fmt.Printf("From %s\n", from.String())
		fmt.Printf("Size %d\n", nread)

		// unmarshal received data into a message struct
		var message MyMessage
		err = json.Unmarshal(mesbuf, &message)
		CheckError(err)

		fmt.Printf("\n---------------\n")
		fmt.Printf(" %s\n", message.SenderName)
		fmt.Printf("---------------\n")
		fmt.Printf(" Catapult: %d\n", message.CatapultCount)
		fmt.Printf(" Archer:   %d\n", message.ArcherCount)
		fmt.Printf(" Cavalry:  %d\n", message.CavalryCount)
		fmt.Printf(" Spearmen: %d\n", message.SpearmenCount)
		fmt.Printf(" Infantry: %d\n", message.InfantryCount)
		fmt.Printf("===============\n")
		fmt.Println(message.Timestamp)
	}
}

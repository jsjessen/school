package main

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"net"
	"time"
)

var protocol string = "udp"
var read_port string = ":9000"
var write_port string = ":9001"

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

func DispMsg(message MyMessage) {
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

// Caesar
// Brutus
// Pompus
// Operachorus

func main() {
	// Seed for random generated initial troop levels
	rand.Seed(time.Now().UnixNano())

	// ---------------- READ Setup -------------------
	uaddr, err := net.ResolveUDPAddr(protocol, read_port)
	CheckError(err)

	list, err := net.ListenUDP(protocol, uaddr)
	CheckError(err)

	// ---------------- WRITE Setup -------------------
	target, err := net.ResolveUDPAddr(protocol, write_port)
	CheckError(err)

	// pass nil for the 'local address'
	con, err := net.DialUDP(protocol, nil, target)
	defer con.Close()
	CheckError(err)

	i := 0
	for {
		// ---------------- READ -------------------
		// make a buffer of size 4096 to read data into
		mesbuf := make([]byte, 4096)

		nread, from, err := list.ReadFromUDP(mesbuf)
		CheckError(err)

		// truncate buf to size of read message
		mesbuf = mesbuf[:nread]

		fmt.Printf("From %s\n", from.String())
		fmt.Printf("Size %d\n", nread)

		// unmarshal received data into a message struct
		var in_message MyMessage
		err = json.Unmarshal(mesbuf, &in_message)
		CheckError(err)

		DispMsg(in_message)

		// ---------------- WRITE -------------------
		out_message := &MyMessage{
			SenderName: "Brutus",

			CatapultCount: rand.Intn(99) + 1,
			ArcherCount:   rand.Intn(99) + 1,
			CavalryCount:  rand.Intn(99) + 1,
			SpearmenCount: rand.Intn(99) + 1,
			InfantryCount: rand.Intn(99) + 1,

			Timestamp: [4]int{0, 0, 0, i},
		}

		data, err := json.Marshal(out_message)
		CheckError(err)

		n, err := con.Write(data)
		CheckError(err)

		fmt.Printf("(%d) wrote %d bytes!\n", i, n)
		i++

		time.Sleep(1000 * time.Millisecond)
	}
}

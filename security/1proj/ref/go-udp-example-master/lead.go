// James Jessen
// 10918967
// CptS 464 Proj #3

// Run followers and then the leader

// To extend the scenario just have them attack after synchronizing & equalizing,
// then have a random number die, they retreat, and then try again!

package main

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"net"
	"time"
)

// The name of this general
const myName = "Caesar"
const leader = true

// Generals
const Generals = map[string]string{
	"Caesar": ":9000",
	"Brutus": ":9002",
	"Pompus", ":9004",
	"Operachorus": ":9006",
}

// Orders
const Orders = map[string]int{
	"report":      0,
	"readReport":  1,
	"transferOut": 2,
	"transferIn":  3,
	"attack":      4,
	"retreat":     5,
}

// Communication
const protocol = "udp"

// Troop Levels
var NumCatapult int
var NumArcher int
var NumCavalry int
var NumSpearmen int
var NumInfantry int

var BrutusReport MyMessage
var PompusReport MyMessage
var OperachorusReport MyMessage

// Troop Levels
var TotalNumCatapult int
var TotalNumArcher int
var TotalNumCavalry int
var TotalNumSpearmen int
var TotalNumInfantry int

// Synchronization
var VectorClock [4]int

// Message
type MyMessage struct {
	Sender string
	Order  string
	Target string

	CatapultCount int
	ArcherCount   int
	CavalryCount  int
	SpearmenCount int
	InfantryCount int

	Timestamp [4]int

	SnapshotToken int
}

func CheckError(err error) {
	if err != nil {
		fmt.Println("Error: ", err)
	}
}

func DispMsg(message MyMessage) {
	fmt.Printf("---------------\n")
	fmt.Printf(" %s -> %s\n", message.Sender, myName)
	fmt.Printf("---------------\n")
	fmt.Printf(" Order: %s\n", message.Order)
	fmt.Printf(" Target: %s\n", message.Target)
	fmt.Printf("---------------\n")
	fmt.Printf(" Catapult: %d\n", message.CatapultCount)
	fmt.Printf(" Archer:   %d\n", message.ArcherCount)
	fmt.Printf(" Cavalry:  %d\n", message.CavalryCount)
	fmt.Printf(" Spearmen: %d\n", message.SpearmenCount)
	fmt.Printf(" Infantry: %d\n", message.InfantryCount)
	fmt.Printf("===============\n")
	fmt.Println(message.Timestamp)
}

func GetStatusMsg() (message *MyMessage) {
	message = &MyMessage{
		Sender: myName,
		Order:  "readReport",

		CatapultCount: NumCatapult,
		ArcherCount:   NumArcher,
		CavalryCount:  NumCavalry,
		SpearmenCount: NumSpearmen,
		InfantryCount: NumCatapult,

		Timestamp: VectorClock,
	}
	return
}

// Not fully implemented
func FollowOrder(order MyMessage) {
	switch Orders[order] {
	case Orders["report"]:
		report := GetStatusMsg()
		SendOrder(sender, "readReport", sender)
	case Orders["readReport"]:
	case Orders["transferOut"]:
	case Orders["transferIn"]:
	case Orders["attack"]:
	}
}

func Listen(sender string) {
	// Setup
	if leader {
		port := Generals[sender]
	} else {
		port := Generals[sender] + 1
	}
	uaddr, err := net.ResolveUDPAddr(protocol, port)
	CheckError(err)

	conn, err := net.ListenUDP(protocol, uaddr)
	CheckError(err)
	defer conn.Close()

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

	DispMsg(message)
	FollowOrder(message)
}

func SendOrder(receiver string, order string, target string) {
	// Setup
	if leader {
		port := Generals[sender] + 1
	} else {
		port := Generals[sender]
	}
	target, err := net.ResolveUDPAddr(protocol, port)
	CheckError(err)

	// pass nil for the 'local address'
	conn, err = net.DialUDP(protocol, nil, target)
	CheckError(err)
	defer conn.Close()

	data, err := json.Marshal(GetStatusMsg())
	CheckError(err)

	n, err := con.Write(data)
	CheckError(err)

	fmt.Printf("(%d) wrote %d bytes!\n", i, n)
	i++
}

func Redistribute() {

}

func Init() {
	// Seed for randomly generated initial troop levels
	rand.Seed(time.Now().UnixNano())

	// Random [1-100]
	NumCatapult = rand.Intn(99) + 1
	NumArcher = rand.Intn(99) + 1
	NumCavalry = rand.Intn(99) + 1
	NumSpearmen = rand.Intn(99) + 1
	NumInfantry = rand.Intn(99) + 1

	VectorClock = [4]int{0, 0, 0, 0}
}

func main() {
	Init()

	for {
		// Order generals report in
		SendOrder("Brutus", "report", "Brutus")
		SendOrder("Pompus", "report", "Pompus")
		SendOrder("Operachorus", "report", "Operachorus")

		// Get troop level data
		Listen()
		Listen()
		Listen()

		// Compute ideal troop levels for each unit type
		// Note: A total not divisible by 4 will leave 1 to 3 generals
		// 1 above the ideal number
		IdealNumCatapult := TotalNumCatapult / 4
		IdealNumArcher := TotalNumArcher / 4
		IdealNumCavalry := TotalNumCavalry / 4
		IdealNumSpearmen := TotalNumSpearmen / 4
		IdealNumInfantry := TotalNumInfantry / 4

		// Order troop transfers between generals

		//SendOrder(general, transferOut, ???)
		//SendOrder(general, transferOut, ???)
		//SendOrder(general, transferOut, ???)

		// SNAPSHOT

		time.Sleep(1000 * time.Millisecond)
	}
}

Joseph Dobrowolski
Ryan Printz

Currently just sends a file between two connected clients.
Commands:
#files 				- Returns a list of files of the other connected clients
#get <filename> <sessionID> 	- Gets the named file from the particular client
#headers			- Toggles packet headers (sending has less info than receiving at the moment)
#connections			- List of connections on the client and whether they're alive
#info <connectionID> 		- Shows specific info about the connection with the specific ID
#decorate <connectionID> [DROP|LATENCY] [highModifier] [lowModifier] 
				- Decorates a connection with packet dropping or latency (currently not working correctly)
quit				- Exits the program

Currently only sends a file in a vacuum. Acks, resending, priority, simulating latency/dropping of packets, flow control, RTT
and keep alive are not fully functional. Packet headers can be toggled on and off but the only "correct" part of them is the 
sequence number. Using the quit command sends the disconnect message correctly to the server, but pressing the X does not.
// IP2DMX-16bit Floor Step LED Light controller
// by Chris Janssen @ Planetarium Wausau School District 2021 (cjanssen.usa@gmail.com)
// Complete project information is at https://github.com/PlanetariumWSD/IP2DMX-16bit

// IMPORTANT ++++++++++++++++++++++++++++++++++++
// Location of this Digistar js file is $Content/User. This is the default js file path!
//	js play leds.js | go('stairs 75 red 3')
// VS  
// 	js play $Content/User/facility/folder/scripts/leds.js | go('stairs 75 red 3')

// HOW TO USE THIS FILE ===============================
// Some error cehcking is done via the Messages tab, if nothing happens, check there.
// METHOD #1 Fade colors to select fixtures
// js play leds.js | go('definedFixture  %brightness  definedColor  duration(sec) LoopMode(optional)')
// js play leds.js | go('row2 75 red 3')
// js play leds.js | go('all 100 black 0 normal')
// js play leds.js | go('stairs 75 red 3 loop'|row1 100 green 4.5 repeat)


// METHOD #2 Raw JSON Command mode (not recomended)
// See full detailed instructions as project web address above.
// js play leds.js | send('[{"n":1,"v":1001,"d":0},{"n":2,"v":1002,"d":0},{"n":3,"v":1003,"d":0},{"n":4,"v":1004,"d":0}]')
// JSON array structure is [{"n": (light fixture 0-19), "v": (0-65535), "d": duration(sec), "r": mode(normal 0, repeat 1, loop 2), "l": Interpertation}]

	function setup(){ // Note: no "var" in front of arrays, therfore become global scope for use in subfunctions.
	// Define color names and thier RGBW % recipies. For brightness equality, try to get a TOTAL of 100%.
	color   = [	{"name":"black",	"R": 0,		"G": 0,		"B": 0, 	"W": 0 		},
				{"name":"white", 	"R": 0,		"G": 0, 	"B": 0, 	"W": 100	},
				{"name":"red", 		"R": 100,	"G": 0, 	"B": 0, 	"W": 0 		},
				{"name":"pink", 	"R": 60, 	"G": 0, 	"B": 10,	"W": 20 	},
				{"name":"orange", 	"R": 68, 	"G": 32, 	"B": 0, 	"W": 0 		},
				{"name":"yellow",	"R": 60, 	"G": 40, 	"B": 0, 	"W": 0 		},
				{"name":"green", 	"R": 0, 	"G": 100,	"B": 0, 	"W": 0 		},
				{"name":"blue", 	"R": 0, 	"G": 0, 	"B": 100,	"W": 0 		},
				{"name":"sky",		"R": 36, 	"G": 6, 	"B": 66, 	"W": 0  	},
				{"name":"indigo", 	"R": 27, 	"G": 4.5, 	"B": 50, 	"W": 0 		},
				{"name":"violet", 	"R": 50, 	"G": 0, 	"B": 50, 	"W": 0 		},
				{"name":"purple", 	"R": 50, 	"G": 0, 	"B": 50, 	"W": 0 		},
				{"name":"R",	 	"R": 100										},
				{"name":"G", 		"G": 100										},
				{"name":"B", 		"B": 100										},
				{"name":"W", 		"W": 100								 		},
				{"name":"maximum", 	"R": 100, 	"G": 100, 	"B": 100, 	"W": 100 	}
			  ];
	
	// Define light fixtures-> name, [for each color RGBW] > ( node #, low cut off value (16-bit) )
	fixture = [	{"name":"SW", "R": 1, "G": 2, "B": 3, "W": 4, "Rmin": 1001, "Gmin": 1002, "Bmin": 1003, "Wmin": 1004},
				{"name":"NW", "R": 6, "G": 7, "B": 8, "W": 9, "Rmin": 1000, "Gmin": 1000, "Bmin": 1000, "Wmin": 1000},
				{"name":"NE", "R": 11, "G": 12, "B": 13, "W": 14, "Rmin": 1000, "Gmin": 1000, "Bmin": 1000, "Wmin": 1000},
				{"name":"SE", "R": 16, "G": 17, "B": 18, "W": 19, "Rmin": 0, "Gmin": 0, "Bmin": 0, "Wmin": 1000},
				{"name":"ALL"}	// This "all" fixture must be last and not changed nor removed!
			  ];
			  
	// Network IP address of the IP2DMX16 box.
	clientIP = "192.168.2.246:8888";
	
	};

	// Sample command js play leds | go('row1 60 R 5 normal|row2 75 blue 8.5 loop')
	function go(string){ 
		setup();																		// Or you could use (in a sperate file) include('.\\ledSetup.js');
		var parsedMultiCmdArray = string.split('|');									// Break multiple commands into solo commands for processing with Delimiter |
		var packetNL = [];																// Total containers for full collection of nodes commands for final UDP packet.
		var packetNVD = [];
		for (let cmdCount = 0; cmdCount < parsedMultiCmdArray.length; cmdCount++){		// Retreive and create the correct 16 bit base color with correct brightness.
			var parsedCmdArray = parsedMultiCmdArray[cmdCount].split(' ');				// Split sub command (parsed by " ") into array for processing.
			var status = 0;
			// CMD ARGUMENT #1 checker  ( is array[0] a valid fixture name ? )-------------------------
			for ( let i = 0; status == 0 && i < fixture.length; i++ ){
				if (parsedCmdArray[0] === fixture[i].name){ 
					status = 1;
					}
				}
			if (status == 0){
				print('ERROR IN ARG #1> No such light fixture name listed in setup');
				return;
				}
			// CMD ARGUMENT #2 checker  ( is array[1] a number 0 to 100 % ? )-------------------------
			if (status == 1 && parsedCmdArray[1] >= 0 && parsedCmdArray[1] <= 100){ 
				status = 2;
				}
			if (status == 1){
				print('ERROR IN ARG #2> Brightness value must be 0-100 %');
				return;
				}	
			// CMD ARGUMENT #3 checker  ( is array[2] a valid color name ? )-------------------------
			for ( let i = 0; status == 2 && i < color.length; i++ ){
				if (parsedCmdArray[2] === color[i].name){ 
					status = 3;
					}
				}
			if (status == 2){
				print('ERROR IN ARG #3> No such color listed in setup');
				return status;
				}	
			// CMD ARGUMENT #4 checker  ( is array[3] a number 0 to 6000 seconds or 100 minutes )-------------------------
			if (status == 3 && parsedCmdArray[3] >= 0 && parsedCmdArray[3] <= 6000){ 
				status = 4;
				}
			if (status == 3){
				print('ERROR IN ARG #4> Invalid time (0-6000 seconds)');
				return;
				}		
			if (parsedCmdArray[4] != null){
				if (parsedCmdArray[4] == 'normal'){ status = 5}
				else if (parsedCmdArray[4] == 'repeat'){ status = 6}
				else if (parsedCmdArray[4] == 'loop'){ status = 7}
				else {
					print('ERROR IN ARG #5> Mode type must be: normal, repeat or loop');
					return;
					};
				}	
				
			if (status >= 4){															// Every part of current cmd checked as OK!
				var nvd = {"n": 0, "v": 0, "d": 0};										// Small container for node based commands
				var chans = ['R', 'G', 'B', 'W'];										// Make 4 variables of name R, G, B, W and set them to 0									
				for (let i = 0; i < 4; i++){
					this[chans[i]] = 0 
					}
							
				nvd.d = parseInt(parsedCmdArray[3] * 10);								// Calculate Duration! User says 'seconds' ip2dmx box reads deciseconds.
							
				for (let i = 0; i < color.length; i++){									// Retreive and create the correct 16 bit base color with correct brightness
					if (parsedCmdArray[2] == color[i].name){ 							// Max 16 bit value is 65535 and the recipe stores percentage.
						for (var e of chans){ 
							this[e] = parseInt(6.5535 * color[i][e] * parsedCmdArray[1])// R = parseInt(6.5535 * color[i].R * parsedCmdArray[1]);
							}
						}
					}
				
				if (parsedCmdArray[0] === fixture[(fixture.length -1)].name){			// do "all" the fixtures! Last name in fixture setup must be "all".
					if (parsedMultiCmdArray.length > 1 ) {
						print("ERROR> An (all) command must be SOLO!");
						return;
						}
					for ( let f = 0; f < fixture.length-1; f++ ){						// Execute for each fixture 0-3
						for (var e of chans){
							if (this[e] < fixture[f][e+'min']){							// if (R < fixture[i].Rmin){R = fixture[i].Rmin};
								this[e] = fixture[f][e+'min'];
								};
							nvd.n = fixture[f][e];										// set the cmd array value n, nvd.n = fixture[1].R;
							nvd.v = this[e];											// nvd.v = R;
							if (nvd.v < 65535){											// If there's no real 16bit value in recipe, then don't send node cmd
								packetNVD.push(JSON.stringify(nvd));
								if (status >= 5){
									var nl = {"n": 0, "l": 0};		
									nl.n = fixture[f][e];											// set the cmd array value n
									if (parsedCmdArray[4] == 'normal'){ nl.l = 0}
									else if (parsedCmdArray[4] == 'repeat'){ nl.l = 1}
									else if (parsedCmdArray[4] == 'loop'){ nl.l = 2}
									packetNL.push(JSON.stringify(nl));								// Push single cmd into the total buffer
									}
								}	
							}
						}
					}						
				else {																	// create node commands for correct single fixture.
					for ( let i = 0; i < fixture.length-1; i++ ){						// Browse all fixtures except the last one "all"
						if (parsedCmdArray[0] === fixture[i].name){						// Use specified fixture name to make appropriate cmd
							for (var e of chans){
								if (this[e] < fixture[i][e+'min']){						// if (R < fixture[i].Rmin){R = fixture[i].Rmin};
									this[e] = fixture[i][e+'min'];
									};
								nvd.n = fixture[i][e];									// nvd.n = fixture[i].R;
								nvd.v = this[e];										// nvd.v = R;
								if (nvd.v < 65535){										// If there's no real 16bit value in recipe, then don't send node cmd
									packetNVD.push(JSON.stringify(nvd));
									}
								if (status >= 5){
									var nl = {"n": 0, "l": 0};		
									nl.n = fixture[i][e];											// set the cmd array value n
									if (parsedCmdArray[4] == 'normal'){ nl.l = 0}
									else if (parsedCmdArray[4] == 'repeat'){ nl.l = 1}
									else if (parsedCmdArray[4] == 'loop'){ nl.l = 2}
									packetNL.push(JSON.stringify(nl));								// Push single cmd into the total buffer
									}
								}	
							}
						}
					}
				}
			}
			
			if ( packetNL.length > 0 ){
				packetNL = "["+ packetNL +"]";
				udp(packetNL) }
				
			if ( packetNVD.length > 0 ){
				packetNVD = "["+ packetNVD +"]";										// The packets have now been pushed full, time to send via UDP
				udp(packetNVD);
				}
		}

	function udp(packet){	//=============== MANUAL COMMAND SENDING OPTION ==================
		var sockClient = Socket(clientIP);				// clientIP is set in SETUP file (that must be included)
		sockClient.openClientUDP();						// open the socket
		sockClient.write(packet);						// send a message to the socket, \r\n is not needed
		sockClient.close();								// close the socket
		
		//print(packet);									// Valuable Troubleshooting
	}

	function send(packet){	//=============== MANUAL COMMAND SENDING OPTION ==================
		setup();
		var sockClient = Socket(clientIP);				// clientIP is set in SETUP file (that must be included)
		sockClient.openClientUDP();						// open the socket
		sockClient.write(packet);						// send a message to the socket, \r\n is not needed
		sockClient.close();								// close the socket
		
		//print(packet);									// Valuable Troubleshooting
	}
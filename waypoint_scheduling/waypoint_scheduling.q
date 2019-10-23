//This file was generated from (Academic) UPPAAL 4.1.20-stratego-6 (rev. 0DC1FC6317AF6369), October 2019

/*

*/
strategy realistic = minE (total) [<=500] {\
	OtherRobot(2).location,\
	OtherRobot(3).location,\
	OtherRobot(4).location,\
	OtherRobot(5).location,\
	Robot.location,\
	OtherRobot(2).cur,\
	OtherRobot(2).next.type,\
	OtherRobot(2).next.value,\
	OtherRobot(3).cur,\
	OtherRobot(3).next.type,\
	OtherRobot(3).next.value,\
	OtherRobot(4).cur,\
	OtherRobot(4).next.type,\
	OtherRobot(4).next.value,\
	OtherRobot(5).cur,\
	OtherRobot(5).next.type,\
	OtherRobot(5).next.value,\
	Robot.visited[1],\
	Robot.visited[2],\
	Robot.visited[3],\
	Robot.visited[4],\
	Robot.visited[5],\
	Robot.visited[6],\
	Robot.visited[7],\
	Robot.visited[8],\
	Robot.visited[9],\
	Robot.visited[10],\
	Robot.visited[11],\
	Robot.visited[12],\
	Robot.visited[13],\
	Robot.visited[14],\
	Robot.visited[15],\
	Robot.visited[16],\
	Robot.visited[17],\
	Robot.visited[18],\
	Robot.visited[19],\
	Robot.visited[20],\
	Robot.visited[21],\
	Robot.visited[22],\
	Robot.visited[23],\
	Robot.visited[24],\
	Robot.visited[25],\
	Robot.visited[26],\
	Robot.visited[27],\
	Robot.visited[28],\
	Robot.dest,\
	Robot.cur_waypoint,\
	Robot.dest_waypoint\
} -> {\
	Waypoint(1).num_in_queue,\
	Waypoint(2).num_in_queue,\
	Waypoint(3).num_in_queue,\
	Waypoint(4).num_in_queue,\
	Waypoint(5).num_in_queue,\
	Waypoint(6).num_in_queue,\
	Waypoint(7).num_in_queue,\
	Waypoint(8).num_in_queue,\
	Waypoint(9).num_in_queue,\
	Waypoint(10).num_in_queue,\
	Waypoint(11).num_in_queue,\
	Waypoint(12).num_in_queue,\
	Waypoint(13).num_in_queue,\
	Waypoint(14).num_in_queue,\
	Waypoint(15).num_in_queue,\
	Waypoint(16).num_in_queue,\
	Waypoint(17).num_in_queue,\
	Waypoint(18).num_in_queue,\
	Waypoint(19).num_in_queue,\
	Waypoint(20).num_in_queue,\
	Waypoint(21).num_in_queue,\
	Waypoint(22).num_in_queue,\
	Waypoint(23).num_in_queue,\
	Waypoint(24).num_in_queue,\
	Waypoint(25).num_in_queue,\
	Waypoint(26).num_in_queue,\
	Waypoint(27).num_in_queue,\
	Waypoint(28).num_in_queue,\
	Robot.x\
} : <> Robot.Done

/*

*/
simulate 1 [<= 100] {\
    Robot.cur_waypoint, Robot.dest_waypoint, Robot.Holding\
} under realistic


/*

*/
saveStrategy("waypoint_strategy.json", realistic)

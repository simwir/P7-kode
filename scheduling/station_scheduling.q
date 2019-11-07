//This file was generated from (Academic) UPPAAL 4.1.20-stratego-6 (rev. 0DC1FC6317AF6369), October 2019

/*

*/
strategy plan = minE (total) [<=200] : <> Robot.Done

/*

*/
simulate 1 [<= 200] {\
    Robot.cur_loc, Robot.dest\
} under plan

/*

*/
saveStrategy("station_strategy.json", plan)

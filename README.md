# Thief_game
In this game, all the people (the four detectives and the thief) are known as agents, and the game consists of a series of turns, known as cycles.

Each agent starts in a city, determined by user configuration. Every cycle, each agent may move from their current city to an adjacent city by road. The goal of the detectives is to end up in the same city as the thief, which would allow them to catch the thief, while the goal of the thief is to reach the getaway city.

Each agent begins with some stamina, also determined by user configuration. Whenever an agent moves from one city to another, they lose stamina equal to the length of the road between them.

Agents cannot travel along a road if they do not have the required level of stamina. This means it is possible for an agent to have no legal moves. If an agent has no legal moves due to not having enough stamina, they must remain in their current city for another cycle. Remaining in the same city for a turn resets the agent's stamina back to its initial level.

Each detective uses a set strategy to navigate the cities, determined by user configuration. Meanwhile, the thief always moves randomly.

The game ends if one of the following conditions is met:

If a detective starts in the same city as the thief, the thief is caught immediately and the detectives win.
If a detective is in the same city as the thief at the end of a turn, the thief is caught and the detectives win.
If the thief is in the getaway city at the end of a turn and there are no detectives there, the thief escapes, so the thief wins.
If the time has run out, regardless of whether the thief was able to reach the getaway city, the trail has gone cold, so the thief wins.

# Inputs
The client program should be invoked as follows:

`./game <city data file> <agent data file> <cycles> [seed]`
The program requires 3 command-line arguments with an optional fourth. The command-line arguments are

the name of the city data file

the name of the agent data file

the maximum number of cycles in the game

(optional) a seed value for the random number generator; by using the same seed, you can produce the same ordering of 'random' moves and repeat exactly the same situation.

# City data
The first line contains a single integer which is the number of cities. Then, for every city there will be a line of data. Each line begins with the ID of the city, which will always be between 0 and (the number of cities - 1), followed by pairs of integers indicating a road to another city of a certain length. After the roads are listed each line will contain either an 'n' or 'i'. An 'i' indicates that the city has an informant, while an 'n' indicates that it doesn't. At the end of each line is the name of the city.

# Agent data
The first line of data represents information about the thief. The first number represents the amount of stamina the thief starts with, which is also the maximum amount of stamina the thief can have. The second number represents the starting location of the thief. The third number indicates where the getaway city is. This is followed by a string representation (i.e., name) of the thief.

The next four lines represent the detectives. The first two numbers represent the initial/maximum amount of stamina and the starting location of the detective. The third number represents the strategy that the detective is assigned. This is followed by a string representation (i.e., name) of the detective.

# Commands
Once the client program has started the initial state of the game will be displayed and the user will be prompted for input. The available commands are as follows:

Command	Description
run	This will run an entire simulation, printing out a trace of the agents' locations for each cycle of the game. It will print out how the game finished, i.e., with the thief being caught, getting away, or time running out.
step	This runs just one cycle of the game, printing out the new location of the agents for the next cycle. If the game finished in that cycle, it will also print out how the game finished. This allows the user to step through the game one cycle at a time.
stats	This prints out the status of each agent. This includes the name of the agents' current location and the agents' stamina.
display	This displays the current locations of all agents.
map	This prints out the map in a textual format, including the ID/name of each city, and the roads from each city and their length.
quit	Quits the game!

# Agent strategies
Stage 0: RANDOM strategy
In stage 0, all agents use the random strategy. In the random strategy, each agent randomly selects an adjacent city that they have the required stamina to move to and move to it. If the agent does not have sufficient stamina to move to any city, they must remain in their current city for another cycle, which will completely replenish their stamina.

The random strategy has already been implemented, so you are not required to do anything to complete this stage. You should not alter the logic of the random strategy in Agent.c. You should also not use any random number generation in your implementation of the other strategies.

Stage 1: CHEAPEST_LEAST_VISITED strategy
If a detective is assigned this strategy, it means that at every opportunity they have to move, they must move to the city they have visited the least number of times, out of the legal options that are available. This means the detective must work out what cities are actually adjacent to the current city that they have sufficient stamina to move to and pick from those the one that has been visited the least. If there is more than one city with the least number of visits, the city which requires the least stamina among those should be chosen. If there is more than one city with the least number of visits and that requires the least stamina, the city with the lowest ID among those should be chosen.

Note that at the beginning of the game, a detective is considered to have visited their starting city once. Also, if a detective must remain in their current city, this counts as an additional visit, even though the detective did not move.

Stage 2: DFS strategy
In this stage a DFS strategy must be implemented. When following this strategy, the detective maps out an entire route that will take them through every city on the map using the DFS algorithm. If the DFS has a choice between multiple cities, it must prioritise the city with the lowest ID. At every cycle, the detective attempts to move to the next city on the plan. If the detective does not have enough stamina, they must wait in the same city to recover. As soon as the detective has visited all cities at least once, a new DFS path from the final location is mapped out and is followed.

You can assume that the maximum stamina of each detective using the DFS strategy is greater than or equal to the length of the longest road, so no detective using the DFS strategy will be stuck forever at some city while trying to complete their route.

Stage 3: Least Turns Path
In this stage we will test your implementation using city data with informants. If a detective starts at, or moves to a city where there is an informant, they will discover where the thief is currently located (this is achieved by the AgentTipOff function being called). The detective must then find the path to that location that will take the least number of turns and then follow this path. Of course, the thief may be gone by the time the detective gets there, in which case the detective must restart their original strategy from their new location. Any cities the detective passes through on the shortest path are counted as being visited if the detective returns to the CHEAPEST_LEAST_VISITED strategy. The detective may also pass through a city with another informant in which the detective would find a new least turns path from the current location.

You must take into account the stamina of the detective. For example, if one path requires the detective to travel through 3 cities, but would have to rest twice (5 turns), that is more turns than the detective travelling through 4 cities but not having to rest (4 turns). If there are multiple paths that would take the least number of turns, the path that results in the agent having the most stamina at the end should be chosen. If there are multiple paths that would take the least number of turns and would also result in the agent having the same stamina, any of them may be chosen.

You can assume that all detectives will be able to reach every city from every other city. That is, for every pair of cities, there exists a route between them such that the length of the longest road on that route is less than or equal to the maximum stamina of each detective.

// Implementation of the Agent ADT
// A program to set up the map
// Written by Ivan Lun Hui Chen (z5557064@ad.unsw.edu.au)
// On 15/11/2024

// Acknowledgements:
//  - dfs: The following code was adapted from the comp2521 2024T3 Graph 
//    Traversal slides.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
//    This creates the visited array and initialises dfsPath values for dfsRec.
//  - dfsRec: The following code was adapted from the comp2521 2024T3 Graph 
//    Traversal slides.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
//    This uses the dfs algorithm to fill the dfsPath array with the path.
//  - leastTurnsPath: The following code was adapted from the comp2521 2024T3 
//    Graph Traversal slides.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
//    This uses the bfs algorithm to find the shortest path that takes the least
//    number of turns to the thief's location given by the informant.
//  - ltpGetMoves: The following code was adapted from the comp2521 2024T3 Graph
//    Traversal slides.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
//    This gets the moves for the bfs until the queue that holds all the cities
//    that still needs to be accounted for is empty
//  - fillQueue: The following code was adapted from the comp2521 2024T3 Graph 
//    Traversal slides.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
//    Enqueues all the non-visited cities that is adjacent to the current city 
//    into the queue.
//  - Queue.h: The following interface was taken from the comp2521 2024T3 lab4 
//    resources.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/labs/week04/files/Queue.h
//  - Queue.c: The following interface was taken from the comp2521 2024T3 lab4 
//    resources.
//    Link :https://cgi.cse.unsw.edu.au/~cs2521/24T3/labs/week04/files/Queue.c

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "Agent.h"
#include "Map.h"
#include "Queue.h"

// This struct stores information about an individual agent and can be
// used to store information that the agent needs to remember.
struct agent
{
    char *name;
    int startLocation;
    int location;
    int maxStamina; // max stamina
    int stamina;    // current stamina
    int strategy;
    Map map;

    int *citiesVisitedCount;

    struct move *dfsPath;
    int dfsPathSize;
    int dfsPathNumElements;
    int dfsIndex;

    struct move *ltpPath;
    int ltpPathNumElements;
    int ltpIndex;
    int thiefLocation;
};

// This struct is used for simulating the path that the agent takes with the
// expected remaining stamina when the agent is in the city of an informant.
struct costedMove
{
    struct move m;
    int remainingStamina;
    int numMovesTaken;
};

static void printNullError(void);

static struct move chooseRandomMove(Agent agent, Map m);
static int filterRoads(Agent agent, struct road roads[], int numRoads,
                       struct road legalRoads[]);

static struct move chooseClvMove(Agent agent, Map m);
static struct move setClvMove(Agent agent, struct road *legalRoads,
                              struct move clvMove, int *index);
static struct road *createRoads(Map m);
static struct move nextClvMove(Agent agent, int numLegalRoads,
                               struct road *legalRoads);

static struct move chooseDfsMove(Agent agent, Map m);
static void dfs(Agent agent, Map m, int city);
static void dfsRec(Map m, int city, bool *visited, Agent agent);
static struct move *reallocatePathCheck(struct move **path, int numElements,
                                        int *pathSize);

static void leastTurnsPath(Agent agent, Map m, int city, int stamina);
static void ltpGetMoves(Queue q, struct costedMove *predecessor, int stamina,
                        Map m, Agent agent);
static int compare(const void *a, const void *b);
static void fillQueue(struct road *roads, int roadSize, Queue q, 
                      int currentCity, struct costedMove *predecessor,
                      int thiefLocation, int stamina);
static bool pathNeedsUpdating(struct road *roads, int currentCity,
                              struct costedMove *predecessor, int index);
static void copyPathIntoLtpPath(Agent agent, struct costedMove *predecessor);

/**
 * Creates a new agent
 */
Agent AgentNew(int start, int stamina, int strategy, Map m, char *name)
{
    if (start >= MapNumCities(m))
    {
        fprintf(stderr, "error: starting city (%d) is invalid\n", start);
        exit(EXIT_FAILURE);
    }

    Agent agent = malloc(sizeof(struct agent));
    if (agent == NULL)
    {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    agent->startLocation = start;
    agent->location = start;
    agent->maxStamina = stamina;
    agent->stamina = stamina;
    agent->strategy = strategy;
    agent->map = m;
    agent->name = strdup(name);

    agent->citiesVisitedCount = calloc(MapNumCities(m), sizeof(int));
    if (agent->citiesVisitedCount == NULL)
    {
        printNullError();
    }
    agent->citiesVisitedCount[start]++;

    agent->dfsPath = malloc(MapNumCities(m) * sizeof(struct move));
    if (agent->dfsPath == NULL)
    {
        printNullError();
    }
    agent->dfsPathSize = MapNumCities(m);
    agent->dfsPathNumElements = 0;
    agent->dfsIndex = 0;

    agent->ltpPath = malloc(MapNumCities(m) * sizeof(struct move));
    if (agent->ltpPath == NULL)
    {
        printNullError();
    }
    agent->ltpPathNumElements = 0;
    agent->ltpIndex = 0;
    agent->thiefLocation = -1;

    return agent;
}

/**
 * Prints an error message when the program attempted to allocate memory but
 * did not succeed. Then exits the program.
 */
static void printNullError(void)
{
    fprintf(stderr, "error: out of memory\n");
    exit(EXIT_FAILURE);
}

/**
 * Frees all memory allocated to the agent
 * NOTE: You should not free the map because the map is owned by the
 *       main program, and the main program will free it
 */
void AgentFree(Agent agent)
{
    free(agent->citiesVisitedCount);
    free(agent->dfsPath);
    free(agent->ltpPath);
    free(agent->name);
    free(agent);
}

////////////////////////////////////////////////////////////////////////
// Gets information about the agent
// NOTE: It is expected that these functions do not need to be modified

/**
 * Gets the name of the agent
 */
char *AgentName(Agent agent)
{
    return agent->name;
}

/**
 * Gets the current location of the agent
 */
int AgentLocation(Agent agent)
{
    return agent->location;
}

/**
 * Gets the amount of stamina the agent currently has
 */
int AgentStamina(Agent agent)
{
    return agent->stamina;
}

////////////////////////////////////////////////////////////////////////
// Making moves

/**
 * Calculates the agent's next move
 * NOTE: Does NOT actually carry out the move
 */
struct move AgentGetNextMove(Agent agent, Map m)
{
    // When the agent is at a city with an informant.
    if (agent->thiefLocation != -1)
    {
        leastTurnsPath(agent, m, agent->location, agent->maxStamina);
        // sets the dfsIndex to the the end of the array so it sets its moves
        // based on the new dfs created when it returns to the dfs strategy
        agent->dfsIndex = agent->dfsPathNumElements;
    }

    // Checks whether to return to original strategy based on if there are still
    // moves in the shortest path created with the thief's location.
    if (agent->ltpIndex >= 0 && agent->ltpPathNumElements != 0)
    {
        if ((agent->stamina - agent->ltpPath[agent->ltpIndex].staminaCost <
             0))
        {
            return (struct move){agent->location, 0};
        }
        agent->citiesVisitedCount[agent->ltpPath[agent->ltpIndex].to]++;
        return agent->ltpPath[agent->ltpIndex--];
    }

    if (agent->strategy == STATIONARY)
    {
        return (struct move){agent->location, 0};
    }
    else if (agent->strategy == RANDOM)
    {
        return chooseRandomMove(agent, m);
    }
    else if (agent->strategy == CHEAPEST_LEAST_VISITED)
    {
        return chooseClvMove(agent, m);
    }
    else if (agent->strategy == DFS)
    {
        return chooseDfsMove(agent, m);
    }
    else
    {
        printf("error: strategy not implemented yet\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * returns the next move based on a random road which the current city that the
 * agent is in has
 */
static struct move chooseRandomMove(Agent agent, Map m)
{
    struct road *roads = malloc(MapNumCities(m) * sizeof(struct road));
    struct road *legalRoads = malloc(MapNumCities(m) * sizeof(struct road));

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);

    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    struct move move;
    if (numLegalRoads > 0)
    {
        // nextMove is randomly chosen from the legal roads
        int k = rand() % numLegalRoads;
        move = (struct move){legalRoads[k].to, legalRoads[k].length};
    }
    else
    {
        // The agent must stay in the same location
        move = (struct move){agent->location, 0};
    }

    free(legalRoads);
    free(roads);
    return move;
}

/**
 * Takes an array with all the possible roads and puts the ones the agent
 * has enough stamina for into the legalRoads array
 * Returns the number of legal roads
 */
static int filterRoads(Agent agent, struct road roads[], int numRoads,
                       struct road legalRoads[])
{
    int numLegalRoads = 0;
    for (int i = 0; i < numRoads; i++)
    {
        if (roads[i].length <= agent->stamina)
        {
            legalRoads[numLegalRoads++] = roads[i];
        }
    }
    return numLegalRoads;
}

/**
 * Returns a move that takes the least stamina out of the least visited cities
 * of the agent. It takes the city with the lower id if there are multiple 
 * cities requiring the same amount of stamina.
 */
static struct move chooseClvMove(Agent agent, Map m)
{
    struct road *roads = createRoads(m);
    struct road *legalRoads = createRoads(m);

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);
    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    struct move clvMove = nextClvMove(agent, numLegalRoads, legalRoads);

    free(roads);
    free(legalRoads);
    return clvMove;
}

/**
 * Allocates memory for a roads array.
 */
static struct road *createRoads(Map m)
{
    struct road *roads = malloc(MapNumCities(m) * sizeof(struct road));
    if (roads == NULL)
    {
        printNullError();
    }
    return roads;
}

/**
 * Returns the next move for the agent using the Clv strategy
 */
static struct move nextClvMove(Agent agent, int numLegalRoads,
                               struct road *legalRoads)
{
    struct move clvMove;

    if (numLegalRoads <= 0)
    {
        // The agent does not have sufficient stamina
        clvMove = (struct move){agent->location, 0};
    }
    else
    {
        clvMove = (struct move){legalRoads[0].to, legalRoads[0].length};
        for (int i = 1; i < numLegalRoads; i++)
        {
            clvMove = setClvMove(agent, legalRoads, clvMove, &i);
        }
    }

    return clvMove;
}

/**
 * returns the next move based on the number of times that the agent has visited
 * the city and the stamina cost of the possible cities.
 */
static struct move setClvMove(Agent agent, struct road *legalRoads,
                              struct move clvMove, int *index)
{
    if (agent->citiesVisitedCount[legalRoads[*index].to] 
        < agent->citiesVisitedCount[clvMove.to])
    {
        // sets the move with the city that has been visited less.
        clvMove = (struct move){legalRoads[*index].to,
                                legalRoads[*index].length};
    }
    else if (agent->citiesVisitedCount[legalRoads[*index].to] 
             == agent->citiesVisitedCount[clvMove.to])
    {
        // sets the move with the city with the lower stamina cost.
        if (legalRoads[*index].length < clvMove.staminaCost)
        {
            clvMove = (struct move){legalRoads[*index].to,
                                    legalRoads[*index].length};
        }
    }
    return clvMove;
}

/**
 * Returns a move that is based on the dfs algorithm
 */
static struct move chooseDfsMove(Agent agent, Map m)
{
    if ((agent->dfsIndex > agent->dfsPathNumElements - 1))
    {
        // clear the dfsPath array.
        agent->dfsIndex = 0;
        agent->dfsPathNumElements = 0;
        struct move *new = realloc(agent->dfsPath, MapNumCities(m) *
                                                       sizeof(struct move));
        if (new == NULL)
        {
            printNullError();
        }
        else
        {
            agent->dfsPath = new;
            agent->dfsPathSize = MapNumCities(m);
        }

        dfs(agent, m, agent->location);
    }

    if (agent->dfsPath[agent->dfsIndex].staminaCost > agent->stamina)
    {
        return (struct move){agent->location, 0};
    }

    return agent->dfsPath[agent->dfsIndex++];
}

/**
 * The following code was adapted from the comp2521 2024T3 Graph Traversal 
 * slides.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
 * This creates the visited array and initialises dfsPath values for dfsRec.
 */
static void dfs(Agent agent, Map m, int city)
{
    bool *visited = calloc(MapNumCities(m), sizeof(bool));
    if (visited == NULL)
    {
        printNullError();
    }

    dfsRec(m, city, visited, agent);
    free(visited);
}

/**
 * The following code was adapted from the comp2521 2024T3 Graph Traversal 
 * slides.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
 * This uses the dfs algorithm to fill the dfsPath array with the path.
 */
static void dfsRec(Map m, int city, bool *visited, Agent agent)
{
    visited[city] = true;

    struct road *roads = createRoads(m);

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, city, roads);

    for (int i = 0; i < numRoads; i++)
    {
        if (!visited[roads[i].to])
        {
            // checks for need of reallocation of dfsPath array
            agent->dfsPath = reallocatePathCheck(&agent->dfsPath,
                                                 agent->dfsPathNumElements,
                                                 &agent->dfsPathSize);
            agent->dfsPath[agent->dfsPathNumElements++] = (struct move)
                                                          {roads[i].to,
                                                           roads[i].length};

            dfsRec(m, roads[i].to, visited, agent);

            agent->dfsPath = reallocatePathCheck(&agent->dfsPath,
                                                 agent->dfsPathNumElements,
                                                 &agent->dfsPathSize);
            // this adds to the path when the dfsPath is backtracking
            agent->dfsPath[agent->dfsPathNumElements++] = (struct move)
                                                           {roads[i].from,
                                                            roads[i].length};
        }
    }
    free(roads);
}

/**
 * Reallocates twice the memory to path array if the current array is filled
 * completely
 */
static struct move *reallocatePathCheck(struct move **path, int numElements,
                                        int *pathSize)
{
    // reallocates the path with 2 times its size if the path is full.
    if (numElements >= *pathSize)
    {
        *pathSize *= 2;
        struct move *new = realloc(*path, *pathSize * sizeof(struct move));
        if (new == NULL)
        {
            printNullError();
        }
        else
        {
            *path = new;
        }
    }
    return *path;
}

/**
 * The following code was adapted from the comp2521 2024T3 Graph Traversal 
 * slides.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
 * This uses the bfs algorithm to find the shortest path that takes the least
 * number of turns to the thief's location given by the informant.
 */
static void leastTurnsPath(Agent agent, Map m, int city, int stamina)
{
    agent->ltpPathNumElements = 0;

    struct costedMove *predecessor = malloc(MapNumCities(m) *
                                            sizeof(struct costedMove));
    if (predecessor == NULL)
    {
        printNullError();
    }

    //initialises the predecessor array to have the maximum number of turns 
    //and maximum total stamina cost
    for (int i = 0; i < MapNumCities(m); i++)
    {
        predecessor[i] = (struct costedMove){(struct move){-1, 0}, 0, INT_MAX};
    }

    Queue q = QueueNew();

    //initialises the first predecessor with basic data
    predecessor[city] = (struct costedMove){(struct move){-1, 0},
                                            agent->stamina, 0};
    QueueEnqueue(q, city);

    ltpGetMoves(q, predecessor, stamina, m, agent);

    copyPathIntoLtpPath(agent, predecessor);
    free(predecessor);
    QueueFree(q);
}

/**
 * The following code was adapted from the comp2521 2024T3 Graph Traversal 
 * slides.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
 * This gets the moves for leastTurnsPath until the queue that holds all the 
 * cities that still needs to be accounted for is empty
 */
static void ltpGetMoves(Queue q, struct costedMove *predecessor, int stamina,
                        Map m, Agent agent)
{
    while (!QueueIsEmpty(q))
    {
        int curr = QueueDequeue(q);

        struct road *roads = createRoads(m);

        // Get all roads to adjacent cities
        int numRoads = MapGetRoadsFrom(m, curr, roads);
        // sorts the roads in ascending order by the length.
        qsort(roads, numRoads, sizeof(struct road), compare);

        fillQueue(roads, numRoads, q, curr, predecessor, agent->thiefLocation,
                  stamina);

        free(roads);
    }
}

/**
 * Comparison function used by qsort that sorts the two roads given as the
 * parameters by ascending order.
 */
static int compare(const void *a, const void *b)
{
    struct road *x = (struct road *)a;
    struct road *y = (struct road *)b;
    return x->length - y->length;
}

/**
 * The following code was adapted from the comp2521 2024T3 Graph Traversal 
 * slides.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/Week5Mon-graph-traversal.pdf
 * Enqueues all the non-visited cities that is adjacent to the current city into
 * the queue.
 */
static void fillQueue(struct road *roads, int roadSize, Queue q, 
                      int currentCity, struct costedMove *predecessor,
                      int thiefLocation, int stamina)
{
    for (int i = 0; i < roadSize; i++)
    {
        //if including this move in the path currently will result in an
        //additional move to replenish stamina, then don't update
        if (predecessor[currentCity].remainingStamina - roads[i].length 
            < 0)
        {
            // replenish to max stamina
            predecessor[currentCity].remainingStamina = stamina;
            QueueEnqueue(q, currentCity);
            predecessor[currentCity].numMovesTaken++;
        } else if (pathNeedsUpdating(roads, currentCity, predecessor, i))
        {
            //sets the predecessor of the city that the road leads to. This will
            //contain the remaining and overall stamina which takes the city's
            //stamina cost into account
            predecessor[roads[i].to] = (struct costedMove)
                {(struct move){currentCity, roads[i].length},
                  predecessor[currentCity].remainingStamina - roads[i].length,
                  predecessor[currentCity].numMovesTaken + 1};
            QueueEnqueue(q, roads[i].to);
        }
    }
}

/**
 * checks if the current road is leading to a city that has not been explored or
 * if it's the thief's location, checks if taken this road would lead to a path
 * with less turns than the current. If it takes the same number of turns, check
 * if it takes less stamina and returns true if it does.
 */
static bool pathNeedsUpdating(struct road *roads, int currentCity,
                              struct costedMove *predecessor, int index)
{
    bool needsUpdating = true;

    //checks if this road leads to a path with less turns used
    if (predecessor[currentCity].numMovesTaken + 1 <
        predecessor[roads[index].to].numMovesTaken)
    {
        needsUpdating = true;
    }
    else if (predecessor[currentCity].numMovesTaken + 1 ==
                predecessor[roads[index].to].numMovesTaken)
    {
        //checks if this road leads to a path which leaves the agent with the
        //most stamina
        if (predecessor[currentCity].remainingStamina - roads[index].length
            > predecessor[roads[index].to].remainingStamina)
        {
            needsUpdating = true;
        }
        else
        {
            needsUpdating = false;
        }
    }
    else
    {
        needsUpdating = false;
    }

    return needsUpdating;
}

/**
 * Copies the path from predecessor array into ltpPath array of agent so the
 * agent's next moves are based off the least turns path.
 */
static void copyPathIntoLtpPath(Agent agent, struct costedMove *predecessor)
{
    int currentCityIndex = agent->thiefLocation;

    while (currentCityIndex != agent->location)
    {
        agent->ltpPath[agent->ltpPathNumElements++] = (struct move){
            currentCityIndex, predecessor[currentCityIndex].m.staminaCost};
        currentCityIndex = predecessor[currentCityIndex].m.to;
    }

    agent->ltpIndex = agent->ltpPathNumElements - 1;
}

/**
 * Executes a given move by updating the agent's internal state
 */
void AgentMakeNextMove(Agent agent, struct move move)
{
    if (move.to == agent->location)
    {
        agent->stamina = agent->maxStamina;
    }
    else
    {
        agent->stamina -= move.staminaCost;
    }
    agent->location = move.to;
    agent->citiesVisitedCount[move.to]++;
    agent->thiefLocation = -1;
}

////////////////////////////////////////////////////////////////////////
// Learning information

/**
 * Tells the agent where the thief is
 */
void AgentTipOff(Agent agent, int thiefLocation)
{
    agent->thiefLocation = thiefLocation;
}

////////////////////////////////////////////////////////////////////////
// Displaying state

/**
 * Prints information about the agent (for debugging purposes)
 */
void AgentShow(Agent agent)
{
}

////////////////////////////////////////////////////////////////////////

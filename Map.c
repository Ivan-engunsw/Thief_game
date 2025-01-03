// Implementation of the Map ADT
// A program to set up the map
// Written by Ivan Lun Hui Chen (z5557064@ad.unsw.edu.au)
// On 24/10/2024

// Acknowledgements:
//  - MapFree
//    This code was adapted from GraphAdjList.c program code from the lectures.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
//    Frees all the roads, names and the map itself.
//  - adjListInsert
//    This code was adapted from GraphAdjList.c program code from the lectures.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
//    Creates a new road and adds it into the map.
//  - newAdjNode
//    This code was adapted from GraphAdjList.c program code from the lectures.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
//    Creates a new road for the map.
//  - MapContainsRoad
//    This code was adapted from GraphAdjList.c program code from the lectures.
//    Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
//   Checks if the road is already in the map.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"

static void printNullError(void);

static struct adjNode *adjListInsert(struct adjNode *r, int city, int length);
static struct adjNode *newAdjNode(int city, int length);

struct adjNode
{
    int city;
    int length;
    struct adjNode *next;
};

struct map
{
    int numCities;
    int numRoads;
    char **names;
    struct adjNode **roads;
};

/**
 * Creates a new map and allocating all the memory that is needed for a map
 */
Map MapNew(int numCities)
{
    Map m = malloc(sizeof(struct map));
    if (m == NULL)
    {
        printNullError();
    }
    m->numCities = numCities;
    m->numRoads = 0;
    m->names = calloc(numCities, sizeof(char *));
    if (m->names == NULL)
    {
        printNullError();
    }
    m->roads = calloc(numCities, sizeof(struct adjNode *));
    if (m->roads == NULL)
    {
        printNullError();
    }
    return m;
}

/**
 * Prints an error message if memory cannot be allocated and exits the program
 */
static void printNullError(void)
{
    fprintf(stderr, "error: out of memory\n");
    exit(EXIT_FAILURE);
}

/**
 * This code was adapted from GraphAdjList.c program code from the lectures.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
 * Frees all the roads, names and the map itself.
 */
void MapFree(Map m)
{
    for (int i = 0; i < m->numCities; i++)
    {
        free(m->names[i]);
        struct adjNode *curr = m->roads[i];
        while (curr != NULL)
        {
            struct adjNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(m->names);
    free(m->roads);
    free(m);
}

/**
 * Returns the number of cities that is in the map given
 */
int MapNumCities(Map m)
{
    return m->numCities;
}

/**
 * Returns the number of roads that is in the map given
 */
int MapNumRoads(Map m)
{
    return m->numRoads;
}

/**
 * Sets the city's name if it's empty and replaces the name if it has an old
 * name
 */
void MapSetName(Map m, int city, char *name)
{
    if (m->names[city] == NULL)
    {
        m->names[city] = strdup(name);
        if (m->names[city] == NULL)
        {
            printNullError();
        }
    }
    else
    {
        strcpy(m->names[city], name);
    }
}

/**
 * Returns the name of the city given and unnamed if city has no name
 */
char *MapGetName(Map m, int city)
{
    if (m->names[city] == NULL)
    {
        return "unnamed";
    }
    else
    {
        return m->names[city];
    }
}

/**
 * Inserts a road between two cities if there was no road
 */
void MapInsertRoad(Map m, int city1, int city2, int length)
{
    if (MapContainsRoad(m, city1, city2) == 0)
    {
        m->roads[city1] = adjListInsert(m->roads[city1], city2, length);
        m->roads[city2] = adjListInsert(m->roads[city2], city1, length);
        m->numRoads++;
    }
}

/**
 * This code was adapted from GraphAdjList.c program code from the lectures.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
 * Creates a new road and adds it into the map.
 */
static struct adjNode *adjListInsert(struct adjNode *r, int city, int length)
{
    if (r == NULL || city < r->city)
    {
        struct adjNode *new = newAdjNode(city, length);
        new->next = r;
        return new;
    }
    else if (city == r->city)
    {
        return r;
    }
    else
    {
        r->next = adjListInsert(r->next, city, length);
        return r;
    }
}

/**
 * This code was adapted from GraphAdjList.c program code from the lectures.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
 * Creates a new road for the map.
 */
static struct adjNode *newAdjNode(int city, int length)
{
    struct adjNode *new = malloc(sizeof(struct adjNode));
    if (new == NULL)
    {
        printNullError();
    }
    new->city = city;
    new->length = length;
    new->next = NULL;
    return new;
}

/**
 * This code was adapted from GraphAdjList.c program code from the lectures.
 * Link: https://cgi.cse.unsw.edu.au/~cs2521/24T3/lectures/code/week4_graph/GraphAdjList.c
 * Checks if the road is already in the map.
 */
int MapContainsRoad(Map m, int city1, int city2)
{
    struct adjNode *curr = m->roads[city1];
    for (; curr != NULL && city2 >= curr->city; curr = curr->next)
    {
        if (curr->city == city2)
        {
            return curr->length;
        }
    }
    return 0;
}

/**
 * stores all the roads that the given city has into the given roads array and
 * returns the number of roads that were stored
 */
int MapGetRoadsFrom(Map m, int city, struct road roads[])
{
    struct adjNode *curr = m->roads[city];
    int roadIndex = 0;
    while (curr != NULL)
    {
        struct road r = {city, curr->city, curr->length};
        roads[roadIndex] = r;
        roadIndex++;
        curr = curr->next;
    }
    return roadIndex;
}

/**
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * This function will work once the other functions are working
 */
void MapShow(Map m)
{
    printf("Number of cities: %d\n", MapNumCities(m));
    printf("Number of roads: %d\n", MapNumRoads(m));

    struct road *roads = malloc(MapNumRoads(m) * sizeof(struct road));
    if (roads == NULL)
    {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MapNumCities(m); i++)
    {
        printf("[%d] %s has roads to:", i, MapGetName(m, i));
        int numRoads = MapGetRoadsFrom(m, i, roads);
        for (int j = 0; j < numRoads; j++)
        {
            if (j > 0)
            {
                printf(",");
            }
            printf(" [%d] %s (%d)", roads[j].to, MapGetName(m, roads[j].to),
                   roads[j].length);
        }
        printf("\n");
    }

    free(roads);
}

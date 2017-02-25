/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include <machine/spl.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of cars created.
 */

#define NCARS 20

/*
 *
 * Global variables
 *
 */

/*
 * Locks for four intersection grids.
 * 0: North-West grid
 * 1: North-East grid
 * 2: South-East grid
 * 3: South-West grid
 */

struct lock *grids_lock[4]; //N_W, N_E, S_E, S_W;//lock_create(const char *name);

/*
 * CVs for intersection grids lock.
 * 0: North-West grid
 * 1: North-East grid
 * 2: South-East grid
 * 3: South-West grid
 */

struct cv *grids_cv[4]; //N_W, N_E, S_E, S_W;//cv_create(const char *name);

/*
 * Check availability of each grid.
 * If 1, a car can grab the lock of the grid. Initialize to 1.
 * 0: North-West grid
 * 1: North-East grid
 * 2: South-East grid
 * 3: South-West grid
 */

int gridsAvailable[4] = {1, 1, 1, 1}; 

/*
 * Number of cars at the intersection, initially 0
 */

int intersectionCars = 0;

/*
 * Lock for at most three cars at the intersection.
 */

struct lock *intersectionCars_lock;

/*
 * CVs for number of cars at the intersection lock.
 */

struct cv *intersectionCars_cv;


/*
 *
 * Function Definitions
 *
 */

static const char *directions[] = { "N", "E", "S", "W" };

static const char *msgs[] = {
        "approaching:",
        "region1:    ",
        "region2:    ",
        "region3:    ",
        "leaving:    "
};

/* use these constants for the first parameter of message */
enum { APPROACHING, REGION1, REGION2, REGION3, LEAVING };

static void
message(int msg_nr, int carnumber, int cardirection, int destdirection)
{
        kprintf("%s car = %2d, direction = %s, destination = %s\n",
                msgs[msg_nr], carnumber,
                directions[cardirection], directions[destdirection]);
}
 
/*
 * proceed()
 * acquire the lock for the next grid the car is going into
 * and print out message 
 */

static
void
proceed(int lockindex, int msg_nr, int carnumber, int cardirection, int destdirection)
{

	// try to hold the lock
	lock_acquire(grids_lock[lockindex]);
	if(!gridsAvailable[lockindex])
		cv_wait(grids_cv[lockindex], grids_lock[lockindex]);

	// disable the grid for other cars to enter
	gridsAvailable[lockindex] = 0;

	// enter the region successfully
	message(msg_nr, carnumber, cardirection, destdirection);

}

/*
 * gostraight()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement passing straight through the
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */
        
        (void) cardirection;
        (void) carnumber;

	// directions[] index
	int from = (int) cardirection;
	int to = (from + 2) % 4;

	// grids[] index for locks to hold
	int lock1 = (int) cardirection;
	int lock2 = (lock1 + 3) % 4;

	message(APPROACHING, carnumber, from, to);

	// try to enter the first region
	proceed(lock1, REGION1, carnumber, from, to);

	// try to enter the second region
	proceed(lock2, REGION2, carnumber, from, to);

	// release first lock
	gridsAvailable[lock1] = 1;
	cv_signal(grids_cv[lock1], grids_lock[lock1]);
	lock_release(grids_lock[lock1]);

	// leaving the intersection
	message(LEAVING, carnumber, from, to);

	// release second lock
	gridsAvailable[lock2] = 1;
	cv_signal(grids_cv[lock2], grids_lock[lock2]);
	lock_release(grids_lock[lock2]);

}


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;

	// directions[] index
	int from = (int) cardirection;
	int to = (from + 1) % 4;

	// grids[] index for locks to hold
	int lock1 = (int) cardirection;
	int lock2 = (lock1 + 3) % 4;
	int lock3 = (lock1 + 2) % 4;

	message(APPROACHING, carnumber, from, to);

	// try to enter the first region
	proceed(lock1, REGION1, carnumber, from, to);

	// try to enter the second region
	proceed(lock2, REGION2, carnumber, from, to);

	//release first lock
	gridsAvailable[lock1] = 1;
	cv_signal(grids_cv[lock1], grids_lock[lock1]);
	lock_release(grids_lock[lock1]);

	// try to enter the third region
	proceed(lock3, REGION3, carnumber, from, to);

	//release second lock
	gridsAvailable[lock2] = 1;
	cv_signal(grids_cv[lock2], grids_lock[lock2]);
	lock_release(grids_lock[lock2]);

	// leaving the intersection
	message(LEAVING, carnumber, from, to);

	//release third lock
	gridsAvailable[lock3] = 1;
	cv_signal(grids_cv[lock3], grids_lock[lock3]);
	lock_release(grids_lock[lock3]);
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;

 	// directions[] index
	int from = (int) cardirection;
	int to = (from + 3) % 4;

	// grids[] index for locks to hold
	int lock1 = (int) cardirection;

	message(APPROACHING, carnumber, from, to);

	// try to enter the region
	proceed(lock1, REGION1, carnumber, from, to);

	// leaving the intersection
	message(LEAVING, carnumber, from, to);

	// release the lock
	gridsAvailable[lock1] = 1;
	cv_signal(grids_cv[lock1], grids_lock[lock1]);
	lock_release(grids_lock[lock1]);
	
}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long carnumber: holds car id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createcars().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */
 
static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
        int cardirection;
	int turndirection;

        /*
         * Avoid unused variable and function warnings.
         */

        (void) unusedpointer;
        (void) carnumber;
	(void) gostraight;
	(void) turnleft;
	(void) turnright;

        /*
         * cardirection is set randomly.
         */

        cardirection = random() % 4; //N(0), W(1), S(2), E(3)
	turndirection = random() % 3; //go straight(0), turn left(1), turn right(2)



	// acquire lock for ensuring the number of cars at the intersection is smaller than 4
	lock_acquire(intersectionCars_lock);
	if(intersectionCars >= 3)
		cv_wait(intersectionCars_cv, intersectionCars_lock);
	intersectionCars++;
	lock_release(intersectionCars_lock);
        
	// call turn functions
	if(turndirection == 0)
		gostraight(cardirection, carnumber);
	else if(turndirection == 1)
		turnleft(cardirection, carnumber);
	else if(turndirection == 2)
		turnright(cardirection, carnumber);

        // acquire lock for decrementing number of car at the intersection
	lock_acquire(intersectionCars_lock);
	intersectionCars--;
        
        //signal the next car waiting (if there's any) to enter the intersection
	cv_signal(intersectionCars_cv, intersectionCars_lock);
        
        lock_release(intersectionCars_lock);
}


/*
 * createcars()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createcars(int nargs,
           char ** args)
{
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;



	char *gridsLockName[] = {"N_W", "N_E", "S_E", "S_W"};
	//char *queuesName[] = { "from_N", "from_E", "from_S", "from_W" };
	char *gridsCVName[] = {"N_W_cv", "N_E_cv", "S_E_cv", "S_W_cv"};

	for(index = 0; index < 4; index++){

		grids_lock[index] = lock_create(gridsLockName[index]);
		//enter_queue[index] = sem_create(queuesName[index], 1);
		grids_cv[index] = cv_create(gridsCVName[index]);

	}

	intersectionCars_lock = lock_create("car_lock");
	intersectionCars_cv = cv_create("car_cv");



        /*
         * Start NCARS approachintersection() threads.
         */

        for (index = 0; index < NCARS; index++) {

                error = thread_fork("approachintersection thread",
                                    NULL,
                                    index,
                                    approachintersection,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {
                        
                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        return 0;
}

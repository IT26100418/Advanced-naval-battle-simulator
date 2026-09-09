#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "part2C.h"
#include "common.h"


/* Part 2-C: Part 1-C with gun impact-power degradation. */


/* Current impact power of B.
   B starts with IP0 = 1.0. */
static double bImpact(const Battlefield *f)
{
    return exp(
        -f->battleship.gamma *
        f->battleship.shotsFired
    );
}


/* Current impact power of an Escort.
   Its IP0 is the Part 1-C impact power. */
static double eImpact(const EscortShip *e)
{
    return e->impactPower *
           exp(
               -e->gamma *
               e->shotsFired
           );
}


/* Generate the Battleship path. */
static void makePath(
    Point path[],
    int n,
    double size)
{
    int i;

    for (i = 0; i < n; i++)
    {
        path[i] =
            generateRandomPosition(size);
    }
}


/* Reset a simulation to the initial state. */
static void resetField(
    Battlefield *f,
    const Battlefield *initial)
{
    int i;

    *f = *initial;

    f->battleship.status = ALIVE;
    f->battleship.health = 1.0;
    f->battleship.shotsFired = 0;

    f->battleship.lastShotVelocity = 0.0;
    f->battleship.lastShotAngle = 0.0;
    f->battleship.lastFlightTime = 0.0;


    for (i = 0;
         i < f->escortCount;
         i++)
    {
        f->escorts[i].status = ALIVE;
        f->escorts[i].health = 1.0;
        f->escorts[i].shotsFired = 0;

        f->escorts[i].lastShotVelocity = 0.0;
        f->escorts[i].lastShotAngle = 0.0;
        f->escorts[i].lastFlightTime = 0.0;
    }
}


/* Set the same Gamma value for
   every Escort of one type. */
static void setGamma(
    Battlefield *f,
    EscortType type,
    double gamma)
{
    int i;

    for (i = 0;
         i < f->escortCount;
         i++)
    {
        if (f->escorts[i].type == type)
        {
            f->escorts[i].gamma = gamma;
        }
    }
}


/*
 * Get Gamma values.
 *
 * The assignment does not specify exact
 * random Gamma ranges. Therefore:
 *
 * B  : 0.001 - 0.010
 * E  : 0.020 - 0.100
 *
 * This keeps B Gamma close to zero and
 * E Gamma relatively higher.
 */
static void gammaSetup(
    Battlefield *f,
    double egamma[])
{
    int mode;
    double gamma;
    EscortType type;


    printf("\n");
    printf("========================================\n");
    printf("PART 2-C GAMMA SETUP\n");
    printf("========================================\n");

    printf("1. User input\n");
    printf("2. Random generation\n");


    do
    {
        printf("Enter Gamma mode (1-2): ");
        scanf("%d", &mode);

    } while (
        mode != 1 &&
        mode != 2
    );


    /*
     * User input mode.
     */
    if (mode == 1)
    {
        do
        {
            printf(
                "Enter Gamma for Battleship "
                "type %c (> 0): ",
                f->battleship.notation
            );

            scanf(
                "%lf",
                &gamma
            );

        } while (gamma <= 0.0);


        f->battleship.gamma = gamma;


        /*
         * One Gamma value for each
         * Escort type.
         */
        for (type = EA;
             type <= EE;
             type++)
        {
            do
            {
                printf(
                    "Enter Gamma for %s (> 0): ",
                    getEscortTypeName(type)
                );

                scanf(
                    "%lf",
                    &gamma
                );

            } while (gamma <= 0.0);


            egamma[type] = gamma;

            setGamma(
                f,
                type,
                gamma
            );
        }
    }


    /*
     * Random Gamma mode.
     */
    else
    {
        f->battleship.gamma =
            randomDouble(
                0.001,
                0.010
            );


        for (type = EA;
             type <= EE;
             type++)
        {
            egamma[type] =
                randomDouble(
                    0.020,
                    0.100
                );

            setGamma(
                f,
                type,
                egamma[type]
            );
        }
    }


    printf("\nGamma values used:\n");

    printf(
        "B (%c) = %.6f\n",
        f->battleship.notation,
        f->battleship.gamma
    );


    for (type = EA;
         type <= EE;
         type++)
    {
        printf(
            "%s = %.6f\n",
            getEscortTypeName(type),
            egamma[type]
        );
    }
}


/* Save Gamma configuration. */
static void saveGamma(
    const Battlefield *f,
    const double egamma[])
{
    FILE *file;
    EscortType type;


    file = fopen(
        "part2C_gamma_configuration.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error: Cannot create "
            "part2C_gamma_configuration.txt\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 2-C GAMMA CONFIGURATION\n\n"
    );


    fprintf(
        file,
        "Battleship type %c Gamma: %.6f\n",
        f->battleship.notation,
        f->battleship.gamma
    );


    fprintf(
        file,
        "\nESCORT GAMMA VALUES\n"
    );


    for (type = EA;
         type <= EE;
         type++)
    {
        fprintf(
            file,
            "%s Gamma: %.6f\n",
            getEscortTypeName(type),
            egamma[type]
        );
    }


    fclose(file);
}


/*
 * Run one Part 2-C battle step.
 *
 * Part 1-C behaviour is maintained:
 *
 * E -> B:
 *     E gets one firing attempt.
 *     Successful impact is added
 *     to cumulative B damage.
 *
 * B -> E:
 *     B attacks alive Escorts.
 *     B no longer destroys E instantly.
 *     Current B impact is subtracted
 *     from E health.
 */
static void runStep(
    Battlefield *f,
    double minAngle,
    double maxAngle,
    double *cumulative,
    BattleResult *result)
{
    int i;

    int lastEscort = -1;

    double lastTime = 0.0;


    result->battleshipSunk = 0;
    result->killerEscortId = -1;
    result->killerTime = 0.0;

    result->hitCount = 0;
    result->duration = 0.0;

    result->cumulativeImpact =
        *cumulative;


    /*
     * E -> B
     *
     * Every alive E gets one firing
     * attempt, as in Part 1-C.
     */
    for (i = 0;
         i < f->escortCount;
         i++)
    {
        EscortShip *e =
            &f->escorts[i];

        double velocity;
        double angle;
        double flightTime;
        double currentImpact;


        if (e->status != ALIVE)
        {
            continue;
        }


        velocity =
            randomDouble(
                e->minVelocity,
                e->maxVelocity
            );


        /*
         * Count the firing first.
         *
         * Therefore n = 1 for the
         * first firing.
         */
        e->shotsFired++;


        currentImpact =
            eImpact(e);


        if (canHit(
                e->position,
                f->battleship.position,
                velocity,
                e->minAngle,
                e->maxAngle,
                &angle,
                &flightTime))
        {
            e->lastShotVelocity =
                velocity;

            e->lastShotAngle =
                angle;

            e->lastFlightTime =
                flightTime;


            /*
             * Add the current degraded
             * E impact to B.
             */
            *cumulative +=
                currentImpact;


            lastEscort =
                e->id;

            lastTime =
                flightTime;


            printf(
                "Escort #%d hit B. "
                "Current IP = %.4f%%\n",
                e->id,
                currentImpact * 100.0
            );
        }
    }


    /*
     * Calculate B health.
     */
    f->battleship.health =
        1.0 - *cumulative;


    if (f->battleship.health < 0.0)
    {
        f->battleship.health = 0.0;
    }


    /*
     * B is destroyed when cumulative
     * E impact reaches 100%.
     */
    if (*cumulative >= 1.0)
    {
        f->battleship.status =
            SUNK;

        result->battleshipSunk =
            1;

        result->killerEscortId =
            lastEscort;

        result->killerTime =
            lastTime;

        result->duration =
            lastTime;


        printf(
            "Battleship destroyed by "
            "cumulative E impact.\n"
        );

        return;
    }


    /*
     * B -> E
     *
     * B attacks every alive E.
     * The current B IP is calculated
     * after increasing B firing count.
     */
    for (i = 0;
         i < f->escortCount;
         i++)
    {
        EscortShip *e =
            &f->escorts[i];

        double velocity;
        double angle;
        double flightTime;
        double currentImpact;
        double oldHealth;


        if (e->status != ALIVE)
        {
            continue;
        }


        velocity =
            randomDouble(
                0.0,
                f->battleship.maxVelocity
            );


        /*
         * Count B firing.
         */
        f->battleship.shotsFired++;


        /*
         * Calculate:
         *
         * IPn = IP0 * e^(-gamma*n)
         *
         * For B, IP0 = 1.
         */
        currentImpact =
            bImpact(f);


        if (canHit(
                f->battleship.position,
                e->position,
                velocity,
                minAngle,
                maxAngle,
                &angle,
                &flightTime))
        {
            oldHealth =
                e->health;


            /*
             * B damage is now the
             * current degraded IP.
             */
            e->health -=
                currentImpact;


            if (e->health < 0.0)
            {
                e->health = 0.0;
            }


            f->battleship.lastShotVelocity =
                velocity;

            f->battleship.lastShotAngle =
                angle;

            f->battleship.lastFlightTime =
                flightTime;


            if (result->hitCount <
                MAX_ESCORTS)
            {
                result->hitIds[
                    result->hitCount
                ] = e->id;

                result->hitTimes[
                    result->hitCount
                ] = flightTime;

                result->hitCount++;
            }


            if (flightTime >
                result->duration)
            {
                result->duration =
                    flightTime;
            }


            printf(
                "B hit Escort #%d. "
                "IP = %.4f%%, "
                "health %.2f%% -> %.2f%%\n",
                e->id,
                currentImpact * 100.0,
                oldHealth * 100.0,
                e->health * 100.0
            );


            /*
             * E is destroyed only when
             * its health reaches zero.
             */
            if (e->health <= 0.0)
            {
                e->status =
                    SUNK;

                printf(
                    "Escort #%d destroyed.\n",
                    e->id
                );
            }
        }
    }


    result->cumulativeImpact =
        *cumulative;
}


/* Save one simulation step. */
static void saveStep(
    const Battlefield *f,
    const BattleResult *result,
    Point position,
    int step,
    const char *simulation,
    double minAngle)
{
    char filename[120];

    FILE *file;

    int i;


    sprintf(
        filename,
        "part2C_%s_step%d.txt",
        simulation,
        step
    );


    file = fopen(
        filename,
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error: Cannot create %s\n",
            filename
        );

        return;
    }


    fprintf(
        file,
        "PART 2-C %s - STEP %d\n\n",
        simulation,
        step
    );


    fprintf(
        file,
        "Battleship position: "
        "(%.2f, %.2f)\n",
        position.x,
        position.y
    );


    fprintf(
        file,
        "B firing angle: "
        "%.2f - 90.00 degrees\n",
        minAngle
    );


    fprintf(
        file,
        "Battleship status: %s\n",
        f->battleship.status == ALIVE
            ? "ALIVE"
            : "SUNK"
    );


    fprintf(
        file,
        "Battleship health: %.2f%%\n",
        f->battleship.health * 100.0
    );


    fprintf(
        file,
        "Battleship Gamma: %.6f\n",
        f->battleship.gamma
    );


    fprintf(
        file,
        "Battleship firing count: %d\n",
        f->battleship.shotsFired
    );


    fprintf(
        file,
        "Battleship current impact factor: "
        "%.6f\n",
        bImpact(f)
    );


    fprintf(
        file,
        "Cumulative E impact on B: %.2f%%\n",
        result->cumulativeImpact * 100.0
    );


    fprintf(
        file,
        "\nB ATTACK RESULTS\n"
    );


    for (i = 0;
         i < result->hitCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d - flight time "
            "%.2f seconds\n",
            result->hitIds[i],
            result->hitTimes[i]
        );
    }


    fprintf(
        file,
        "\nESCORT STATUS AND CURRENT IMPACT\n"
    );


    for (i = 0;
         i < f->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d (%s):\n",
            f->escorts[i].id,
            getEscortTypeName(
                f->escorts[i].type)
        );


        fprintf(
            file,
            "  Status: %s\n",
            f->escorts[i].status == ALIVE
                ? "ALIVE"
                : "SUNK"
        );


        fprintf(
            file,
            "  Health: %.2f%%\n",
            f->escorts[i].health * 100.0
        );


        fprintf(
            file,
            "  Gamma: %.6f\n",
            f->escorts[i].gamma
        );


        fprintf(
            file,
            "  Firing count: %d\n",
            f->escorts[i].shotsFired
        );


        fprintf(
            file,
            "  Current impact factor: %.6f\n",
            eImpact(
                &f->escorts[i])
        );
    }


    fprintf(
        file,
        "\nBattle duration: %.2f seconds\n",
        result->duration
    );


    fclose(file);
}


/* Save final simulation summary. */
static void saveSummary(
    const Battlefield *f,
    const BattleResult *result,
    double totalTime,
    const char *simulation)
{
    char filename[120];

    FILE *file;

    int i;
    int destroyed = 0;


    sprintf(
        filename,
        "part2C_%s_summary.txt",
        simulation
    );


    file = fopen(
        filename,
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error: Cannot create %s\n",
            filename
        );

        return;
    }


    for (i = 0;
         i < f->escortCount;
         i++)
    {
        if (f->escorts[i].status ==
            SUNK)
        {
            destroyed++;
        }
    }


    fprintf(
        file,
        "PART 2-C %s SUMMARY\n\n",
        simulation
    );


    fprintf(
        file,
        "Battleship Status: %s\n",
        f->battleship.status == ALIVE
            ? "SURVIVED"
            : "SUNK"
    );


    fprintf(
        file,
        "Battleship Health: %.2f%%\n",
        f->battleship.health * 100.0
    );


    fprintf(
        file,
        "Battleship Gamma: %.6f\n",
        f->battleship.gamma
    );


    fprintf(
        file,
        "Battleship Firing Count: %d\n",
        f->battleship.shotsFired
    );


    fprintf(
        file,
        "Battleship Current Impact Factor: "
        "%.6f\n",
        bImpact(f)
    );


    fprintf(
        file,
        "Total Battle Time: %.2f seconds\n",
        totalTime
    );


    fprintf(
        file,
        "Total Cumulative Impact on B: "
        "%.2f%%\n",
        result->cumulativeImpact * 100.0
    );


    fprintf(
        file,
        "Escorts Destroyed: %d\n",
        destroyed
    );


    fprintf(
        file,
        "Escorts Remaining: %d\n",
        f->escortCount - destroyed
    );


    fprintf(
        file,
        "\nFINAL ESCORT IMPACT FACTORS\n"
    );


    for (i = 0;
         i < f->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d (%s): "
            "IP=%.6f, "
            "Gamma=%.6f, "
            "Firings=%d, "
            "Health=%.2f%%, "
            "Status=%s\n",

            f->escorts[i].id,

            getEscortTypeName(
                f->escorts[i].type),

            eImpact(
                &f->escorts[i]),

            f->escorts[i].gamma,

            f->escorts[i].shotsFired,

            f->escorts[i].health * 100.0,

            f->escorts[i].status == ALIVE
                ? "ALIVE"
                : "SUNK"
        );
    }


    fclose(file);
}


/*
 * Run one of the two Part 1-C
 * simulations with degradation.
 */
static void simulation(
    Battlefield *f,
    const Battlefield *initial,
    const Point path[],
    int points,
    int jamIteration,
    double jamAngle,
    int jammed,
    unsigned int seed,
    const char *name)
{
    BattleResult result;

    double cumulative = 0.0;
    double totalTime = 0.0;

    int i;
    int destroyed;


    resetField(
        f,
        initial
    );


    /*
     * Same random starting sequence
     * for Simulation 1, like Part 1-C.
     */
    if (!jammed)
    {
        srand(seed);
    }


    printf("\n");
    printf("========================================\n");
    printf(
        "PART 2-C - %s\n",
        name
    );
    printf("========================================\n");


    for (i = 0;
         i < points;
         i++)
    {
        double minimumAngle;


        if (jammed &&
            i + 1 > jamIteration)
        {
            minimumAngle =
                jamAngle;
        }
        else
        {
            minimumAngle =
                0.0;
        }


        f->battleship.position =
            path[i];


        result.cumulativeImpact =
            cumulative;


        printf("\n");
        printf(
            "--- %s : Step %d ---\n",
            name,
            i + 1
        );


        printf(
            "B position: "
            "(%.2f, %.2f)\n",
            path[i].x,
            path[i].y
        );


        printf(
            "B gun: %s\n",
            minimumAngle > 0.0
                ? "JAMMED"
                : "NORMAL"
        );


        runStep(
            f,
            minimumAngle,
            90.0,
            &cumulative,
            &result
        );


        totalTime +=
            result.duration;


        printf(
            "B current IP: %.6f\n",
            bImpact(f)
        );


        printf(
            "Cumulative E impact: "
            "%.2f%%\n",
            cumulative * 100.0
        );


        printf(
            "B health: %.2f%%\n",
            f->battleship.health * 100.0
        );


        saveStep(
            f,
            &result,
            path[i],
            i + 1,
            name,
            minimumAngle
        );


        if (f->battleship.status ==
            SUNK)
        {
            printf(
                "Battleship destroyed "
                "at step %d.\n",
                i + 1
            );

            break;
        }
    }


    result.cumulativeImpact =
        cumulative;


    destroyed = 0;


    for (i = 0;
         i < f->escortCount;
         i++)
    {
        if (f->escorts[i].status ==
            SUNK)
        {
            destroyed++;
        }
    }


    printf("\n");
    printf(
        "========== PART 2-C SUMMARY =========\n"
    );


    printf(
        "Battleship Status : %s\n",
        f->battleship.status == ALIVE
            ? "SURVIVED"
            : "SUNK"
    );


    printf(
        "B Current Impact  : %.6f\n",
        bImpact(f)
    );


    printf(
        "B Firing Count    : %d\n",
        f->battleship.shotsFired
    );


    printf(
        "Total Battle Time : "
        "%.2f seconds\n",
        totalTime
    );


    printf(
        "Total E Impact    : %.2f%%\n",
        cumulative * 100.0
    );


    printf(
        "Escorts Destroyed : %d\n",
        destroyed
    );


    printf(
        "Escorts Remaining : %d\n",
        f->escortCount - destroyed
    );


    saveSummary(
        f,
        &result,
        totalTime,
        name
    );
}


/* Run complete Part 2-C. */
void runPart2C(
    Battlefield *field)
{
    Battlefield initial;
    Battlefield simulationField;

    Point path[MAX_PATH_POINTS];

    double escortGamma[5];

    int points;
    int jamIteration;

    double jamAngle;

    unsigned int seed;

    int i;


    printf("\n");
    printf(
        "========================================\n"
    );

    printf(
        "              PART 2-C\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "Part 2-C = Part 1-C + "
        "gun impact degradation.\n"
    );


    printf(
        "IPn = IP0 * exp(-Gamma * n)\n"
    );


    /*
     * Get Gamma values first.
     */
    gammaSetup(
        field,
        escortGamma
    );


    /*
     * Gamma values are part of
     * the initial simulation state.
     */
    initial = *field;


    saveGamma(
        field,
        escortGamma
    );


    /*
     * Number of path points.
     */
    do
    {
        printf(
            "\nEnter number of path points "
            "(1-%d): ",
            MAX_PATH_POINTS
        );

        scanf(
            "%d",
            &points
        );

    } while (
        points < 1 ||
        points > MAX_PATH_POINTS
    );


    /*
     * Gun jam iteration.
     */
    do
    {
        printf(
            "Enter gun jam iteration t "
            "(0 < t < %d): ",
            points
        );

        scanf(
            "%d",
            &jamIteration
        );

    } while (
        jamIteration <= 0 ||
        jamIteration >= points
    );


    /*
     * Jam minimum angle.
     */
    do
    {
        printf(
            "Enter theta_min "
            "(0 < theta_min < 30): "
        );

        scanf(
            "%lf",
            &jamAngle
        );

    } while (
        jamAngle <= 0.0 ||
        jamAngle >= 30.0
    );


    /*
     * Generate one path.
     * Both simulations use the
     * same path.
     */
    seed =
        (unsigned int)rand();

    srand(seed);


    makePath(
        path,
        points,
        field->size
    );


    printf("\n");
    printf("PART 2-C PATH\n");
    printf("--------------\n");


    for (i = 0;
         i < points;
         i++)
    {
        printf(
            "Point %d : "
            "(%.2f, %.2f)\n",
            i + 1,
            path[i].x,
            path[i].y
        );
    }


    /*
     * Simulation 1:
     * B gun is normal.
     */
    simulation(
        &simulationField,
        &initial,
        path,
        points,
        jamIteration,
        jamAngle,
        0,
        seed,
        "SIMULATION1"
    );


    /*
     * Simulation 2:
     * B gun becomes jammed after t.
     */
    simulation(
        &simulationField,
        &initial,
        path,
        points,
        jamIteration,
        jamAngle,
        1,
        seed,
        "SIMULATION2"
    );


    printf("\n");
    printf(
        "========================================\n"
    );

    printf(
        "PART 2-C COMPLETED\n"
    );

    printf(
        "========================================\n"
    );
}

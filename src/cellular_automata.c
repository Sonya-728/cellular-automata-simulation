/*
 * Cellular Automata Simulation
 *
 * Portfolio refactor of a 2024 COMP10002 coursework implementation.
 * The program performs one-dimensional elementary cellular automata
 * computations and demonstrates density classification using Rules 184 and 232.
 *
 * This version was reorganised after grading feedback to improve:
 *   - linked-list structure and O(1) state insertion,
 *   - memory ownership and cleanup,
 *   - initialization safety,
 *   - avoidance of duplicate state copies,
 *   - modularity and repeated traversal logic.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ON '*'
#define OFF '.'
#define NBRHDS 8
#define MAX_LINE_LEN 4096

#define SDELIM "==STAGE %d============================\n"
#define MDELIM "-------------------------------------\n"
#define THEEND "==THE END============================\n"

typedef char cells_t;
typedef unsigned char rule_t[NBRHDS];

typedef struct state {
    cells_t *cells;
    struct state *next;
} state_t;

typedef struct {
    state_t *head;   /* state at t = 0 */
    state_t *tail;   /* most recently appended state */
    size_t count;    /* number of stored states */
} run_t;

typedef struct {
    unsigned int code;
    unsigned int size;
    unsigned int time;          /* time index of tail */
    unsigned int stage1_steps;  /* requested Stage 1 evolution steps */
    rule_t rule;
    run_t run;
} CA_t;

static void fail(const char *message);
static char *copy_state_string(const char *source, unsigned int size);
static state_t *state_create_owned(cells_t *cells);
static void run_init(run_t *run, cells_t *initial_cells);
static void run_append_owned(run_t *run, cells_t *cells);
static state_t *state_at(const run_t *run, unsigned int time);
static void run_free(run_t *run);

static void set_rule(CA_t *ca, unsigned int code);
static void read_data(CA_t *ca, int cell_index[2], int start_time[2]);
static void print_rule(const CA_t *ca);
static void print_state_at(const CA_t *ca, unsigned int time);
static void print_automaton(const CA_t *ca, unsigned int start_time,
                            unsigned int stop_time);
static void evolve_steps(CA_t *ca, unsigned int steps);
static void count_cell_states(const CA_t *ca, unsigned int start_step,
                              unsigned int stop_step, unsigned int cell_index,
                              unsigned int *on_count, unsigned int *off_count);

static void do_stage0(const CA_t *ca);
static void do_stage1(CA_t *ca, int cell_index, int start_time);
static void do_stage2(CA_t *ca, int cell_index, int start_time);

static int classify_density_from_final(const CA_t *ca);

int
main(void)
{
    CA_t *ca = calloc(1, sizeof(*ca));
    if (ca == NULL) {
        fail("Unable to allocate cellular automaton.");
    }

    int cell_index[2] = {0, 0};
    int start_time[2] = {0, 0};

    read_data(ca, cell_index, start_time);

    do_stage0(ca);
    do_stage1(ca, cell_index[0], start_time[0]);
    do_stage2(ca, cell_index[1], start_time[1]);

    run_free(&ca->run);
    free(ca);

    return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------- */

static void
fail(const char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

static char *
copy_state_string(const char *source, unsigned int size)
{
    char *copy = malloc((size_t)size + 1U);
    if (copy == NULL) {
        fail("Unable to allocate cell state.");
    }

    memcpy(copy, source, size);
    copy[size] = '\0';
    return copy;
}

static state_t *
state_create_owned(cells_t *cells)
{
    state_t *state = malloc(sizeof(*state));
    if (state == NULL) {
        free(cells);
        fail("Unable to allocate state node.");
    }

    state->cells = cells;
    state->next = NULL;
    return state;
}

static void
run_init(run_t *run, cells_t *initial_cells)
{
    state_t *initial = state_create_owned(initial_cells);
    run->head = initial;
    run->tail = initial;
    run->count = 1U;
}

static void
run_append_owned(run_t *run, cells_t *cells)
{
    state_t *new_state = state_create_owned(cells);

    /* Keep a tail pointer so insertion is O(1), not O(n). */
    run->tail->next = new_state;
    run->tail = new_state;
    run->count++;
}

static state_t *
state_at(const run_t *run, unsigned int time)
{
    if ((size_t)time >= run->count) {
        return NULL;
    }

    state_t *state = run->head;
    for (unsigned int t = 0; t < time; ++t) {
        state = state->next;
    }
    return state;
}

static void
run_free(run_t *run)
{
    state_t *state = run->head;

    while (state != NULL) {
        state_t *next = state->next;
        free(state->cells);
        free(state);
        state = next;
    }

    run->head = NULL;
    run->tail = NULL;
    run->count = 0U;
}

/* ------------------------------------------------------------------------- */

static void
set_rule(CA_t *ca, unsigned int code)
{
    if (code > 255U) {
        fail("Rule code must be between 0 and 255.");
    }

    ca->code = code;
    for (unsigned int i = 0; i < NBRHDS; ++i) {
        ca->rule[i] = (unsigned char)((code >> i) & 1U);
    }
}

static void
read_data(CA_t *ca, int cell_index[2], int start_time[2])
{
    char line[MAX_LINE_LEN];

    if (fgets(line, sizeof(line), stdin) == NULL ||
        sscanf(line, "%u", &ca->size) != 1 ||
        ca->size == 0U) {
        fail("Invalid automaton size.");
    }

    unsigned int code = 0U;
    if (fgets(line, sizeof(line), stdin) == NULL ||
        sscanf(line, "%u", &code) != 1) {
        fail("Invalid rule code.");
    }
    set_rule(ca, code);

    if (fgets(line, sizeof(line), stdin) == NULL) {
        fail("Missing initial state.");
    }
    line[strcspn(line, "\r\n")] = '\0';

    if (strlen(line) != ca->size) {
        fail("Initial state length does not match automaton size.");
    }

    for (unsigned int i = 0; i < ca->size; ++i) {
        if (line[i] != ON && line[i] != OFF) {
            fail("Initial state must contain only '*' and '.'.");
        }
    }

    cells_t *initial_cells = copy_state_string(line, ca->size);
    run_init(&ca->run, initial_cells);

    unsigned int stage1_steps = 0U;
    if (fgets(line, sizeof(line), stdin) == NULL ||
        sscanf(line, "%u", &stage1_steps) != 1) {
        fail("Invalid Stage 1 step count.");
    }

    if (fgets(line, sizeof(line), stdin) == NULL ||
        sscanf(line, "%d,%d", &cell_index[0], &start_time[0]) != 2) {
        fail("Invalid Stage 1 cell-count request.");
    }

    if (fgets(line, sizeof(line), stdin) == NULL ||
        sscanf(line, "%d,%d", &cell_index[1], &start_time[1]) != 2) {
        fail("Invalid Stage 2 cell-count request.");
    }

    for (int i = 0; i < 2; ++i) {
        if (cell_index[i] < 0 ||
            (unsigned int)cell_index[i] >= ca->size ||
            start_time[i] < 0) {
            fail("Cell index or start time is out of range.");
        }
    }

    ca->time = 0U;
    ca->stage1_steps = stage1_steps;
}

/* ------------------------------------------------------------------------- */

static void
print_rule(const CA_t *ca)
{
    printf(MDELIM);
    printf(" 000 001 010 011 100 101 110 111\n");

    for (unsigned int i = 0; i < NBRHDS; ++i) {
        printf("  %u ", (unsigned int)ca->rule[i]);
    }
    printf("\n");
    printf(MDELIM);
}

static void
print_state_at(const CA_t *ca, unsigned int time)
{
    state_t *state = state_at(&ca->run, time);
    if (state == NULL) {
        fail("Requested state does not exist.");
    }

    printf("%4u: %s\n", time, state->cells);
}

static void
print_automaton(const CA_t *ca, unsigned int start_time,
                unsigned int stop_time)
{
    if (start_time > stop_time) {
        return;
    }

    state_t *state = state_at(&ca->run, start_time);
    if (state == NULL) {
        fail("Requested start state does not exist.");
    }

    for (unsigned int t = start_time; t <= stop_time; ++t) {
        if (state == NULL) {
            fail("Automaton history ended unexpectedly.");
        }

        printf("%4u: %s\n", t, state->cells);
        state = state->next;
    }

    printf(MDELIM);
}

static void
evolve_steps(CA_t *ca, unsigned int steps)
{
    for (unsigned int step = 0; step < steps; ++step) {
        const cells_t *current = ca->run.tail->cells;
        cells_t *next = malloc((size_t)ca->size + 1U);

        if (next == NULL) {
            fail("Unable to allocate next cell state.");
        }

        for (unsigned int i = 0; i < ca->size; ++i) {
            unsigned int left_i = (i + ca->size - 1U) % ca->size;
            unsigned int right_i = (i + 1U) % ca->size;

            unsigned int left = (current[left_i] == ON) ? 1U : 0U;
            unsigned int centre = (current[i] == ON) ? 1U : 0U;
            unsigned int right = (current[right_i] == ON) ? 1U : 0U;

            unsigned int neighbourhood =
                (left << 2U) | (centre << 1U) | right;

            next[i] = ca->rule[neighbourhood] ? ON : OFF;
        }

        next[ca->size] = '\0';

        /*
         * Ownership of 'next' moves directly into the linked list.
         * No extra buffer or memcpy is required.
         */
        run_append_owned(&ca->run, next);
        ca->time++;
    }
}

static void
count_cell_states(const CA_t *ca, unsigned int start_step,
                  unsigned int stop_step, unsigned int cell_index,
                  unsigned int *on_count, unsigned int *off_count)
{
    *on_count = 0U;
    *off_count = 0U;

    if (start_step > stop_step) {
        return;
    }

    state_t *state = state_at(&ca->run, start_step);
    if (state == NULL) {
        fail("Requested counting start state does not exist.");
    }

    for (unsigned int t = start_step; t <= stop_step; ++t) {
        if (state == NULL) {
            fail("Automaton history ended unexpectedly during counting.");
        }

        if (state->cells[cell_index] == ON) {
            (*on_count)++;
        } else {
            (*off_count)++;
        }

        state = state->next;
    }
}

/* ------------------------------------------------------------------------- */

static void
do_stage0(const CA_t *ca)
{
    printf(SDELIM, 0);
    printf("SIZE: %u\nRULE: %u\n", ca->size, ca->code);
    print_rule(ca);
    print_state_at(ca, 0U);
}

static void
do_stage1(CA_t *ca, int cell_index, int start_time)
{
    printf(SDELIM, 1);
    evolve_steps(ca, ca->stage1_steps);
    print_automaton(ca, 0U, ca->time);

    unsigned int on_count = 0U;
    unsigned int off_count = 0U;

    count_cell_states(ca, (unsigned int)start_time, ca->time,
                      (unsigned int)cell_index, &on_count, &off_count);

    printf("#ON=%u #OFF=%u CELL#%d START@%d\n",
           on_count, off_count, cell_index, start_time);
}

/*
 * Return:
 *   1  -> original state has more ON than OFF
 *   0  -> equal numbers
 *  -1  -> original state has more OFF than ON
 *
 * The classification is inferred from the final Rule-232 state, rather than
 * by directly counting the original state.
 */
static int
classify_density_from_final(const CA_t *ca)
{
    const cells_t *cells = ca->run.tail->cells;

    int all_on = 1;
    int all_off = 1;
    int alternating = 1;

    for (unsigned int i = 0; i < ca->size; ++i) {
        if (cells[i] != ON) {
            all_on = 0;
        }
        if (cells[i] != OFF) {
            all_off = 0;
        }

        if (i > 0U && cells[i] == cells[i - 1U]) {
            alternating = 0;
        }
    }

    if (all_on) {
        return 1;
    }
    if (all_off) {
        return -1;
    }
    if (alternating) {
        return 0;
    }

    fail("Final state is not a valid density-classification pattern.");
    return 0; /* unreachable */
}

static void
do_stage2(CA_t *ca, int cell_index, int start_time)
{
    unsigned int classification_time = ca->time;
    unsigned int steps_184 = (ca->size - 2U) / 2U;
    unsigned int steps_232 = (ca->size - 1U) / 2U;

    printf(SDELIM, 2);

    printf("RULE: 184; STEPS: %u.\n", steps_184);
    printf(MDELIM);
    set_rule(ca, 184U);

    unsigned int start_184 = ca->time;
    evolve_steps(ca, steps_184);
    print_automaton(ca, start_184, ca->time);

    printf("RULE: 232; STEPS: %u.\n", steps_232);
    printf(MDELIM);
    set_rule(ca, 232U);

    unsigned int start_232 = ca->time;
    evolve_steps(ca, steps_232);
    print_automaton(ca, start_232, ca->time);

    unsigned int on_count = 0U;
    unsigned int off_count = 0U;

    count_cell_states(ca, (unsigned int)start_time, ca->time,
                      (unsigned int)cell_index, &on_count, &off_count);

    printf("#ON=%u #OFF=%u CELL#%d START@%d\n",
           on_count, off_count, cell_index, start_time);
    printf(MDELIM);

    print_state_at(ca, classification_time);

    int result = classify_density_from_final(ca);

    if (result > 0) {
        printf("AT T=%u: #ON/#CELLS > 1/2\n", classification_time);
    } else if (result < 0) {
        printf("AT T=%u: #ON/#CELLS < 1/2\n", classification_time);
    } else {
        printf("AT T=%u: #ON/#CELLS = 1/2\n", classification_time);
    }

    printf(THEEND);
}

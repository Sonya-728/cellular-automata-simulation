# Refactoring notes

This portfolio version addresses the main issues identified in the original
coursework feedback.

| Feedback | Refactor |
|---|---|
| Duplicate code segments | Shared traversal/counting/printing logic moved into helper functions. |
| Unnecessary duplication/copying of data | The initial state is stored once, and new cell buffers are transferred directly into list nodes. |
| Structural issues | Clear separation between run history, simulation, I/O, and density classification. |
| O(n) node insertion | `run_t` now stores a tail pointer, so appending a state is O(1). |
| Initial/current state not linked | The initial state is the head and tail of one linked history from t=0 onward. |
| Incomplete freeing | `run_free` releases every cell buffer and every state node before freeing the automaton. |
| Uninitialised values / invalid reads-writes | Fields are zero-initialised, buffers are sized with a null terminator, and state accesses are validated. |

The refactored source compiles with `-Wall -Wextra -Werror -pedantic` and the
provided sample run was also checked with AddressSanitizer and UndefinedBehaviorSanitizer.

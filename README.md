*This activity has been created as part of the 42 curriculum by jfoeller.*

# Codexion

## Description
**Codexion** is a concurrent programming project developed at school 42. It models an interactive simulation where multiple coders share a quantum compiler and compete for a limited set of specialized hardware USB dongles. 

Each coder cycles continuously through three primary states: **compiling**, **debugging**, and **refactoring**. To compile quantum code, a coder must simultaneously hold two adjacent dongles (one in each hand). The system enforces a mandatory hardware cooldown period after each dongle release and resolves contention using an internal min-heap priority scheduler implementing either **FIFO** (*First In, First Out*) or **EDF** (*Earliest Deadline First*) arbitration. The simulation terminates when either a coder burns out due to a missed deadline or all coders have completed the required number of compilations.

---

## Instructions

### Compilation
The project includes a standard `Makefile` compiling with `cc` and the mandatory flags `-Wall -Wextra -Werror -pthread`:

```bash
make
```

Available Makefile rules: `all`, `clean`, `fclean`, `re`.

### Execution
Run the program with the following eight mandatory arguments:

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

* `number_of_coders`: Total number of coders and dongles.
* `time_to_burnout`: Time in milliseconds without compiling before a coder burns out.
* `time_to_compile`: Duration of the compilation phase in milliseconds.
* `time_to_debug`: Duration of the debugging phase in milliseconds.
* `time_to_refactor`: Duration of the refactoring phase in milliseconds.
* `number_of_compiles_required`: Number of compilations each coder must complete before the simulation stops.
* `dongle_cooldown`: Cooldown time in milliseconds before a released dongle can be used again.
* `scheduler`: Arbitration policy, strictly either `fifo` or `edf`.

#### Examples
* Run an EDF simulation with 5 coders:
  ```bash
  ./codexion 5 2000 200 200 200 3 100 edf
  ```
* Run a FIFO simulation with cooldown set to 0 ms:
  ```bash
  ./codexion 5 2000 200 200 200 3 0 fifo
  ```

### Global Variables Verification
Per 42 evaluation guidelines, global variables are strictly forbidden. You can verify that all exported symbols in the object files belong strictly to the code segment (`T` for text/function) using the `nm` utility:

```bash
nm -g --defined-only *.o
```

Expected output: only `T` symbols (functions) should be displayed, confirming the total absence of data segment symbols (`D`, `d`, `B`, `b`).

### Compilation Count Verification
To verify that compilations are distributed fairly and that every coder reaches their target quota without starvation, you can process the simulation stream using a Unix pipeline:

```bash
./codexion 5 900 200 200 100 5 0 edf | grep "is compiling" | awk '{print $2}' | sort | uniq -c
```

* 🔍 `grep "is compiling"`: Filters the log to keep only lines where a compilation starts.
* 🔢 `awk '{print $2}'`: Extracts the second column, corresponding to the coder ID.
* 🗂️ `sort`: Groups identical coder IDs together.
* 📊 `uniq -c`: Prefixes each unique coder ID with its total count of compilations.

---

## Resources

### References
* **POSIX Threads:** `pthread_create`, `pthread_join`, `pthread_mutex_*`, `pthread_cond_*` manual pages.
* **Operating Systems Concepts:** Silberschatz, Galvin & Gagne (Deadlocks and Synchronization).
* **Scheduling Algorithms:** Stankovic et al., *Deadline Scheduling for Real-Time Systems: EDF and Rate Monotonic*.
* **Data Structures:** Cormen, Leiserson, Rivest & Stein (Binary Min-Heaps and Priority Queues).

### AI Usage
Artificial Intelligence was utilized as a pedagogical thought partner throughout this project for:
* **Concurrency analysis:** Identifying potential data races and validating the elimination of Coffman's *Hold and Wait* condition.
* **Algorithm design:** Reviewing the custom binary min-heap implementation, bubble-up/down invariants, and deterministic tie-breaking logic.
* **Edge case validation:** Diagnosing self-deadlocks on single-coder configurations (`nb_coders == 1`) and verifying zero-cooldown handling.
* **Compliance verification:** Ensuring code structure and function line bounds conform to 42 Norminette requirements.

---

## Blocking Cases Handled

### Deadlock Prevention & Coffman's Conditions
A deadlock can only occur if all four Coffman conditions hold simultaneously. Codexion eliminates deadlock by invalidating these conditions:
1. **Eliminating Hold and Wait:** Coders do not acquire a single dongle and then block waiting for the other. The `wait_both_cooldowns` routine atomically checks that both adjacent dongles are not `in_use` and have passed their respective `dongle_cooldown` before the coder claims ownership.
2. **Eliminating Circular Wait:** Physical dongle mutexes are acquired strictly in increasing order of their numerical identifier (`dongle_id`), establishing a strict partial resource ordering.
3. **Single Coder Edge Case:** When `nb_coders == 1`, both adjacent pointers reference the exact same dongle mutex. Re-locking a held non-recursive mutex results in an immediate self-deadlock. A dedicated condition detects `nb_coders == 1`, logs the single dongle acquisition, and waits passively for the monitor to detect burnout without attempting a self-locking sequence.

### Starvation Prevention
Under the `edf` scheduler, coders closest to burnout ($last\_compile + time\_to\_burnout$) are given priority in the min-heap. To guarantee deterministic behavior when two deadlines are identical, an explicit tie-breaker based on coder ID ensures no coder is starved due to ambiguous priority.

### Cooldown Handling
Each dongle maintains a `free_time` timestamp updated upon release. A thread attempting to acquire a dongle calculates the exact time difference remaining until `free_time + dongle_cooldown` and suspends execution with high-precision sleeps, preventing premature acquisition or busy-waiting.

### Precise Burnout Detection
A dedicated background monitor thread samples coder states at 1 ms intervals. Burnout is triggered if `current_time - last_compile > time_to_burnout`, guaranteeing that burnout logs appear within the mandatory 10 ms window.

### Log Serialization
All console logging is protected by a dedicated `can_display` mutex. Once the simulation stop condition is set, further non-burnout log messages are discarded, preventing interleaved text lines and post-termination prints.

---

## Thread Synchronization Mechanisms

### Synchronization Primitives
* `pthread_mutex_t`:
  * `is_available`: Protects individual hardware dongles from concurrent physical access.
  * `state_lock`: Protects individual coder variables (`last_compile`, `count_compile`).
  * `can_display`: Serializes console writes to `stdout` to avoid garbled terminal outputs.
  * `can_stop`: Protects the simulation-wide termination boolean `stop`.
  * `secure_heap`: Protects the shared min-heap priority queue and the `in_use` flags of dongles.
* `pthread_cond_t`:
  * `wait_heap`: Coordinates waking coder threads whenever a dongle is released, the heap is modified, or the simulation stops.

### Coordination of Shared Resources
* **Dongle Allocation:** When a coder requests dongles, it registers in the shared heap under `secure_heap`. When dongles are released in `compile`, `in_use` flags are set to `false`, new `free_time` values are stamped, and `pthread_cond_broadcast(&sim->wait_heap)` notifies all waiting threads to re-evaluate their turn and dongle availability.
* **Coder to Monitor Communication:** Coders update `last_compile` under `state_lock`. The monitor safely reads these timestamps under the same mutex lock, eliminating data races between compilation routines and burnout verification.
* **Graceful Termination:** When the monitor detects a burnout or that all coders reached `number_of_compiles_required`, it sets `sim->stop = true` under `can_stop` and broadcasts to `wait_heap`, unblocking all threads sleeping on condition variables so they can exit cleanly and join.
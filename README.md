# Carbon-Aware FCFS Scheduling

A C simulation comparing conventional First-Come, First-Served (FCFS) CPU scheduling with a carbon-aware variant. The project explores how delaying work until lower-carbon periods affects estimated emissions, waiting time, turnaround time, and deadline completion.

## Scheduling policies

- **Normal FCFS** (`newfcfs.c`): executes processes in the supplied order as soon as the CPU and process are available.
- **Carbon-aware FCFS** (`newcarbonfcfs.c`): preserves that order. When carbon intensity at the proposed start exceeds 500 gCO2/kWh, it searches for the earliest uninterrupted execution window at or below the threshold that finishes by the process deadline. If none exists, it starts immediately.

Both programs are non-preemptive. Supply processes in arrival-time order; neither program sorts the input. Deferring one process can delay later processes, so the carbon-aware policy does not guarantee that every deadline is met. If intensity at the proposed start is already below the threshold, execution begins without checking the rest of that window.

## Build and run

Use a C99-compatible compiler, such as GCC. No external libraries are required. Run commands from the repository root so the programs can find `carbon.txt` in the current working directory.

### Windows PowerShell (GCC on PATH)

```powershell
gcc -std=c99 -Wall -Wextra newfcfs.c -o fcfs.exe
gcc -std=c99 -Wall -Wextra newcarbonfcfs.c -o carbon_fcfs.exe
.\fcfs.exe
.\carbon_fcfs.exe
```

### Linux or macOS (GCC or compatible compiler)

```sh
gcc -std=c99 -Wall -Wextra newfcfs.c -o fcfs
gcc -std=c99 -Wall -Wextra newcarbonfcfs.c -o carbon_fcfs
./fcfs
./carbon_fcfs
```

Enter the process count, followed by one row per process:

```text
PID ArrivalTime BurstTime Deadline
```

Times are integer seconds, and deadlines are absolute simulation times. For example, enter the following into each program to compare the same workload:

```text
3
1 0 2 10
2 1 1 12
3 2 1 15
```

## Carbon trace

`carbon.txt` contains whitespace-separated time/intensity pairs, without a header:

```text
0 700
1 700
2 600
3 600
4 300
5 300
6 200
```

The first column is the simulation second; the second is carbon intensity in gCO2/kWh. Provide a value for every second that may be used: missing entries default to zero and can produce misleading estimates or scheduling decisions.

## Output and model

Both programs print a text Gantt chart, per-process results, average waiting and turnaround times, CPU utilization, execution duration, energy, estimated carbon emissions, and deadlines met/missed. The carbon-aware version also reports deferral time (`DEF`).

| Field | Meaning |
| --- | --- |
| AT / BT / DL | Arrival time / burst time / deadline |
| ST / CT | Start time / completion time |
| WT | Start minus arrival |
| TAT | Completion minus arrival |
| DEF | Additional delay introduced by the carbon-aware policy |

The model assumes constant active CPU power of **50 W** and one-second time steps:

- Energy (J) = 50 × total burst seconds.
- Energy (kWh) = energy (J) / 3,600,000.
- Carbon (gCO2) = sum of `(50 / 3,600,000) × intensity[t]` across execution seconds.
- CPU utilization (%) = 100 × total burst time / final completion time.

Idle energy is excluded. For identical executed work, modeled energy remains the same; changing execution times can change estimated emissions. The C programs simulate schedules and calculate estimates; they do not measure hardware power or execute real CPU workloads.

## Repository contents

| Path | Contents |
| --- | --- |
| `newfcfs.c` | Conventional FCFS simulator |
| `newcarbonfcfs.c` | Carbon-aware FCFS simulator |
| `carbon.txt` | Carbon-intensity input trace |
| `process_metrics_verification.txt` / `.csv` | Saved process-metric verification results |
| `metrics_graphs_run.txt` | Saved metric checks and comparison summaries |
| Root-level `.png` files | Emissions, energy, timing, and deadline comparison figures |
| `01_Main_Research_Paper_Carbon_Aware_Scheduling_AWS_EC2_Validated.docx` | Research paper artifact |
| `Carbon_Aware_Scheduling_Supplementary_Datasets.zip` | Supplementary dataset archive |
| [`initial_version/`](initial_version/README.md) | Earlier simulation documentation and graphs |

## Current limits

- The checked-in programs allocate space for at most **100 processes**. Normal FCFS validates the count; the carbon-aware program does not. Use 1–100 processes with either program.
- Use nonnegative arrival times, positive burst times, and valid integer input. Input parsing is not fully validated.
- The carbon array covers seconds **0–9999**. Keep all execution within that range (completion at or before 10000); normal FCFS does not bounds-check its carbon reads.
- Saved reports include workloads larger than 100 processes. Reproducing those requires changing the capacity and validating the implementation; they cannot be run safely with the current limits.
- Comparison figures and verification logs are saved artifacts. The repository does not include a standalone script to regenerate them.

Power, trace capacity, process capacity, and the carbon threshold are compile-time constants near the top of the source files. Recompile after changing them.

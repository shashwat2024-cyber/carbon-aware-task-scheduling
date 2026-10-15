
#include <stdio.h>

#define MAX 100
#define MAX_TIME 10000

#define POWER 50.0
#define JOULES_PER_KWH 3600000.0

struct Process {
    int pid;
    int arrival;
    int burst;
    int deadline;
    int start;
    int completion;
    int waiting;
    int turnaround;
};

int main() {
    struct Process p[MAX];
    double carbon[MAX_TIME];

    int n, i, t;
    int current_time = 0;
    int total_burst = 0;
    int deadlines_met = 0;

    double total_waiting = 0;
    double total_turnaround = 0;
    double total_energy;
    double energy_kwh;
    double total_carbon = 0;
    double cpu_utilization;

    /* Read carbon data for measurement only */
    for (i = 0; i < MAX_TIME; i++) {
        carbon[i] = 0;
    }

    FILE *fp = fopen("carbon.txt", "r");

    if (fp == NULL) {
        printf("ERROR: carbon.txt not found.\n");
        return 1;
    }

    int time;
    double value;

    while (fscanf(fp, "%d %lf", &time, &value) == 2) {
        if (time >= 0 && time < MAX_TIME) {
            carbon[time] = value;
        }
    }

    fclose(fp);

    printf("\n============================================\n");
    printf("           NORMAL FCFS SCHEDULER\n");
    printf("============================================\n");

    printf("CPU Power: %.0f W\n", POWER);

    printf("\nEnter number of processes: ");
    scanf("%d", &n);

    if (n < 1 || n > MAX) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    printf("\nEnter process details:\n");
    printf("PID  Arrival Time  Burst Time  Deadline\n");

    for (i = 0; i < n; i++) {
        printf("\nProcess %d: ", i + 1);

        scanf("%d %d %d %d",
              &p[i].pid,
              &p[i].arrival,
              &p[i].burst,
              &p[i].deadline);
    }

    /* Normal FCFS: execute in the supplied arrival order */
    for (i = 0; i < n; i++) {

        if (current_time < p[i].arrival) {
            current_time = p[i].arrival;
        }

        p[i].start = current_time;
        p[i].completion = p[i].start + p[i].burst;

        p[i].waiting = p[i].start - p[i].arrival;
        p[i].turnaround = p[i].completion - p[i].arrival;

        if (p[i].completion <= p[i].deadline) {
            deadlines_met++;
        }

        total_waiting += p[i].waiting;
        total_turnaround += p[i].turnaround;
        total_burst += p[i].burst;

        /* Measure energy and carbon; do not delay processes */
        for (t = p[i].start; t < p[i].completion; t++) {
            total_carbon +=
                (POWER / JOULES_PER_KWH) * carbon[t];
        }

        current_time = p[i].completion;
    }

    total_energy = POWER * total_burst;
    energy_kwh = total_energy / JOULES_PER_KWH;

    if (current_time > 0) {
        cpu_utilization =
            ((double)total_burst / current_time) * 100.0;
    } else {
        cpu_utilization = 0;
    }

    printf("\n============================================\n");
    printf("                 GANTT CHART\n");
    printf("============================================\n");

    for (i = 0; i < n; i++) {
        printf("| P%d ", p[i].pid);
    }

    printf("|\n");

    for (i = 0; i < n; i++) {
        printf("-----");
    }

    printf("-\n");

    printf("%d", p[0].start);

    for (i = 0; i < n; i++) {
        printf("    %d", p[i].completion);
    }

    printf("\n");

    printf("\n============================================\n");
    printf("              PROCESS RESULTS\n");
    printf("============================================\n");

    printf("PID\tAT\tBT\tDL\tST\tCT\tWT\tTAT\n");
    printf("----------------------------------------------------\n");

    for (i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].arrival,
               p[i].burst,
               p[i].deadline,
               p[i].start,
               p[i].completion,
               p[i].waiting,
               p[i].turnaround);
    }

    printf("\n============================================\n");
    printf("                 FCFS METRICS\n");
    printf("============================================\n");

    printf("Average Waiting Time     : %.2f\n",
           total_waiting / n);

    printf("Average Turnaround Time  : %.2f\n",
           total_turnaround / n);

    printf("CPU Utilization          : %.2f%%\n",
           cpu_utilization);

    printf("CPU Power                : %.2f W\n", POWER);

    printf("Total Execution Time     : %d seconds\n",
           current_time);

    printf("Total Energy             : %.2f J\n",
           total_energy);

    printf("Total Energy             : %.6f kWh\n",
           energy_kwh);

    printf("Carbon Emission          : %.6f gCO2\n",
           total_carbon);

    printf("Deadlines Met            : %d/%d\n",
           deadlines_met, n);

    printf("Deadlines Missed         : %d/%d\n",
           n - deadlines_met, n);

    printf("============================================\n");

    return 0;
}

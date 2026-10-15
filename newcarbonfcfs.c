#include <stdio.h>
 
#define MAX 100
#define MAX_TIME 10000
 
#define POWER 50.0
#define JOULES_PER_KWH 3600000.0
 
#define CARBON_THRESHOLD 500.0
 
struct Process {
    int pid;
    int arrival;
    int burst;
    int deadline;
 
    int start;
    int completion;
    int waiting;
    int turnaround;
 
    int deferred;
};
 
int main() {
 
    struct Process p[MAX];
    double carbon[MAX_TIME];
 
    int n;
    int i, t;
 
    int current_time = 0;
    int total_burst = 0;
 
    double total_waiting = 0;
    double total_turnaround = 0;
 
    double total_energy = 0;
    double total_carbon = 0;
 
    double energy_kwh;
    double cpu_utilization;
 
    int deadlines_met = 0;
 
    for (i = 0; i < MAX_TIME; i++) {
        carbon[i] = 0;
    }
 
    FILE *fp = fopen("carbon.txt", "r");
 
    if (fp == NULL) {
        printf("\nERROR: carbon.txt not found.\n");
        printf("Make sure carbon.txt is in the same folder.\n");
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
    printf("        CARBON-AWARE FCFS SCHEDULER\n");
    printf("============================================\n");
 
    printf("\nCarbon Threshold : %.0f gCO2/kWh\n",
           CARBON_THRESHOLD);
 
    printf("CPU Power        : %.0f W\n",
           POWER);
 
    printf("\nEnter number of processes: ");
    scanf("%d", &n);
 
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
 
    for (i = 0; i < n; i++) {
 
        if (current_time < p[i].arrival) {
            current_time = p[i].arrival;
        }
 
        int proposed_start = current_time;
        int green_start = -1;
 
        if (proposed_start < MAX_TIME &&
            carbon[proposed_start] > CARBON_THRESHOLD) {
 
            int candidate;
 
            for (candidate = proposed_start;
                 candidate + p[i].burst <= p[i].deadline;
                 candidate++) {
 
                int feasible = 1;
 
                for (t = candidate;
                     t < candidate + p[i].burst;
                     t++) {
 
                    if (t >= MAX_TIME ||
                        carbon[t] > CARBON_THRESHOLD) {
 
                        feasible = 0;
                        break;
                    }
                }
 
                if (feasible) {
                    green_start = candidate;
                    break;
                }
            }
        }
 
        if (green_start != -1) {
 
            p[i].start = green_start;
 
            p[i].deferred =
                green_start - proposed_start;
 
            printf("\nP%d: Carbon = %.0f gCO2/kWh",
                   p[i].pid,
                   carbon[proposed_start]);
 
            printf(" -> deferred by %d seconds",
                   p[i].deferred);
 
        } else {
 
            p[i].start = proposed_start;
            p[i].deferred = 0;
 
            printf("\nP%d: Executed immediately",
                   p[i].pid);
        }
 
        p[i].completion =
            p[i].start + p[i].burst;
 
        p[i].waiting =
            p[i].start - p[i].arrival;
 
        p[i].turnaround =
            p[i].completion - p[i].arrival;
 
        if (p[i].completion <= p[i].deadline) {
            deadlines_met++;
        }
 
        total_waiting += p[i].waiting;
        total_turnaround += p[i].turnaround;
        total_burst += p[i].burst;
 
        for (t = p[i].start;
             t < p[i].completion;
             t++) {
 
            total_energy += POWER;
 
            if (t >= 0 && t < MAX_TIME) {
 
                double energy_this_second =
                    POWER / JOULES_PER_KWH;
 
                total_carbon +=
                    energy_this_second * carbon[t];
            }
        }
 
        current_time = p[i].completion;
    }
 
    energy_kwh =
        total_energy / JOULES_PER_KWH;
 
    if (current_time > 0) {
 
        cpu_utilization =
            ((double)total_burst / current_time)
            * 100.0;
 
    } else {
 
        cpu_utilization = 0;
    }
 
    printf("\n\n============================================\n");
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
    printf("          CARBON-AWARE PROCESS RESULTS\n");
    printf("============================================\n");
 
    printf("PID\tAT\tBT\tDL\tST\tCT\tWT\tTAT\tDEF\n");
    printf("------------------------------------------------------------\n");
 
    for (i = 0; i < n; i++) {
 
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].arrival,
               p[i].burst,
               p[i].deadline,
               p[i].start,
               p[i].completion,
               p[i].waiting,
               p[i].turnaround,
               p[i].deferred);
    }
 
    printf("\n============================================\n");
    printf("        CARBON-AWARE FCFS METRICS\n");
    printf("============================================\n");
 
    printf("Average Waiting Time     : %.2f\n",
           total_waiting / n);
 
    printf("Average Turnaround Time  : %.2f\n",
           total_turnaround / n);
 
    printf("CPU Utilization          : %.2f%%\n",
           cpu_utilization);
 
    printf("CPU Power                : %.2f W\n",
           POWER);
 
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

# Initial Version — Carbon-Aware CPU Scheduling

This folder preserves the initial simulation-stage artifacts from the project
*Energy-Optimal Task Scheduling Using Carbon-Aware Computing*, before the later
AWS/VM experiments.

## Contents
- `graphs/`: the original five workload-comparison plots and two aggregate comparison plots.
- `docs/`: research paper, experimental results report, technical design, synthetic carbon trace documentation, and controlled workload documentation.

## Initial experiment scope
- Conventional FCFS vs. carbon-aware FCFS
- Five workload configurations
- Synthetic time-varying carbon-intensity trace
- Carbon threshold: 500 gCO2/kWh
- Constant CPU power assumption: 50 W

## Interpretation
These are initial discrete-event simulation results, not measurements of real AWS power
or real-world carbon savings. Under the constant-power model, the scheduling policy can
change estimated carbon emissions by changing *when* work executes, while modeled energy
for the same executed CPU work remains unchanged. Waiting and turnaround times can increase
because carbon-aware FCFS may defer work.

Keep these artifacts separate from later AWS/VM results so the project history remains clear.

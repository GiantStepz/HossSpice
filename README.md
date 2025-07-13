## HossSpice - simple parser/language for circuit simulation

I started this just for fun but I'll likely keep working on it as there are many things I want to implement. Currently it is capable of describing reusable analog and digital circuit components and simulations.

The goal is to offer an intuitive and readable syntax for circuit modeling, simulation setup, and netlist generation. 

I view it as something you would use for quick prototyping where you put a .hsp (.hoss maybe?) script together describing the circuit + testbench, 
then execute it from the command line, and HossSpice will:
- Compile your script into a SPICE-compatible netlist
- Launch the simulator of your choice
- Output waveforms from the simulation

## Current features:
- Parse .hsp files w/ basic circuit definitions
- Define voltage sources, resistors, capacitors, and ground nodes
- Describe and run transient simulations
- Generate spice .cir netlists
- Macro support w/ net substitution for reusable components
- Simple model generation
- DC Operating Point Analysis

## Try:
**Linux**
- Just clone the repo and run the included script:
```bash
./run.sh
```
**All** 
```bash
git clone https://github.com/GiantStepz/HossSpice.git
cd HossSpice
make
./output/hoss examples/basic_rc.hsp
cat output/netlist.cir
```

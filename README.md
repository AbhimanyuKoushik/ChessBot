# ChessEngine

A high-performance chess engine written in C++, engineered from first principles with a focus on systems-level memory efficiency and search optimization.

## Engine Details
- **Strength:** 2000 ELO.
- **Protocol:** Fully compliant with the Universal Chess Interface (UCI) protocol.

## Technical Architecture
- **Board Representation:** 64-bit Unsigned Integer Bitboards.
- **Move Generation:** Pseudo-legal move generation utilizing Magic Bitboards for sliding piece kinematics.
- **Search Algorithm:**
  - Iterative Deepening with Fail-Soft Alpha-Beta Pruning.
  - Quiescence Search strictly filtered for captures and promotions to guarantee OS stack safety.
  - Global Transposition Tables (Zobrist Hashing) for branch pruning.
- **Move Ordering:**
  - MVV-LVA (Most Valuable Victim - Least Valuable Attacker) scoring with a memory-safe En Passant bypass.
  - History Heuristic for quiet moves, utilizing depth-squared bonuses.
- **Evaluation:** Tapered Piece-Square Tables (PSTs) bridging opening and endgame phases.

## Prerequisites
This engine is developed and tested on **Ubuntu 24.04** (Linux-based systems). You will need the following toolchain:

- **GCC/G++ 13+** (or any compiler with robust C++20 support)
- **CMake 3.15+**
- **Make**
- **Git**

Install the dependencies on Debian/Ubuntu via:
```bash
sudo apt update
sudo apt install build-essential cmake git
```
## Build Instructions
The build process is automated via a shell script. From the project root, run:
```shell
chmod +x ./scripts/build.sh
./scripts/build.sh
```
Upon successful compilation the executable will be located at /build/bin/ChessEngine.

## Usage
The engine follows the standard UCI protocol. To use it, simply load the /build/bin/ChessEngine binary into any UCI-compatible chess GUI, such as Cute Chess or Arena.

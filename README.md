# ChessEngine

A high-performance, UCI compliant chess engine written in C++

## Project goals
- **Strength:** Target 2000+ FIDE rating.
- **Protocol:** Full support for UCI protocol

## Technical Architecture
- **Board Representation:** Bitboard (64-bit Unsigned integers)
- **Move Generation:** Magic bitboard for sliding pieces
- **Search:** Alpha-beta pruning, Quiescence search Transposition tables
- **Evaluation:** Tapered Piece-Square Tables (PSTs)

## Prerequisites
This project is developed and tested on **Ubuntu 24.04** . You will need the following

- **GCC/G++ 13+** (or any compiler with C++20 support)
- **CMake 3.15+**
- **Make**
- **Git**

You can install these via:
```bash
sudo apt update
sudo apt install build-essential cmake git
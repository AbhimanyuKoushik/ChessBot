#pragma once

#include <bitboards.hpp>
#include <cstdint>
#include <types.hpp>

// #########################################
// Occupancy Mask generation
// #########################################

constexpr Bitboard rook_occupancy_mask(Square sq) {
  Square occupancy_sq;
  int rank, file;
  Bitboard occupancy_mask = 0ULL;

  // Original code, this is indexed 1, basically of traditional ranks and files
  // are We can refactor make some modifications so that will be faster we are
  // doing subtraction for ranks and addition for files on top, and then
  // removing them by doing same thing in for loops, might as well not do both

  // Understandable code
  /*
  rank = (8 - (sq / 8));
  file = (1 + (sq % 8));

  for (int i = 2; i < rank; i++) {
    occupancy_sq = static_cast<Square>((8 - i) * 8 + (file - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = rank + 1; i < 8; i++) {
    occupancy_sq = static_cast<Square>((8 - i) * 8 + (file - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 2; i < file; i++) {
    occupancy_sq = static_cast<Square>((8 - rank) * 8 + (i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = file + 1; i < 8; i++) {
    occupancy_sq = static_cast<Square>((8 - rank) * 8 + (i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }
  */

  // Refactored code
  rank = (sq / 8);
  file = (sq % 8);

  for (int i = rank + 1; i < 7; i++) {
    occupancy_sq = static_cast<Square>(i * 8 + file);
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; i < rank; i++) {
    occupancy_sq = static_cast<Square>(i * 8 + file);
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; i < file; i++) {
    occupancy_sq = static_cast<Square>(rank * 8 + i);
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = file + 1; i < 7; i++) {
    occupancy_sq = static_cast<Square>(rank * 8 + i);
    set_bit(occupancy_mask, occupancy_sq);
  }

  return occupancy_mask;
}

constexpr Bitboard bishop_occupancy_mask(Square sq) {
  Square occupancy_sq;
  int rank, file;
  Bitboard occupancy_mask = 0ULL;

  // Similar to rook_occupancy_mask, the understandable code is indexed 1, we
  // will refactor it to remove unnecessary additions and subtractions

  // Understandable code
  /*
  rank = (8 - (sq / 8));
  file = (1 + (sq % 8));

  for (int i = 1; (rank - i > 1) && (file - i > 1); i++) {
    occupancy_sq = static_cast<Square>((8 - rank + i) * 8 + (file - i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 8) && (file + i < 8); i++) {
    occupancy_sq = static_cast<Square>((8 - rank - i) * 8 + (file + i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank - i > 1) && (file + i < 8); i++) {
    occupancy_sq = static_cast<Square>((8 - rank + i) * 8 + (file + i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 8) && (file - i > 1); i++) {
    occupancy_sq = static_cast<Square>((8 - rank - i) * 8 + (file - i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }
  */

  // Refactored code
  rank = sq / 8;
  file = sq % 8;

  for (int i = 1; (rank - i > 0) && (file - i > 0); i++) {
    occupancy_sq = static_cast<Square>((rank - i) * 8 + (file - i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 7) && (file + i < 7); i++) {
    occupancy_sq = static_cast<Square>((rank + i) * 8 + (file + i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank - i > 0) && (file + i < 7); i++) {
    occupancy_sq = static_cast<Square>((rank - i) * 8 + (file + i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 7) && (file - i > 0); i++) {
    occupancy_sq = static_cast<Square>((rank + i) * 8 + (file - i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  return occupancy_mask;
}

// storing the occupancy mask in a lookup table so that its fast later on
constexpr std::array<Bitboard, 64> store_rook_occupancy_masks() {
  std::array<Bitboard, 64> rook_occupancy_table;
  for (int sq = 0; sq < 64; sq++) {
    rook_occupancy_table[sq] = rook_occupancy_mask(static_cast<Square>(sq));
  }
  return rook_occupancy_table;
}

constexpr std::array<Bitboard, 64> store_bishop_occupancy_masks() {
  std::array<Bitboard, 64> bishop_occupancy_table;
  for (int sq = 0; sq < 64; sq++) {
    bishop_occupancy_table[sq] = bishop_occupancy_mask(static_cast<Square>(sq));
  }
  return bishop_occupancy_table;
}

inline constexpr std::array<Bitboard, 64> kRookOccupancyTable =
    store_rook_occupancy_masks();
inline constexpr std::array<Bitboard, 64> kBishopOccupancyTable =
    store_bishop_occupancy_masks();

// #########################################
// For loop Attack Mask generation
// #########################################

constexpr Bitboard rook_attack_gen(Bitboard occupied_squares, Square sq) {
  Square attack_sq;
  int rank, file;
  Bitboard attack_mask = 0ULL;

  rank = (sq / 8);
  file = (sq % 8);

  for (int i = rank + 1; i <= 7; i++) {
    attack_sq = static_cast<Square>(i * 8 + file);
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  for (int i = rank - 1; i >= 0; i--) {
    attack_sq = static_cast<Square>(i * 8 + file);
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  for (int i = file - 1; i >= 0; i--) {
    attack_sq = static_cast<Square>(rank * 8 + i);
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  for (int i = file + 1; i <= 7; i++) {
    attack_sq = static_cast<Square>(rank * 8 + i);
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  return attack_mask;
}

constexpr Bitboard bishop_attack_gen(Bitboard occupied_squares, Square sq) {
  Square attack_sq;
  int rank, file;
  Bitboard attack_mask = 0ULL;

  rank = (sq / 8);
  file = (sq % 8);

  for (int i = 1; (rank - i >= 0) && (file - i >= 0); i++) {
    attack_sq = static_cast<Square>((rank - i) * 8 + (file - i));
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  for (int i = 1; (rank + i <= 7) && (file + i <= 7); i++) {
    attack_sq = static_cast<Square>((rank + i) * 8 + (file + i));
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  for (int i = 1; (rank - i >= 0) && (file + i <= 7); i++) {
    attack_sq = static_cast<Square>((rank - i) * 8 + (file + i));
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  for (int i = 1; (rank + i <= 7) && (file - i >= 0); i++) {
    attack_sq = static_cast<Square>((rank + i) * 8 + (file - i));
    set_bit(attack_mask, attack_sq);
    if (get_bit(occupied_squares, attack_sq)) break;
  }

  return attack_mask;
}

// ###############################################
// Magic Numbers and initializing the hash table
// ###############################################

inline constexpr std::array<uint64_t, 64> rook_magic_numbers = {
    0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,
    0x100081001000420ULL,  0x200020010080420ULL,  0x3001c0002010008ULL,
    0x8480008002000100ULL, 0x2080088004402900ULL, 0x800098204000ULL,
    0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL,
    0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL,
    0x801000060821100ULL,  0x80044006422000ULL,   0x100808020004000ULL,
    0x12108a0010204200ULL, 0x140848010000802ULL,  0x481828014002800ULL,
    0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,
    0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,
    0x20100080080080ULL,   0x2000a00200410ULL,    0x20080800400ULL,
    0x80088400100102ULL,   0x80004600042881ULL,   0x4040008040800020ULL,
    0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL,
    0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,
    0x200046502000484ULL,  0x480400080088020ULL,  0x1000422010034000ULL,
    0x30200100110040ULL,   0x100021010009ULL,     0x2002080100110004ULL,
    0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL,
    0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL,
    0x2060820c0120200ULL,  0x1001004080100ULL,    0x20c020080040080ULL,
    0x2935610830022400ULL, 0x44440041009200ULL,   0x280001040802101ULL,
    0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
    0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,
    0x1004081002402ULL};

inline constexpr std::array<uint64_t, 64> bishop_magic_numbers = {
    0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,
    0x108060845042010ULL,  0x581104180800210ULL,  0x2112080446200010ULL,
    0x1080820820060210ULL, 0x3c0808410220200ULL,  0x4050404440404ULL,
    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,
    0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,
    0x801010402020200ULL,  0x400210c3880100ULL,   0x404022024108200ULL,
    0x810018200204102ULL,  0x4002801a02003ULL,    0x85040820080400ULL,
    0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
    0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,
    0x20080002081110ULL,   0x4001001021004000ULL, 0x800040400a011002ULL,
    0xe4004081011002ULL,   0x1c004001012080ULL,   0x8004200962a00220ULL,
    0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
    0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL,
    0x42008c0340209202ULL, 0x209188240001000ULL,  0x400408a884001800ULL,
    0x110400a6080400ULL,   0x1840060a44020800ULL, 0x90080104000041ULL,
    0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
    0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL,
    0x300000261044000aULL, 0x802241102020002ULL,  0x20906061210001ULL,
    0x5a84841004010310ULL, 0x4010801011c04ULL,    0xa010109502200ULL,
    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
    0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL,
    0x4010011029020020ULL};

inline constexpr std::array<uint8_t, 64> nb_squares_rook_moves = {
    12, 11, 11, 11, 11, 11, 11, 12,  // Rank 8
    11, 10, 10, 10, 10, 10, 10, 11,  // Rank 7
    11, 10, 10, 10, 10, 10, 10, 11,  // Rank 6
    11, 10, 10, 10, 10, 10, 10, 11,  // Rank 5
    11, 10, 10, 10, 10, 10, 10, 11,  // Rank 4
    11, 10, 10, 10, 10, 10, 10, 11,  // Rank 3
    11, 10, 10, 10, 10, 10, 10, 11,  // Rank 2
    12, 11, 11, 11, 11, 11, 11, 12   // Rank 1
};

inline constexpr std::array<uint8_t, 64> nb_squares_bishop_moves = {
    6, 5, 5, 5, 5, 5, 5, 6,  // Rank 8
    5, 5, 5, 5, 5, 5, 5, 5,  // Rank 7
    5, 5, 7, 7, 7, 7, 5, 5,  // Rank 6
    5, 5, 7, 9, 9, 7, 5, 5,  // Rank 5
    5, 5, 7, 9, 9, 7, 5, 5,  // Rank 4
    5, 5, 7, 7, 7, 7, 5, 5,  // Rank 3
    5, 5, 5, 5, 5, 5, 5, 5,  // Rank 2
    6, 5, 5, 5, 5, 5, 5, 6   // Rank 1
};

constexpr std::array<std::array<Bitboard, 4096>, 64>
initialize_rook_magic_luts() {
  std::array<std::array<Bitboard, 4096>, 64> rook_attack_table{};
  Bitboard current_occupancy_mask;
  Bitboard current_occupied_squares;
  int nb_indices, hash_index;

  for (int sq = 0; sq < 64; sq++) {
    current_occupancy_mask = kRookOccupancyTable[sq];
    nb_indices = 1 << (nb_squares_rook_moves[sq]);  // 2 power number of squares
                                                    // rook can move
    current_occupied_squares = 0ULL;

    for (int i = 0; i < nb_indices; i++) {
      hash_index = (current_occupied_squares * rook_magic_numbers[sq]) >>
                   (64 - nb_squares_rook_moves[sq]);
      rook_attack_table[sq][hash_index] =
          rook_attack_gen(current_occupied_squares, static_cast<Square>(sq));

      current_occupied_squares =
          (current_occupied_squares - current_occupancy_mask) &
          current_occupancy_mask;
    }
  }

  return rook_attack_table;
}

constexpr std::array<std::array<Bitboard, 512>, 64>
initialize_bishop_magic_luts() {
  std::array<std::array<Bitboard, 512>, 64> bishop_attack_table{};
  Bitboard current_occupancy_mask;
  Bitboard current_occupied_squares;
  int nb_indices, hash_index;

  for (int sq = 0; sq < 64; sq++) {
    current_occupancy_mask = kBishopOccupancyTable[sq];
    nb_indices = 1 << (nb_squares_bishop_moves[sq]);
    current_occupied_squares = 0ULL;

    for (int i = 0; i < nb_indices; i++) {
      hash_index = (current_occupied_squares * bishop_magic_numbers[sq]) >>
                   (64 - nb_squares_bishop_moves[sq]);
      bishop_attack_table[sq][hash_index] =
          bishop_attack_gen(current_occupied_squares, static_cast<Square>(sq));

      current_occupied_squares =
          (current_occupied_squares - current_occupancy_mask) &
          current_occupancy_mask;
    }
  }

  return bishop_attack_table;
}

inline constexpr std::array<std::array<Bitboard, 4096>, 64> kRookAttackTable =
    initialize_rook_magic_luts();
inline constexpr std::array<std::array<Bitboard, 512>, 64> kBishopAttackTable =
    initialize_bishop_magic_luts();

// #########################################
// For getting the final attack mask
// #########################################

inline Bitboard RookAttacks(Bitboard full_occupancies, Square sq) {
  Bitboard rook_occupancy = full_occupancies & kRookOccupancyTable[sq];
  int hash_index = (rook_occupancy * rook_magic_numbers[sq]) >>
                   (64 - nb_squares_rook_moves[sq]);

  return kRookAttackTable[sq][hash_index];
}

inline Bitboard BishopAttacks(Bitboard full_occupancies, Square sq) {
  Bitboard bishop_occupancy = full_occupancies & kBishopOccupancyTable[sq];
  int hash_index = (bishop_occupancy * bishop_magic_numbers[sq]) >>
                   (64 - nb_squares_bishop_moves[sq]);

  return kBishopAttackTable[sq][hash_index];
}

inline Bitboard QueenAttacks(Bitboard full_occupancies, Square sq) {
  return (RookAttacks(full_occupancies, sq) |
          BishopAttacks(full_occupancies, sq));
}
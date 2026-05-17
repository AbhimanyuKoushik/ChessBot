#include <iostream>
#include <movedef.hpp>
#include <types.hpp>

bool test_movedef() {
  Move mv = encode_move(E4, E5, ENPASS_CAP);
  if (mv != 22308) {
    print_move(mv);
    std::cout << "The source square is E4, target square is E5, flag is "
                 "Enpassent capture is encoded in a wrong manner.\n";
    return false;
  }

  mv = encode_move(A2, A3, CAPTURE);
  if (mv != 18992) {
    print_move(mv);
    std::cout << "The source square is E4, target square is E5, flag is "
                 "Enpassent capture is encoded in a wrong manner.\n";
    return false;
  }

  return true;
}
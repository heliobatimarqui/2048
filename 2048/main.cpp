//
//  main.cpp
//  2048
//
//  Created by Hélio Nunes Santos on 2/3/23.
//

#include <iostream>
#include "2048.hpp"

int main(int argc, const char * argv[]) {
    Gamestate g;
    
    std::size_t i = 0;
    
    for(auto row : g.get_game_board()) {
        for(auto x : row) {
            std::cout << x << " ";
            ++i;
            if(i == 4) {
                i = 0;
                std::cout << "\n";
            }
        }
    }
}

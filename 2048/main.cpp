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

    while(!g.game_over()) {

        char c;
        
        std::cin >> c;
        
        c = std::tolower(c);
        
        if(c == 'd') {
            g.move_in_direction(MovementDirection::RIGHT);
        }
        
        if(c == 'a') {
            g.move_in_direction(MovementDirection::LEFT);
        }
        
        if(c == 's') {
            g.move_in_direction(MovementDirection::DOWN);
        }
        
        if(c == 'w') {
            g.move_in_direction(MovementDirection::UP);
        }
        
        std::size_t i = 0;
        
        for(auto row : g.get_game_board()) {
            for(auto x : row) {
                std::cout << x << " ";
                ++i;
                if(i == row.size()) {
                    i = 0;
                    std::cout << "\n";
                }
            }
            std::cout << "\n";
        }
    }
}

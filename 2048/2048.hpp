//
//  2048.hpp
//  2048
//
//  Created by Hélio Nunes Santos on 2/3/23.
//

#ifndef _048_hpp
#define _048_hpp

#include <cstdint>
#include <array>
#include <vector>
#include <random>

enum class MovementDirection {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

const uint16_t EMPTY_BOX = 0;

class Gamestate {
public:
    using GameBoard = std::array<std::array<uint16_t,4>, 4>;
private:
    GameBoard m_gameboard;
    uint16_t max_value;
    std::mt19937 m_mt {std::random_device{}()};
    
    std::vector<uint16_t*> get_empty_boxes() {
        std::vector<uint16_t*> empty_boxes;
        for(auto& row : m_gameboard) {
            for(auto box = row.begin(); box != row.end(); ++box) {
                if(*box == EMPTY_BOX) {
                    empty_boxes.push_back(&(*box));
                }
            }
        }
        
        return empty_boxes;
    }
    
    // Inserts a number (by default 2) in a random box of the game table
    void insert_number_in_random_box(uint16_t number = 2) {
        auto empty_boxes = get_empty_boxes();
       
        if(empty_boxes.size() != 0) {
            std::uniform_int_distribution<std::size_t> index_distribution(0, empty_boxes.size() - 1);
            std::size_t index = index_distribution(m_mt);
            *empty_boxes[index] = 2;
        }
    }
    
public:
    Gamestate() {
        for(auto& row : m_gameboard) {
            for(auto& val : row) {
                val = EMPTY_BOX;
            }
        }
        
        insert_number_in_random_box();
    }
    
    ~Gamestate() = default;
    
    bool is_victorious() const {
        return max_value == 2048;
    }
    
    const GameBoard& get_game_board() const {
        return m_gameboard;
    }
    
};

#endif /* _048_hpp */

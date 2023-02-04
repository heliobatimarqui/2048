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
#include <iostream>

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

    // To make it easier, we only consolidate from left to right, so the calling code
    // handles placing the values in the correct order (positive horizontal,
    // negative horizontal, positive vertical and negative vertical)
    void consolidate2(const std::vector<uint16_t*>& values) {
        std::vector<uint16_t> temp;
        for(auto v : values) {
            if(*v != EMPTY_BOX) {
                temp.push_back(*v);
                *v = EMPTY_BOX;
            }
        }
        
        if(temp.size() == 0)
            return;
        
        auto values_it = values.rbegin();
        for(auto it = temp.rbegin(); it != temp.rend(); ++it) {
            auto current = *it;
            
            auto next = it == temp.rend() - 1 ? 1 : *(it + 1);
            if(current != EMPTY_BOX) {
                if(current == next) {
                    *it *= 2;
                    *(it + 1) = EMPTY_BOX;
                }
                *(*values_it) = *it;
                ++values_it;
            }
        }
    }
    
   
    void consolidate(std::array<uint16_t, 4>& values) {
        std::vector<uint16_t> values_stack;
        for(auto& v : values) {
            if(v != EMPTY_BOX) {
                values_stack.push_back(v);
                v = EMPTY_BOX;
            }
        }
        
        if(values_stack.size() == 0)
            return;

        // We can use only one loop and there is no need to do boundary checks given that
        // values_stack will never have more elements than value
        auto values_it = values.rbegin();
        
        for(auto it = values_stack.rbegin(); it != values_stack.rend(); ++it) {
            auto current = *it;
            
            auto next = it == values_stack.rend() - 1 ? 1 : *(it + 1);
            if(current != EMPTY_BOX) {
                if(current == next) {
                    *it *= 2;
                    *(it + 1) = EMPTY_BOX;
                }
                *values_it = *it;
                ++values_it;
            }
        }
    };
    
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
    
    bool game_over() const {
        // We will use it for the last row
        std::array<uint16_t, 4> mock = {1, 1, 1 ,1};
        for(auto it = m_gameboard.begin(); it != m_gameboard.end(); ++it) {
            auto& current_row = *it;
            auto& next_row = (it == m_gameboard.end() - 1) ? mock : *(it + 1);
            
            // Tests the current box, the one beside to the right and the one below
            for(size_t i = 0; i < 3; ++i) {
                if(current_row[i] == 2048)
                    return true;
                if(current_row[i] == next_row[i])
                    return false;
                if(current_row[i] == current_row[i + 1])
                    return false;
                if(current_row[i] == EMPTY_BOX)
                    return false;
            }
        }
        
        // If there are no empty boxes and no boxes with an equal value beside,
        // it can only mean that the board is full and thus no moves are possible.
        return true;
    }
    
    void move_in_direction(MovementDirection direction) {
        if(!game_over()) {
            
            // I thought on using a switch statement, but given the amount of variables
            // that I would create and thus curly braces would be used all over the place,
            // I have opted to use if statements
            
            // We will be storing
            std::vector<uint16_t*> to_consolidate;
            
            // Easy case, just store the pointers in order and we are good to go
            if(direction == MovementDirection::RIGHT) {
                for(auto& row : m_gameboard) {
                    for(auto& v : row) {
                        to_consolidate.push_back(&v);
                    }
                    consolidate2(to_consolidate);
                    to_consolidate.clear();
                }
            }
            
            // Iterators (well, reversed iterators) make this a breeze!
            if(direction == MovementDirection::LEFT) {
                for(auto& row : m_gameboard) {
                    for(auto it = row.rbegin(); it != row.rend(); ++it) {
                        to_consolidate.push_back(&(*it));
                    }
                    consolidate2(to_consolidate);
                    to_consolidate.clear();
                }
            }
            
            // Not so hard, just a little bit of thought... actually I started using a
            // temporary vector storing pointers due to this case
            if(direction == MovementDirection::DOWN) {
                for(std::size_t i = 0; i < m_gameboard[0].size(); ++i) {
                    for(std::size_t j = 0; j < m_gameboard.size(); ++j) {
                        to_consolidate.push_back(&m_gameboard[j][i]);
                    }
                    consolidate2(to_consolidate);
                    to_consolidate.clear();
                }
            }
            
            if(direction == MovementDirection::UP) {
                for(std::size_t i = 0; i < m_gameboard[0].size(); ++i) {
                    for(std::size_t j = 0; j < m_gameboard.size(); ++j) {
                        to_consolidate.push_back(&m_gameboard[m_gameboard.size() - j - 1][i]);
                    }
                    consolidate2(to_consolidate);
                    to_consolidate.clear();
                }
            }
            
            /*
            // Easy case, we can consolidate directly.
            if(direction == MovementDirection::RIGHT) {
                for(auto& row : m_gameboard) {
                    consolidate(row);
                }
            }
        
            // A little bit more complicated, but a temporary row comes to the rescue
            if(direction == MovementDirection::LEFT) {
                for(auto& row : m_gameboard) {
                    std::array<uint16_t, 4> temp_row;
                    auto temp_it = temp_row.begin();
                    for(auto it = row.rbegin(); it != row.rend(); ++it) {
                        *temp_it = *it;
                        ++temp_it;
                    }
                    consolidate(temp_row);
                    temp_it = temp_row.begin();
                    for(auto it = row.rbegin(); it != row.rend(); ++it) {
                        *it = *temp_it;
                        ++temp_it;
                    }
                }
            }
            */
            insert_number_in_random_box();
        }
    }
    
    const GameBoard& get_game_board() const {
        return m_gameboard;
    }
    
};

#endif /* _048_hpp */

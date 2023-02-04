//
//  2048.hpp
//  2048
//
//  Created by Hélio Nunes Santos on 2/3/23.
//

#ifndef _048_hpp
#define _048_hpp

#include <cstdint>
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
    // By using vectors we can have a game board with different sizes.
    using GameBoard = std::vector<std::vector<uint16_t>>;
private:
    GameBoard m_gameboard;
    uint16_t max_value;
    std::mt19937 m_mt {std::random_device{}()};
    
    // Collects which boxes are empty and fills a vector with pointers to them.
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
    void consolidate(const std::vector<uint16_t*>& values) {
        // We have to skip all the empty boxes, so that values are "beside each other"
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
            // We skip empties, and doing so we also guarantee that all values will be beside
            // each other after consolidating.
            if(current != EMPTY_BOX) {
                if(current == next) {
                    *it *= 2;
                    // Since what matters is only the value beside, after using it we mark it
                    // as empty, so we don't end up wrongly considering it and the value beside
                    // to it.
                    *(it + 1) = EMPTY_BOX;
                }
                *(*values_it) = *it;
                ++values_it;
            }
        }
    }
    
public:
    Gamestate(std::size_t board_side_size = 4) {
        // Should I throw an error?
        if(board_side_size < 3) {
            board_side_size = 3;
        }
    
        for(auto i = 0; i < board_side_size; ++i) {
            m_gameboard.push_back(std::vector<uint16_t> (board_side_size, EMPTY_BOX));
        }
    
        insert_number_in_random_box();
        insert_number_in_random_box();
    }
    
    ~Gamestate() = default;
    
    bool game_over() const {
        // We will use it for the last row... no box can contain a one, thus it will never
        // be equal to any value inside any box. That way we can use the same logic for every
        // row.
        const std::vector<uint16_t> mock (m_gameboard[0].size(), 1);
    
        for(auto it = m_gameboard.begin(); it != m_gameboard.end(); ++it) {
            const auto& current_row = *it;
            const auto& next_row = (it == m_gameboard.end() - 1) ? mock : *(it + 1);
            
            // Tests the current box, the one beside to the right and the one below
            for(std::size_t i = 0; i < m_gameboard[0].size(); ++i) {
                if(current_row[i] == 2048)
                    return true;
                if(current_row[i] == next_row[i])
                    return false;
                if(i != m_gameboard[0].size() - 1) {
                    if(current_row[i] == current_row[i + 1])
                        return false;
                }
                if(current_row[i] == EMPTY_BOX)
                    return false;
            }
        }
        
        // If there are no empty boxes and no boxes with an equal value beside or below,
        // or any box containing 2048, it can only mean that the board is full and thus no
        // moves are possible, which is an (implicit in this code) game over condition.
        return true;
    }
    
    void move_in_direction(MovementDirection direction) {
        if(!game_over()) {
            // We will be storing pointers to values in an order as if they were all going
            // to be consolidated from left to right.
            std::vector<uint16_t*> ordered_values;
            
            switch(direction) {
                // Easy case, just store the pointers in order and we are good to go
                case MovementDirection::RIGHT:
                    for(auto& row : m_gameboard) {
                        for(auto& v : row) {
                            ordered_values.push_back(&v);
                        }
                        consolidate(ordered_values);
                        ordered_values.clear();
                    };
                    break;
                // Iterators (well, reverse iterators) make this a breeze!
                case MovementDirection::LEFT:
                    for(auto& row : m_gameboard) {
                        for(auto it = row.rbegin(); it != row.rend(); ++it) {
                            ordered_values.push_back(&(*it));
                        }
                        consolidate(ordered_values);
                        ordered_values.clear();
                    };
                    break;
                // Not so hard, just a little bit of thought... actually I started using a
                // temporary vector storing pointers due to this case
                case MovementDirection::DOWN:
                    for(std::size_t i = 0; i < m_gameboard[0].size(); ++i) {
                        for(std::size_t j = 0; j < m_gameboard.size(); ++j) {
                            ordered_values.push_back(&m_gameboard[j][i]);
                        }
                        consolidate(ordered_values);
                        ordered_values.clear();
                    }
                    break;
                case MovementDirection::UP:
                    for(std::size_t i = 0; i < m_gameboard[0].size(); ++i) {
                        for(std::size_t j = 0; j < m_gameboard.size(); ++j) {
                            ordered_values.push_back(&m_gameboard[m_gameboard.size() - j - 1][i]);
                        }
                        consolidate(ordered_values);
                        ordered_values.clear();
                    };
                    break;
                default:
                    return;
            }
            
            insert_number_in_random_box();
        }
    }
    
    const GameBoard& get_game_board() const {
        return m_gameboard;
    }
    
};

#endif /* _048_hpp */

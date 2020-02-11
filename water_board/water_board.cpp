// water_board.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>


#include <deque> // for bfs

// @return -1 for invalid board
int CalcContainedWater( const int *p_data, int num_columns, int num_rows )
{
    if ( num_columns <= 2 || num_rows <= 2 )
    {   // not enough cells to wall an inner cell
        return 0;
    }

    int const c_cell_count = num_columns * num_rows;

    int highest_wall = 0;
    for ( int i = 0; i < c_cell_count; ++i )
    {   // validate heights and find highest wall
        if ( p_data[ i ] < 0 ) {
            return -1;
        }
        if ( p_data[ i ] > highest_wall ) {
            highest_wall = p_data[ i ];
        }
    }

    // optimization, a cell that can't hold water in a layer can never hold water in a higher layer
    // a neighboring wall cell that can't hold water means we can't either
    bool* const holds_water = new bool[ c_cell_count ];
    for ( int i = 0; i < c_cell_count; ++i )
    {
        holds_water[ i ] = true;
    }

    bool* const visited = new bool[ c_cell_count ]; // for bfs
    // for bfs adjacencies
    int const c_num_offsets = 4;
    int const bfs_offsets[ c_num_offsets ] = { -1, 1, -num_columns, num_columns };

    // fill board, one layer at a time
    int volume = 0;
    for ( int fill_layer = 0; fill_layer < highest_wall; ++fill_layer )
    {
        // walk inner cells and test for ability to hold water for this layer
        // using a bfs search to attempt to escape the layer from given cell
        for ( int y = 1; y < num_rows - 1; ++y )
        {
            for ( int x = 1; x < num_columns - 1; ++x )
            {
                int const index = y * num_columns + x;
                if ( p_data[ index ] > fill_layer || !holds_water[ index ] )
                {   // cell has either been discarded or starts higher than current fill layer
                    continue;
                }

                for ( int i = 0; i < c_cell_count; ++i )
                {   // reset visited
                    visited[ i ] = false;
                }

                std::deque< int > queue;
                queue.push_back( index );
                visited[ index ] = true;
                while ( !queue.empty() )
                {
                    int const cell_index = queue.front();
                    int const cell_height = p_data[ cell_index ];
                    queue.pop_front();

                    if ( cell_index != index )
                    {
                        if ( cell_height > fill_layer )
                        {   // we hit a wall, this path is safe
                            continue;
                        }
                        if ( !holds_water[ cell_index ] )
                        {   // if adjacent can't hold water, neither can we
                            holds_water[ index ] = false;
                            break;
                        }

                        int const cell_x = cell_index % num_columns;
                        int const cell_y = cell_index / num_columns;
                        if ( cell_x <= 0 || cell_x >= ( num_columns - 1 ) ||
                             cell_y <= 0 || cell_y >= ( num_rows - 1 ) )
                        {   // path found board boundary, can't hold water
                            holds_water[ index ] = false;
                            break;
                        }
                    }

                    for ( int i = 0; i < c_num_offsets; ++i )
                    {   // push back valid adjacencies
                        int const adj_index = cell_index + bfs_offsets[ i ];
                        if ( !visited[ adj_index ] ) {
                            visited[ adj_index ] = true;
                            queue.push_back( adj_index );
                        }
                    }
                }

                if ( holds_water[ index ] ) {
                    //std::cout << x << " " << y << std::endl; // #todo
                    ++volume;
                }
            }
        }
    }

    delete [] visited;
    delete [] holds_water;
    return volume;
}


#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
#include <vector>

void test_board( int const row_len, int const expected, std::initializer_list< int > rows_in )
{
    auto const board = std::vector< int >{ rows_in };
    auto const column_len = static_cast< int >( board.size() / row_len );
    auto const volume = CalcContainedWater( board.data(), row_len, column_len );

    // print board and result
    for ( auto y = 0; y < column_len; ++y )
    {
        for ( auto x = 0; x < row_len; ++x )
        {
            auto const v = board[ y * row_len + x ];
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
    if ( volume >= 0 ) {
        std::cout << "= " << volume << "\n\n";
        assert( volume == expected );
    }
    else
    {   // don't stop the rest of the tests
        std::cout << "= invalid board" << "\n\n";
    }
}

int main()
{
    test_board( 5, 3, {
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1
    } );

    test_board( 3, 0, {
        1, 0, 1,
        1, 0, 1,
        1, 1, 1
    } );

    test_board( 3, 1, {
        0, 1, 0,
        1, 0, 1,
        0, 1, 0
    } );

    test_board( 3, 1, {
        0, 1, 0,
        1, -1, 1,
        0, 1, 0
    } );

    test_board( 5, 0, {
        1, 0, 1, 1, 1,
        1, 0, 1, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1
    } );

    test_board( 5, 1, {
        2, 1, 2, 2, 2,
        2, 1, 2, 0, 2,
        2, 1, 1, 1, 2,
        2, 2, 2, 2, 2
    } );

    test_board( 5, 5, {
        0, 2, 0, 2, 0,
        2, 0, 2, 0, 2,
        0, 2, 2, 1, 2,
        0, 0, 0, 2, 0
    } );

    test_board( 5, 6, {
        0, 2, 2, 2, 0,
        2, 0, 1, 0, 2,
        0, 2, 2, 1, 2,
        0, 0, 0, 2, 0
    } );

    test_board( 4, 2, {
        1, 2, 2, 2,
        2, 0, 1, 0,
        0, 1, 0, 1,
        2, 0, 1, 2
    } );

    test_board( 7, 6, {
        2, 2, 2, 2, 2, 2, 2,
        2, 0, 2, 1, 1, 0, 2,
        2, 2, 2, 2, 2, 2, 2
    } );

    test_board( 7, 3, {
        2, 2, 2, 2, 2, 2, 2,
        2, 0, 2, 1, 1, 0, 1,
        2, 2, 2, 2, 2, 2, 2
    } );

    test_board( 7, 15, {
        0, 5, 5, 5, 5, 5, 0,
        9, 0, 1, 2, 3, 4, 5,
        0, 5, 5, 5, 5, 5, 0
    } );
}

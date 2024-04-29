#include <iostream>
#include <assert.h>
#include <sstream>

#include "include/generalize.h"

#define LOG(x) if (ENABLE_DEBUG_LOGS) std::cout << x;

#define TEST(void_fn) \
    void_fn(); \
    LOG("TEST COMPLETED: " << #void_fn << std::endl);

using namespace dag;
using namespace logic;
using namespace karnaugh;

////////////////////////////////////////////
//////////////// UNIT TESTS ////////////////
////////////////////////////////////////////
#pragma region UNIT TESTS

void test_utils_filter(

)
{
    std::set<int> l_ints =
    {
        11, 12, 13, 14, 15,
        21, 22, 23, 24, 25,
        31, 32, 33, 34, 35,
        41, 42, 43, 44, 45,
        51, 52, 53, 54, 55,
        61, 62, 63, 64, 65,
        71, 72, 73, 74, 75,
    };

    std::set<int> l_filter_0 = filter(
        l_ints,
        [](
            int a_int
        )
        {
            return a_int < 20;
        }
    );

    std::set<int> l_filter_1 = filter(
        l_ints,
        [](
            int a_int
        )
        {
            return a_int < 30;
        }
    );

    std::set<int> l_filter_2 = filter(
        l_ints,
        [](
            int a_int
        )
        {
            return a_int < 40;
        }
    );
    
    assert(l_filter_0 == std::set({
        11, 12, 13, 14, 15,
    }));

    assert(l_filter_1 == std::set({
        11, 12, 13, 14, 15,
        21, 22, 23, 24, 25,
    }));

    assert(l_filter_2 == std::set({
        11, 12, 13, 14, 15,
        21, 22, 23, 24, 25,
        31, 32, 33, 34, 35,
    }));
    
}

void test_utils_cover(

)
{
    std::set<int> l_ints = {0, 1, 2, 3};

    std::map<double, std::set<int>> l_int_cover = cover(
        l_ints,
        [](
            const int& a_int
        )
        {
            std::set<double> l_result {
                double(a_int),
                double(a_int) / 2,
                double(a_int) / 3,
            };
            
            return l_result;
        }
    );

    assert(l_int_cover.size() == 8);

    assert(l_int_cover[0.0] == std::set({0}));
    assert(l_int_cover[1.0/3.0] == std::set({1}));
    assert(l_int_cover[0.5] == std::set({1}));
    assert(l_int_cover[2.0/3.0] == std::set({2}));
    assert(l_int_cover[1.0] == std::set({1, 2, 3}));
    assert(l_int_cover[1.5] ==  std::set({3}));
    assert(l_int_cover[2.0] == std::set({2}));
    assert(l_int_cover[3.0] == std::set({3}));

}

void test_utils_partition(

)
{
    std::set<int> l_ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::map<int, std::set<int>> l_int_cover = partition(
        l_ints,
        [](
            const int& a_int
        )
        {
            return a_int % 3;
        }
    );

    assert(l_int_cover.size() == 3);

    assert(l_int_cover[0] == std::set({0, 3, 6, 9}));
    assert(l_int_cover[1] == std::set({1, 4, 7, 10}));
    assert(l_int_cover[2] == std::set({2, 5, 8}));

}

void test_small_generalization_0(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = false;
    
    std::set<node> l_nodes;

    global_node_sink::bind(&l_nodes);
    
    std::set<input> l_zeroes =
    {
        { 0, 1, 1 },
        { 0, 1, 0 },
        { 0, 0, 0 }
    };

    std::set<input> l_ones =
    {
        { 1, 1, 1 },
        { 1, 0, 1 },
        { 0, 0, 1 }
    };
    
    /// Should create k-map equivalent to: a + b'c.
    const dag::node* l_dag = generalize(
        l_zeroes,
        l_ones
    );

    assert(evaluate(l_dag, {0, 0, 0}) == false);
    assert(evaluate(l_dag, {0, 0, 1}) == true);
    assert(evaluate(l_dag, {0, 1, 0}) == false);
    assert(evaluate(l_dag, {0, 1, 1}) == false); // Unknown
    assert(evaluate(l_dag, {1, 0, 0}) == true); // Unknown
    assert(evaluate(l_dag, {1, 0, 1}) == true);
    assert(evaluate(l_dag, {1, 1, 0}) == true);
    assert(evaluate(l_dag, {1, 1, 1}) == true);

    LOG(l_dag << std::endl);

    std::stringstream l_ss;

    l_ss << l_dag;

    assert(l_ss.str() == "([0]'[1]'[2]+[0])");
    
}

void test_small_generalization_1(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = false;
    
    std::set<node> l_nodes;

    global_node_sink::bind(&l_nodes);
    
    std::set<input> l_zeroes =
    {
        {0, 1, 0, 0},
        {0, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 1, 0},
        {0, 1, 1, 0}
    };

    std::set<input> l_ones =
    {
        {1, 0, 0, 0},
        {1, 1, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 1, 1},
        {1, 1, 1, 0},
        {1, 0, 1, 0}
    };

    const node* l_dag = generalize(
        l_zeroes,
        l_ones
    );

    assert(evaluate(l_dag, {0, 0, 0, 0}) == false);
    assert(evaluate(l_dag, {0, 0, 0, 1}) == false);
    assert(evaluate(l_dag, {0, 0, 1, 0}) == false);
    assert(evaluate(l_dag, {0, 0, 1, 1}) == false);
    assert(evaluate(l_dag, {0, 1, 0, 0}) == false);
    assert(evaluate(l_dag, {0, 1, 0, 1}) == false);
    assert(evaluate(l_dag, {0, 1, 1, 0}) == false);
    assert(evaluate(l_dag, {0, 1, 1, 1}) == false);

    assert(evaluate(l_dag, {1, 0, 0, 0}) == true);
    assert(evaluate(l_dag, {1, 0, 0, 1}) == true);
    assert(evaluate(l_dag, {1, 0, 1, 0}) == true);
    assert(evaluate(l_dag, {1, 0, 1, 1}) == true);
    assert(evaluate(l_dag, {1, 1, 0, 0}) == true);
    assert(evaluate(l_dag, {1, 1, 0, 1}) == true);
    assert(evaluate(l_dag, {1, 1, 1, 0}) == true);
    assert(evaluate(l_dag, {1, 1, 1, 1}) == true);
    
    LOG(l_dag << std::endl);

    std::stringstream l_ss;

    l_ss << l_dag;

    assert(l_ss.str() == "[0]");
    
}

void test_small_generalization_2(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = false;

    std::set<node> l_nodes;

    global_node_sink::bind(&l_nodes);
    
    std::set<input> l_zeroes =
    {
        {0, 0, 0, 0},
        {0, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 1, 1, 0},
        {1, 1, 1, 1},
    };

    std::set<input> l_ones =
    {
        {0, 0, 1, 1},
        {0, 1, 0, 0},
        {1, 1, 0, 1},
        {1, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 1, 1},
        {1, 0, 1, 0},
    };

    const node* l_dag = generalize(
        l_zeroes,
        l_ones
    );

    assert(evaluate(l_dag, {0, 0, 0, 0}) == false);
    assert(evaluate(l_dag, {0, 0, 0, 1}) == false);
    assert(evaluate(l_dag, {0, 0, 1, 0}) == true); // Unknown
    assert(evaluate(l_dag, {0, 0, 1, 1}) == true);
    assert(evaluate(l_dag, {0, 1, 0, 0}) == true);
    assert(evaluate(l_dag, {0, 1, 0, 1}) == false);
    assert(evaluate(l_dag, {0, 1, 1, 0}) == false);
    assert(evaluate(l_dag, {0, 1, 1, 1}) == false);

    assert(evaluate(l_dag, {1, 0, 0, 0}) == true); // Unknown
    assert(evaluate(l_dag, {1, 0, 0, 1}) == true);
    assert(evaluate(l_dag, {1, 0, 1, 0}) == true);
    assert(evaluate(l_dag, {1, 0, 1, 1}) == true);
    assert(evaluate(l_dag, {1, 1, 0, 0}) == true); // Unknown
    assert(evaluate(l_dag, {1, 1, 0, 1}) == true);
    assert(evaluate(l_dag, {1, 1, 1, 0}) == true);
    assert(evaluate(l_dag, {1, 1, 1, 1}) == false);
    
    LOG(l_dag << std::endl);

    std::stringstream l_ss;

    l_ss << l_dag;

    assert(l_ss.str() == "([0]'([1]'[2]+[1][2]'[3]')+[0]([1]'+[1]([2]'+[2][3]')))");
    
}

void test_small_generalization_3(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = false;

    std::set<node> l_nodes;

    global_node_sink::bind(&l_nodes);

    std::set<input> l_zeroes =
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 1},
        {1, 0, 1, 0},
    };

    std::set<input> l_ones =
    {
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 1, 1},
        {1, 0, 1, 1},
    };

    const node* l_dag = generalize(
        l_zeroes,
        l_ones
    );

    LOG(l_dag << std::endl);

    assert(evaluate(l_dag, {0, 0, 0, 0}) == true);
    assert(evaluate(l_dag, {0, 0, 0, 1}) == true); // Unknown
    assert(evaluate(l_dag, {0, 0, 1, 0}) == true); // Unknown
    assert(evaluate(l_dag, {0, 0, 1, 1}) == true); // Unknown
    assert(evaluate(l_dag, {0, 1, 0, 0}) == true);
    assert(evaluate(l_dag, {0, 1, 0, 1}) == true); // Unknown
    assert(evaluate(l_dag, {0, 1, 1, 0}) == false);
    assert(evaluate(l_dag, {0, 1, 1, 1}) == true); // Unknown

    assert(evaluate(l_dag, {1, 0, 0, 0}) == false);
    assert(evaluate(l_dag, {1, 0, 0, 1}) == false);
    assert(evaluate(l_dag, {1, 0, 1, 0}) == false);
    assert(evaluate(l_dag, {1, 0, 1, 1}) == true);
    assert(evaluate(l_dag, {1, 1, 0, 0}) == false);
    assert(evaluate(l_dag, {1, 1, 0, 1}) == true);  // Unknown
    assert(evaluate(l_dag, {1, 1, 1, 0}) == false); // Unknown
    assert(evaluate(l_dag, {1, 1, 1, 1}) == true);

    std::stringstream l_ss;

    l_ss << l_dag;

    assert(l_ss.str() == "([0]'([1]'+[1]([2]'+[2][3]))+[0]([1]'[2][3]+[1][3]))");
    
}

void test_generalize_exnor(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = false;
    
    std::set<node> l_nodes;

    global_node_sink::bind(&l_nodes);

    /// Modelling c exnor d.
    

    std::set<input> l_zeroes =
    {
        { 0, 0, 0, 1 },
        { 0, 1, 0, 1 },
        { 0, 0, 1, 0 },
        { 0, 1, 1, 0 },
        { 1, 0, 0, 1 },
        { 1, 0, 1, 0 },
        { 1, 1, 0, 1 },
        { 1, 1, 1, 0 },
    };

    std::set<input> l_ones =
    {
        { 0, 0, 0, 0 },
        { 0, 0, 1, 1 },
        { 0, 1, 0, 0 },
        { 0, 1, 1, 1 },
        { 1, 0, 0, 0 },
        { 1, 0, 1, 1 },
        { 1, 1, 0, 0 },
        { 1, 1, 1, 1 },
    };

    const node* l_model = generalize(l_zeroes, l_ones);
    
    LOG(l_model << std::endl);

    std::stringstream l_ss;

    l_ss << l_model;

    assert(l_ss.str() == "([2]'[3]'+[2][3])");
    
}

void test_generalize_exor(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = false;
    
    std::set<node> l_nodes;

    global_node_sink::bind(&l_nodes);

    /// Modelling c exnor d.
    

    std::set<input> l_zeroes =
    {
        { 0, 0, 0, 0 },
        { 0, 0, 1, 1 },
        { 0, 1, 0, 0 },
        { 0, 1, 1, 1 },
        { 1, 0, 0, 0 },
        { 1, 0, 1, 1 },
        { 1, 1, 0, 0 },
        { 1, 1, 1, 1 },
    };

    std::set<input> l_ones =
    {
        { 0, 0, 0, 1 },
        { 0, 1, 0, 1 },
        { 0, 0, 1, 0 },
        { 0, 1, 1, 0 },
        { 1, 0, 0, 1 },
        { 1, 0, 1, 0 },
        { 1, 1, 0, 1 },
        { 1, 1, 1, 0 },
    };

    const node* l_model = generalize(l_zeroes, l_ones);
    
    LOG(l_model << std::endl);

    std::stringstream l_ss;

    l_ss << l_model;

    assert(l_ss.str() == "([2]'[3]+[2][3]')");
    
}

void unit_test_main(

)
{
    constexpr bool ENABLE_DEBUG_LOGS = true;

    TEST(test_utils_filter);
    TEST(test_utils_cover);
    TEST(test_utils_partition);
    TEST(test_small_generalization_0);
    TEST(test_small_generalization_1);
    TEST(test_small_generalization_2);
    TEST(test_small_generalization_3);
    TEST(test_generalize_exnor);
    TEST(test_generalize_exor);
    
}

int main(

)
{
    unit_test_main();
    return 0;
}

#pragma endregion

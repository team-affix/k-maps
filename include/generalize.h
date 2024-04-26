#ifndef GENERALIZE_H
#define GENERALIZE_H

#include <unistd.h>
#include <set>
#include <vector>

#include "../factor-dag/include/dag.h"

namespace karnaugh
{
    
    ////////////////////////////////////////////
    //////////// UTILITY FUNCTIONS /////////////
    ////////////////////////////////////////////
    #pragma region UTILITY FUNCTIONS

    /// Constructs a set of pointers given
    ///     a set of any type.
    template<typename T>
    inline std::set<const T*> pointers(
        const std::set<T>& a_vals
    )
    {
        std::set<const T*> l_result;
        
        for (const T& l_val : a_vals)
            l_result.insert(&l_val);

        return l_result;
        
    }

    /// Filters the inputted set by
    ///     the supplied predicate.
    template<typename T, typename FUNCTION>
    inline std::set<T> filter(
        const std::set<T>& a_vals,
        const FUNCTION& a_query
    )
    {
        std::set<T> l_result;

        std::copy_if(
            a_vals.begin(),
            a_vals.end(),
            std::inserter(
                l_result,
                l_result.begin()
            ),
            a_query
        );

        return l_result;
        
    }

    /// Returns a cover (see set theory)
    ///     of an inputted set, grouped
    ///     by the supplied lambda.
    template<typename VALUE, typename FUNCTION>
    inline auto cover(
        const std::set<VALUE>& a_values,
        const FUNCTION& a_grouper
    )
    {
        using KEY =
            std::decay_t<
                decltype(*a_grouper(VALUE()).begin())
            >;
        
        std::map<KEY, std::set<VALUE>> l_result;

        for (const VALUE& l_value : a_values)
        {
            std::set<KEY> l_keys = a_grouper(l_value);

            for (const KEY& l_key : l_keys)
                l_result[l_key].insert(l_value);
            
        }

        return l_result;
        
    }

    /// Returns a partition (see set theory)
    ///     of the inputted set, grouped by
    ///     the supplied lambda.
    template<typename VALUE, typename FUNCTION>
    inline auto partition(
        const std::set<VALUE>& a_values,
        const FUNCTION& a_partitioner
    )
    {
        return cover(
            a_values,
            [&a_partitioner](
                VALUE a_value
            )
            {
                return std::set({ a_partitioner(a_value) });
            }
        );
    }

    #pragma endregion

    ////////////////////////////////////////////
    ///////////// GENERALIZATION ///////////////
    ////////////////////////////////////////////
    #pragma region GENERALIZATION

    using input = std::vector<bool>;

    inline const dag::node* generalize(
        const uint32_t a_variable_index,
        const std::set<const input*>& a_zeroes,
        const std::set<const input*>& a_ones
    )
    {
        /// Must check in this order.
        ///     ensures that we return
        ///     a 1 iff sat coverage is
        ///     nonzero, but dissatisfying
        ///     coverage is zero.
        if (a_ones.size() == 0)
            return dag::ZERO;
        if (a_zeroes.size() == 0)
            return dag::ONE;

        std::set<const input*> l_left_zeroes;
        std::set<const input*> l_right_zeroes;

        for (const input* l_input : a_zeroes)
        {
            if (l_input->at(a_variable_index) == false)
                l_left_zeroes.insert(l_input);
            else
                l_right_zeroes.insert(l_input);
        }

        std::set<const input*> l_left_ones;
        std::set<const input*> l_right_ones;

        for (const input* l_input : a_ones)
        {
            if (l_input->at(a_variable_index) == false)
                l_left_ones.insert(l_input);
            else
                l_right_ones.insert(l_input);
        }

        return dag::global_node_sink::emplace(
            a_variable_index,
            generalize(
                a_variable_index + 1,
                l_left_zeroes,
                l_left_ones
            ),
            generalize(
                a_variable_index + 1,
                l_right_zeroes,
                l_right_ones
            )
        );

    }

    #pragma endregion

}

#endif

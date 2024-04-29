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
        const std::set<const dag::node*>& a_remaining_literals,
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

        /////////////////////////////////////////////////////
        /// 1. Group each zero into subsets,
        ///      defined by coverage by a literal.
        /////////////////////////////////////////////////////
        
        std::map<const dag::node*, std::set<const input*>> l_zero_cover =
            cover(
                a_zeroes,
                [&a_remaining_literals](
                    const input* a_zero
                )
                {
                    return karnaugh::filter(
                        a_remaining_literals,
                        [a_zero](
                            const dag::node* a_literal
                        )
                        {
                            return dag::evaluate(a_literal, *a_zero);
                        }
                    );
                }
            );
        
        //////////////////////////////////////////////////////
        /// 2. Sort literals in ascending dissatisfying cov. size.
        //////////////////////////////////////////////////////
        
        /// Key type is std::pair<size_t, literal> because we
        ///     can populate size_t with dissatisfying cov size.
        ///     This will ensure the set is sorted by minimum
        ///     dissatisfying coverage.
        std::set<std::pair<size_t, const dag::node*>> l_sorted_literals;

        for (const dag::node* l_literal : a_remaining_literals)
            l_sorted_literals.emplace(l_zero_cover[l_literal].size(), l_literal);

        //////////////////////////////////////////////////////
        /// 3. Partition the ones based on the literal
        ///     that has minimum dissatisfying coverage 
        ///     that simultaneously covers it.
        //////////////////////////////////////////////////////

        std::map<const dag::node*, std::set<const input*>> l_one_partition =
            partition(
                a_ones,
                [&l_sorted_literals](
                    const input* a_input
                )
                {
                    auto l_first_covering_literal =
                        std::find_if(
                            l_sorted_literals.begin(),
                            l_sorted_literals.end(),
                            [a_input](
                                const auto& a_entry
                            )
                            {
                                return dag::evaluate(a_entry.second, *a_input);
                            }
                        );

                    return l_first_covering_literal->second;
                    
                }
            );

        /////////////////////////////////////////////////////
        /// 4. Realize ALL subtrees.
        /////////////////////////////////////////////////////

        #pragma region REALIZE SUBTREES

        const dag::node* l_result = dag::ZERO;

        for (const auto& [l_selected_literal, l_one_block] : l_one_partition)
        {
            /// Filter all remaining literals based on
            ///     literal that is being taken care of
            ///     by this edge to the subtree.
            std::set<const dag::node*> l_subtree_remaining_literals =
                filter(
                    a_remaining_literals,
                    [l_selected_literal](
                        const dag::node* a_literal
                    )
                    {
                        return a_literal->depth() != l_selected_literal->depth();
                    }
                );

            l_result = 
                logic::disjoin(
                    l_result,
                    logic::conjoin(
                        l_selected_literal,
                        generalize(
                            l_subtree_remaining_literals,
                            l_zero_cover[l_selected_literal],
                            l_one_block
                        )
                    )
                );

        }

        #pragma endregion

        return l_result;

    }

    inline const dag::node* generalize(
        const std::set<input>& a_zeroes,
        const std::set<input>& a_ones
    )
    {

        /////////////////////////////////////////////////////
        /// CONSTRUCT STARTING LITERALS
        /////////////////////////////////////////////////////

        const size_t l_num_vars = a_zeroes.begin()->size();

        std::set<const dag::node*> l_literals;

        for (uint32_t l_variable = 0; l_variable < l_num_vars; ++l_variable)
        {
            l_literals.insert(dag::literal(l_variable, false));
            l_literals.insert(dag::literal(l_variable, true));
        }

        /////////////////////////////////////////////////////
        /// CREATE INPUT POINTERS
        /////////////////////////////////////////////////////

        std::set<const input*> l_zero_pointers, l_one_pointers;

        for (const input& l_input : a_zeroes)
            l_zero_pointers.insert(&l_input);

        for (const input& l_input : a_ones)
            l_one_pointers.insert(&l_input);

        /////////////////////////////////////////////////////
        /// DONE PREPROCESSING, BEGIN GENERALIZATION
        /////////////////////////////////////////////////////

        return generalize(
            l_literals,
            l_zero_pointers,
            l_one_pointers
        );
        
    }

    #pragma endregion

}

#endif

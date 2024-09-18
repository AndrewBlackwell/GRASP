#ifndef GRASP_NODE_HPP
#define GRASP_NODE_HPP

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>

namespace Trainer
{

    // @brief Represents a node in the regret minimization process, storing strategies and regrets.
    class Node
    {
    public:
        // @brief Constructs a Node with the specified number of actions.
        // @param actionNum The number of possible actions for this node.
        explicit Node(int actionNum = 0);

        // @brief Destructor for Node, responsible for cleaning up dynamically allocated memory.
        ~Node();

        // @brief Returns the current strategy for this node.
        // @return A pointer to the strategy array.
        const double *strategy();

        // @brief Returns the average strategy for this node.
        // @return A pointer to the average strategy array.
        const double *averageStrategy();

        // @brief Updates the cumulative strategy sum with the given strategy and realization weight.
        // @param strategy The strategy array to be added to the cumulative sum.
        // @param realizationWeight The weight by which to scale the strategy before adding it.
        void strategySum(const double *strategy, double realizationWeight);

        // @brief Updates the strategy based on the regret sum.
        void updateStrategy();

        // @brief Returns the cumulative regret for a given action.
        // @param chooseAction The index of the action.
        // @return The cumulative regret for the chosen action.
        double regretSum(int chooseAction) const;

        // @brief Sets the regret sum for a given action.
        // @param chooseAction The index of the action.
        // @param value The new regret value to set.
        void regretSum(int chooseAction, double value);

        // @brief Returns the number of possible actions for this node.
        // @return The number of actions as an unsigned 8-bit integer.
        uint8_t actionNum() const;

    private:
        friend class boost::serialization::access;

        // @brief Calculates the average strategy based on the cumulative strategy sum.
        void calcAverageStrategy();

        // @brief Saves the Node state to an archive (serialization).
        // @tparam Archive The type of archive used for serialization.
        // @param ar The archive where the data will be saved.
        // @param version The version of the serialization.
        template <class Archive>
        void save(Archive &ar, const unsigned int version) const
        {
            std::vector<double> vec(mAverageStrategy, mAverageStrategy + mActionNum);
            ar & vec;
        }

        // @brief Loads the Node state from an archive (deserialization).
        // @tparam Archive The type of archive used for deserialization.
        // @param ar The archive from which the data will be loaded.
        // @param version The version of the serialization.
        template <class Archive>
        void load(Archive &ar, const unsigned int version)
        {
            std::vector<double> vec;
            ar & vec;
            mActionNum = vec.size();
            delete[] mAverageStrategy;
            mAverageStrategy = new double[vec.size()];
            for (int i = 0; i < vec.size(); ++i)
            {
                mAverageStrategy[i] = vec[i];
            }
            alreadyCalculated = true;
            strategyNeedsUpdate = false;
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()

        uint8_t mActionNum;       // Number of possible actions.
        double *mRegretSum;       // Array holding cumulative regrets for each action.
        double *mCurrentStrategy; // Array holding the current strategy probabilities.
        double *mStrategySum;     // Array holding the cumulative strategy sums.
        double *mAverageStrategy; // Array holding the average strategy.
        bool alreadyCalculated;   // Flag indicating if the average strategy has been calculated.
        bool strategyNeedsUpdate; // Flag indicating if the strategy needs to be updated.
    };

}

#endif
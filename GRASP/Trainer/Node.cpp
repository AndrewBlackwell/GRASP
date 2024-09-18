#include "Node.hpp"

namespace Trainer
{

    // @brief Constructs a Node with the given number of actions, initializing all internal data structures.
    Node::Node(const int actionNum) : mActionNum(actionNum), alreadyCalculated(false), strategyNeedsUpdate(false)
    {
        mRegretSum = new double[actionNum];
        mCurrentStrategy = new double[actionNum];
        mStrategySum = new double[actionNum];
        mAverageStrategy = new double[actionNum];
        for (int a = 0; a < actionNum; ++a)
        {
            mRegretSum[a] = 0.0;
            mCurrentStrategy[a] = 1.0 / (double)actionNum;
            mStrategySum[a] = 0.0;
            mAverageStrategy[a] = 0.0;
        }
    }

    // @brief Destructor for Node, responsible for deallocating dynamic memory.
    Node::~Node()
    {
        delete[] mRegretSum;
        delete[] mCurrentStrategy;
        delete[] mStrategySum;
        delete[] mAverageStrategy;
    }

    // @brief Returns the current strategy for this node.
    // @return A pointer to the strategy array.
    const double *Node::strategy()
    {
        return mCurrentStrategy;
    }

    // @brief Returns the average strategy for this node.
    // @return A pointer to the average strategy array.
    const double *Node::averageStrategy()
    {
        if (!alreadyCalculated)
        {
            calcAverageStrategy();
        }
        return mAverageStrategy;
    }

    // @brief Adds the given strategy to the cumulative strategy sum, scaled by the realization weight.
    // @param strategy The strategy array to be added to the cumulative sum.
    // @param realizationWeight The weight by which to scale the strategy before adding it.
    void Node::strategySum(const double *strategy, const double realizationWeight)
    {
        for (int a = 0; a < mActionNum; ++a)
        {
            mStrategySum[a] += realizationWeight * strategy[a];
        }
        alreadyCalculated = false;
    }

    // @brief Updates the strategy based on the cumulative regret sums.
    void Node::updateStrategy()
    {
        if (!strategyNeedsUpdate)
        {
            return;
        }
        double normalizingSum = 0.0;
        for (int a = 0; a < mActionNum; ++a)
        {
            mCurrentStrategy[a] = mRegretSum[a] > 0 ? mRegretSum[a] : 0;
            normalizingSum += mCurrentStrategy[a];
        }
        for (int a = 0; a < mActionNum; ++a)
        {
            if (normalizingSum > 0)
            {
                mCurrentStrategy[a] /= normalizingSum;
            }
            else
            {
                mCurrentStrategy[a] = 1.0 / (double)mActionNum;
            }
        }
    }

    // @brief Returns the cumulative regret for a specific action.
    // @param chooseAction The index of the action.
    // @return The cumulative regret for the chosen action.
    double Node::regretSum(const int chooseAction) const
    {
        return mRegretSum[chooseAction];
    }

    // @brief Sets the cumulative regret for a specific action and marks the strategy as needing an update.
    // @param chooseAction The index of the action.
    // @param value The new regret value to set.
    void Node::regretSum(const int chooseAction, const double value)
    {
        mRegretSum[chooseAction] = value;
        strategyNeedsUpdate = true;
    }

    // @brief Returns the number of actions available at this node.
    // @return The number of actions as an unsigned 8-bit integer.
    uint8_t Node::actionNum() const
    {
        return mActionNum;
    }

    // @brief Calculates the average strategy based on the cumulative strategy sums.
    void Node::calcAverageStrategy()
    {
        if (alreadyCalculated)
        {
            return;
        }

        for (int a = 0; a < mActionNum; ++a)
        {
            mAverageStrategy[a] = 0.0;
        }
        double normalizingSum = 0.0;
        for (int a = 0; a < mActionNum; ++a)
        {
            normalizingSum += mStrategySum[a];
        }
        for (int a = 0; a < mActionNum; ++a)
        {
            if (normalizingSum > 0)
            {
                mAverageStrategy[a] = mStrategySum[a] / normalizingSum;
            }
            else
            {
                mAverageStrategy[a] = 1.0 / (double)mActionNum;
            }
        }
        alreadyCalculated = true;
    }

}
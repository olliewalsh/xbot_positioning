#ifndef KALMAN_EXAMPLES_ROBOT1_POSITIONMEASUREMENTMODEL_HPP_
#define KALMAN_EXAMPLES_ROBOT1_POSITIONMEASUREMENTMODEL_HPP_

#include <kalman/LinearizedMeasurementModel.hpp>
#include "SystemModel.hpp"

namespace xbot
{
namespace positioning
{

/**
 * @brief Measurement vector measuring the robot position
 *
 * @param T Numeric scalar type
 */
template<typename T>
class PositionMeasurement : public Kalman::Vector<T, 2>
{
public:
    KALMAN_VECTOR(PositionMeasurement, T, 2)
    
    //! X position of the GPS antenna
    static constexpr size_t X = 0;
    
    //! Y position of the GPS antenna
    static constexpr size_t Y = 1;
    
    T x_pos()       const { return (*this)[ X ]; }
    T y_pos()       const { return (*this)[ Y ]; }
    
    T& x_pos()      { return (*this)[ X ]; }
    T& y_pos()      { return (*this)[ Y ]; }
};

/**
 * @brief Measurement model for measuring the position of the robot
 *        using two beacon-landmarks
 *
 * This is the measurement model for measuring the position of the robot.
 * The measurement is given by two landmarks in the space, whose positions are known.
 * The robot can measure the direct distance to both the landmarks, for instance
 * through visual localization techniques.
 *
 * @param T Numeric scalar type
 * @param CovarianceBase Class template to determine the covariance representation
 *                       (as covariance matrix (StandardBase) or as lower-triangular
 *                       coveriace square root (SquareRootBase))
 */
template<typename T, template<class> class CovarianceBase = Kalman::StandardBase>
class PositionMeasurementModel : public Kalman::LinearizedMeasurementModel<State<T>, PositionMeasurement<T>, CovarianceBase>
{
public:
    //! State type shortcut definition
    typedef  xbot::positioning::State<T> S;
    
    //! Measurement type shortcut definition
    typedef  xbot::positioning::PositionMeasurement<T> M;
    
    /**
     * @brief Constructor
     *
     * @param landmark1x The x-position of landmark 1
     * @param landmark1y The y-position of landmark 1
     * @param landmark2x The x-position of landmark 2
     * @param landmark2y The y-position of landmark 2
     */
    PositionMeasurementModel()
    {
        // Setup noise jacobian. As this one is static, we can define it once
        // and do not need to update it dynamically
        this->V.setIdentity();
    }
    
    /**
     * @brief Definition of (possibly non-linear) measurement function
     *
     * This function maps the system state to the measurement that is expected
     * to be received from the sensor assuming the system is currently in the
     * estimated state.
     *
     * @param [in] x The system state in current time-step
     * @returns The (predicted) sensor measurement for the system state
     */
    M h(const S& x) const
    {
        M measurement;

        // Calculate the GPS antenna position given the current system state.
        measurement.x_pos() = x.x_pos() + std::cos(x.theta()) * antenna_distance_x - std::sin(x.theta()) * antenna_distance_y;
        measurement.y_pos() = x.y_pos() + std::sin(x.theta()) * antenna_distance_x + std::cos(x.theta()) * antenna_distance_y;

        return measurement;
    }


protected:

    // TODO: play with offset here.
    // It's a right handed coordinate system, x = forward, y = left, z = up
    // X = distance to the front of the robot
    // Y = distance to the left of the robot
    // for YF classic these should be 0.3 and 0.0

    // DONT FORGET TO EDIT OTHER FILE AS WELL
    double antenna_distance_x = -0.01;
    double antenna_distance_y = 0.03;

    /**
     * @brief Update jacobian matrices for the system state transition function using current state
     *
     * This will re-compute the (state-dependent) elements of the jacobian matrices
     * to linearize the non-linear measurement function \f$h(x)\f$ around the
     * current state \f$x\f$.
     *
     * @note This is only needed when implementing a LinearizedSystemModel,
     *       for usage with an ExtendedKalmanFilter or SquareRootExtendedKalmanFilter.
     *       When using a fully non-linear filter such as the UnscentedKalmanFilter
     *       or its square-root form then this is not needed.
     *
     * @param x The current system state around which to linearize
     * @param u The current system control input
     */
    void updateJacobians( const S& x )
    {
      this->H.setIdentity();
    }
};

} // namespace Robot
} // namespace KalmanExamples

#endif
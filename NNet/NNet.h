#pragma once

#include "../utils.h"

namespace net{
    class Linear{
    public:
        Linear() = delete;
        Linear(int input_size, int output_size);
        void get_input(const matrix<double>& input);
        void push_forward(const matrix<double>& input_layer);
        matrix<double>& get_output();
        const matrix<double>& get_output() const;
        void soft_max();
        void apply_activation_function();
        matrix<double>& apply_derivative_function();
        matrix<double>& get_weights();
    private:
        int input_size, output_size;
        matrix<double> layer, not_activated_layer, bias, weights; // matrix [...] 1xsize
    };

    class NNet{
    public:
        NNet() = delete;
        NNet(const std::vector<Linear>& topology);
        void propagate_front(const matrix<double>& input);
        void propagate_back(const matrix<double>& loss);
        matrix<double>& get_output();
        const matrix<double>& get_output() const;
    private:
        double lr = 0.3;
        int size;
        std::vector<Linear> layers;
        matrix<double> input_layer;
    };

    class Loss{
    public:
        template <typename func_type, typename func_derivative_type>
        Loss(const NNet& neural_net, const matrix<double>& answer, func_type&& func, func_derivative_type&& func_derivative){
            matrix<double> result = neural_net.get_output();
            loss = aggregate(result, answer, func);
            loss_derivative = aggregate(result, answer, func_derivative);
        }
        double sum();
        matrix<double> get_loss();
        const matrix<double> get_loss() const;
        matrix<double> get_loss_derivative();
        const matrix<double> get_loss_derivative() const;
    private:
        matrix<double> loss;
        matrix<double> loss_derivative;
    };

    // ADDITIONAL FUNCTIONS

    double ReLU(double x);

    double ReLU_derivative(double x);

    double MSE(double a, double y);

    double MSE_derivative(double a, double y);
}
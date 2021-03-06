#include "NNet.h"

namespace net {
    Linear::Linear(int input_size, int output_size) {
        this->input_size = input_size;
        this->output_size = output_size;
        not_activated_layer = matrix<double>(1, output_size);
        layer = not_activated_layer;
        bias = matrix<double>(output_size, 1);
        weights = matrix<double>(input_size, output_size);
        {
            // TODO write function fill with random
            std::mt19937 rnd(42);
            std::uniform_real_distribution<double> dist(-1, 2);
            for (int i = 0; i < input_size; ++i)
                for (int j = 0; j < output_size; ++j)
                    weights[i][j] = dist(rnd);
        }
    }

    matrix<double>& Linear::get_output(){
        return layer;
    }
    const matrix<double>& Linear::get_output() const{
        return layer;
    }

    void Linear::get_input(const matrix<double>& input_layer){
        // receiving input
        ASSERT((input_layer.row_size() == 1 && input_layer.col_size() == input_size), "incorrect input size");

        // converting to output
        push_forward(input_layer);
    }

    matrix<double>& Linear::get_weights(){
        return weights;
    }

    void Linear::push_forward(const matrix<double>& input_layer){
        not_activated_layer = input_layer * weights;
        layer = not_activated_layer;
    }

    void Linear::soft_max(){
        // TODO aggregate with not void
        double sum = 0;
        {
            for (int i = 0; i < output_size; ++i)
                sum += std::exp(layer[0][i]);
        }
        layer.aggregate([&sum](double& value){
            value = std::exp(value) / sum;
        });
    }

    void Linear::apply_activation_function(){
        layer.aggregate([](double& value){
            value = ReLU(value);
        });
    }

    matrix<double>& Linear::apply_derivative_function(){
        not_activated_layer.aggregate([](double& value){
            value = ReLU_derivative(value);
        });
        return not_activated_layer;
    }

    NNet::NNet(const std::vector<Linear>& topology){
        size = topology.size();
        layers = topology;
    }

    void NNet::propagate_front(const matrix<double>& input) {
        input_layer = input;
        {
            layers[0].get_input(input);
            if (0 == size - 1) {
                // soft_max
                layers[0].soft_max();
            } else {
                // ReLU
                layers[0].apply_activation_function();
            }
        }
        for (int layer = 1; layer < size; ++layer){
            layers[layer].get_input(layers[layer - 1].get_output());
            // crutch TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! only for linear classification
            if (layer == size - 1) {
                // soft_max
                layers[layer].soft_max();
            } else {
                // ReLU
                layers[layer].apply_activation_function();
            }
        }
    }

    matrix<double>& NNet::get_output(){
        return layers.back().get_output();
    }
    const matrix<double>& NNet::get_output() const{
        return layers.back().get_output();
    }

    void NNet::propagate_back(const matrix<double> &loss) {
        // propagating for the first layer
        auto& delta = layers.back().apply_derivative_function();
        delta = aggregate(delta, loss, [](double a, double b){ // delta *= loss;
            return a * b;
        });

        auto& weights = layers.back().get_weights();
        weights -= ((size == 1 ? input_layer.transpose() : layers[size-2].get_output().transpose()) * delta); // TODO may be +=
        // propagating for the others
        for (int layer = size-2; layer >= 0; --layer){
            delta *= layers[layer].get_weights();
            delta = aggregate(delta, layers[layer].apply_derivative_function(), [](double a, double b){ // delta *= layers[layer].apply_derivative_function();
                return a * b;
            });
            auto& weights1 = layers[layer].get_weights();
            weights1 -= ((layer == 0 ? input_layer.transpose() : layers[layer-1].get_output().transpose()) * delta); // TODO may be +=
        }
    }

    double Loss::sum(){
        return loss.sum();
    }

    matrix<double> Loss::get_loss(){
        return loss;
    }
    const matrix<double> Loss::get_loss() const{
        return loss;
    }

    matrix<double> Loss::get_loss_derivative(){
        return loss_derivative;
    }
    const matrix<double> Loss::get_loss_derivative() const{
        return loss_derivative;
    }

    double ReLU(double x) {
        if (x < 0) return 0;
        return x;
    }

    double ReLU_derivative(double x){
        if (x < 0) return 0; // TODO check if <=
        return 1;
    }

    double MSE(double a, double y){
        return (a-y)*(a-y);
    }

    double MSE_derivative(double a, double y){
        return a-y;
    }
}
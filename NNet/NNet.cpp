#include "NNet.h"

namespace net {
    Linear::Linear(int input_size, int output_size) {
        this->input_size = input_size;
        this->output_size = output_size;
        input_layer = matrix<double>(1, input_size);
        output_layer = matrix<double>(1, output_size);
        bias = matrix<double>(output_size, 1);
        weights = matrix<double>(input_size, output_size);
    }

    matrix<double>& Linear::get_output(){
        return output_layer;
    }
    const matrix<double>& Linear::get_output() const{
        return output_layer;
    }

    void Linear::get_input(const matrix<double>& input){
        // receiving input
        ASSERT((input.row_size() == 1 && input.col_size() == input_size), "incorrect input size");
        input_layer = input;

        // converting to output
        push_forward();
    }

    void Linear::push_forward(){
        output_layer = input_layer * weights;
    }

    void Linear::soft_max(){
        double sum = output_layer.sum();
        output_layer.aggregate([&sum](double& value){
            value /= sum;
        });
    }

    void Linear::apply_activation_function(){
        output_layer.aggregate([](double& value){
            value = ReLU(value);
        });
    }

    NNet::NNet(const std::vector<Linear>& topology){
        size = topology.size();
        layers = topology;
    }

    void NNet::propagate_front(const matrix<double>& input) {
        layers[0].get_input(input);
        for (int layer = 1; layer < size; ++layer){
            layers[layer].get_input(layers[layer - 1].get_output());
            std::cout << layers[layer].get_output() << std::endl;
            // crutch TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! only for linear classification
            if (layer == size - 1) {
                // soft_max
                layers[layer].soft_max();
            } else {
                // ReLU
                layers[layer].apply_activation_function();
            }
            std::cout << layers[layer].get_output() << std::endl;
        }
    }

    void NNet::propagate_back(const matrix<double> &loss) {

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
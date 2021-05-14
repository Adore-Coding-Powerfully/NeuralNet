#include <iostream>
#include "utils.h"
#include "NNet/NNet.h"

int main() {
    const int n = 4;

    std::vector<matrix<double> > batchX;
    batchX.push_back(std::vector<std::vector<double> >{{0, 0}});
    batchX.push_back(std::vector<std::vector<double> >{{0, 1}});
    batchX.push_back(std::vector<std::vector<double> >{{1, 0}});
    batchX.push_back(std::vector<std::vector<double> >{{1, 1}});

    std::vector<matrix<double> > batchY;
    batchY.push_back(std::vector<std::vector<double> >{{1, 0}});
    batchY.push_back(std::vector<std::vector<double> >{{0, 1}});
    batchY.push_back(std::vector<std::vector<double> >{{0, 1}});
    batchY.push_back(std::vector<std::vector<double> >{{1, 0}});

    net::NNet nn({net::Linear(2, 2), net::Linear(2, 2)});
    for (int i = 0; i < n; ++i) {
        nn.propagate_front(batchX[i]);
        net::Loss loss(nn, batchY[i], net::MSE, net::MSE_derivative);
        std::cout << loss.sum() << std::endl;
    }
    return 0;
}

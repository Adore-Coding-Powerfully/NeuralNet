#include "matrix.h"

#include <string>
#include <iostream>
#include <set>

using namespace std;

int main() {

    // example to use matrix [][] and output
    {
        matrix<int> M(3, 4);//, 4);

        M[2][1] = M[0][3] = 17;
        M[0][1] = M[1][2] = -9;
        M[0][1] = M[1][2] = -9;
        M[0][3] = M[0][0] = 2;

        std::cout << M << "\n";

        std::cout << M.transpose() << "\n";


        // constructors. Move and copy
        {
            auto C = M = M;

            auto G = move(M);

            M.clear();

            C.clear();
        } // call ~matrix()
    }

    // example to use aggregateregate, +, sum, mean
    {
        matrix<string> S(4, 4);

        for (int i = 0; i < S.row_size(); i++) {
            for (int j = 0; j < S.col_size(); j++) {
                S[i][j] = "a";
            }
        }

        S += S; // "a" + "a"

        // compile error
        // такой операции нет для данного типа
        // S *= S; 

        int accum = 0;
        S.aggregate([&accum](const string& b) {
            accum += b.size();
        });

        cout << accum << "\n" << S.sum() << "\n" << S << "\n";

        // compile error
        /*matrix<double> F = vector<double>({
            1.932, 7.128,
            -912.2, 74.000001,
        });*/


        // set отсортирует в:
        // {-912.2, 74.0001},
        // {1.932, 7.128},
        matrix<double> D = set<vector<double>>({
            {1.932, 7.128},
            {-912.2, 74.0001},
        });

        cout << D << "\n" << D.sum() << " " << D.mean() << "\n";
    }

    // others
    {
        struct my_st {
            int g;
        };
        matrix<my_st> m(1, 3);

        // compile error
        // у my_st нет такой операции
        // cout << m << "\n";
    }

    return 0;
}

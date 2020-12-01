#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>
#include <numeric>
#include <algorithm>

using namespace std;
random_device r;
default_random_engine e1(r());

auto hill_climbing_alg = [](auto get_r_solution, auto get_neighbours, auto point, int max_i) {
    auto current_solution = get_r_solution();
    for (int i = 0; i < max_i; i++)
    {
        auto solutions = get_neighbours(current_solution);
        uniform_int_distribution<int> uniform_dist(0, solutions.size()-1);
        auto next_solution = solutions.at(uniform_dist(e1));
        if (point(current_solution) > point(next_solution)) {
            current_solution = next_solution;
        }
    }
    return current_solution;
};

int main(int argc, char** argv)
{
    int first;
    first = atoi(argv[1]); // string to int
    int last;
    last = atoi(argv[2]); // string to int

    auto beale = [](vector<double> d) {
        double x = d.at(0);
        double y = d.at(1);
        return pow(1.5-x+x*y,2.0)+ pow(2.25-x+x*pow(y,2),2)+pow(2.625-x+x*pow(y,3),2);

    };

    auto booth = [](vector<double> d) {
        double x = d.at(0);
        double y = d.at(1);
        return pow(x+2*y-7,2)+pow(2*x+y-5,2);

    };

    auto matyas = [](vector<double> d) {
        double x = d.at(0);
        double y = d.at(1);
        return 0.26*(pow(x,2)+pow(y,2))-0.48*x*y;

    };

    auto optimize = [](auto function, int z1, int z2){
        int max_i = 100000;
        uniform_real_distribution<double> uni_dist(z1,z2);
        auto neighbours = [](vector<double> z1, double dx = 0.001) {
            vector<vector<double>> ret;
            for (int i = 0; i < z1.size(); i++)
            {
                auto nx = z1;
                nx[i] += dx;
                ret.push_back(nx);
                nx[i] -= 2.0 * dx;
                ret.push_back(nx);
            }
            return ret;
        };

        auto init = [&uni_dist]() {
            vector<double> x(2);
            x[0] = uni_dist(e1);
            x[1] = uni_dist(e1);
            return x;
        };

        auto solution = hill_climbing_alg(
                init,
                neighbours,
                function,
                max_i);

        cout << "results: [ ";
        for (auto e : solution)
            cout << e << " ";
        cout << "] -> " << function(solution) << endl;
    };

    if(string (argv[3]) == "matyas") {
        cout << "matyas: " << endl;
        optimize(matyas, first, last); //-4.5 <= x,y <= 4.5
    }
    if(string (argv[3]) == "booth") {
        cout << "booth: " << endl;
        optimize(booth, first, last); //-10 <= x,y <= 10
    }

    if(string (argv[3]) == "beale") {
        cout << "beale: " << endl;
        optimize(beale, first, last); //-10 <= x,y <= 10
    }
}
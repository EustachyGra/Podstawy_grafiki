#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

std::vector<double> poly_multiply(const std::vector<double>& a, const std::vector<double>& b)
{
    std::vector<double> result(a.size() + b.size() - 1, 0.0);
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = 0; j < b.size(); j++)
            result[i + j] += a[i] * b[j];
    return result;
}

long long factorial(int n)
{
    long long result = 1;
    for (int i = 2; i <= n; i++)
        result *= i;
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 3 || (argc - 1) % 2 != 0)
    {
        std::cerr << "Usage: " << argv[0] << " x1 x2 ... xn y1 y2 ... yn\n";
        return 1;
    }

    int n = (argc - 1) / 2;

    std::vector<double> X(n), Y(n);
    for (int i = 0; i < n; i++)
    {
        X[i] = std::atof(argv[1 + i]);
        Y[i] = std::atof(argv[1 + n + i]);
    }

    std::vector<std::vector<double>> D(n, std::vector<double>(n, 0.0));

    D[0][0] = Y[0];
    for (int j = 1; j < n; j++)
    {
        if (X[j] == X[j - 1])
            D[j][0] = D[j - 1][0];
        else
            D[j][0] = Y[j];
    }

    for (int j = 1; j < n; j++)
    {
        int s = 0;
        for (int k = j; k < n; k++)
        {
            if (X[k] == X[k - j])
            {
                D[k][j] = Y[k - s] / factorial(j);
                s++;
            }
            else
            {
                D[k][j] = (D[k][j - 1] - D[k - 1][j - 1]) / (X[k] - X[k - j]);
                s = 0;
            }
        }
    }

    std::vector<double> C = { D[n - 1][n - 1] };

    for (int k = n - 2; k >= 0; k--)
    {
        std::vector<double> poly = { 1.0, -X[k] };
        C = poly_multiply(C, poly);
        C.back() += D[k][k];
    }

    std::cout << "C = [";
    for (size_t i = 0; i < C.size(); i++)
    {
        if (i > 0) std::cout << ", ";
        std::cout << C[i];
    }
    std::cout << "]\n";

    std::cout << "D = \n";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            std::cout << D[i][j] << "\t";
        std::cout << "\n";
    }

    return 0;
}

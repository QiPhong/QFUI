#pragma once
#include <vector>
#include <cmath>
#include "QFUIWND.h"
namespace QF {

    struct Point {
        double X, Y;
    };
    // 矩阵乘法
    std::vector<std::vector<double>> multiplyMatrices(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B) {
        int n = A.size();
        int m = A[0].size();
        int p = B[0].size();
        std::vector<std::vector<double>> C(n, std::vector<double>(p, 0.0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < p; ++j) {
                for (int k = 0; k < m; ++k) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return C;
    }

    // 矩阵转置
    std::vector<std::vector<double>> transposeMatrix(const std::vector<std::vector<double>>& A) {
        int n = A.size();
        int m = A[0].size();
        std::vector<std::vector<double>> B(m, std::vector<double>(n, 0.0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                B[j][i] = A[i][j];
            }
        }
        return B;
    }

    // 高斯消元法求解线性方程组
    std::vector<double> gaussianElimination(std::vector<std::vector<double>> A, std::vector<double> b) {
        int n = A.size();
        for (int i = 0; i < n; ++i) {
            // 找到主元
            int maxRow = i;
            for (int k = i + 1; k < n; ++k) {
                if (std::abs(A[k][i]) > std::abs(A[maxRow][i])) {
                    maxRow = k;
                }
            }

            // 交换行
            std::swap(A[i], A[maxRow]);
            std::swap(b[i], b[maxRow]);

            // 消元
            for (int k = i + 1; k < n; ++k) {
                double c = A[k][i] / A[i][i];
                for (int j = i; j < n; ++j) {
                    A[k][j] -= c * A[i][j];
                }
                b[k] -= c * b[i];
            }
        }

        // 回代求解
        std::vector<double> x(n);
        for (int i = n - 1; i >= 0; --i) {
            x[i] = b[i] / A[i][i];
            for (int k = i - 1; k >= 0; --k) {
                b[k] -= A[k][i] * x[i];
            }
        }
        return x;
    }

    // 多项式回归
    std::vector<double> polynomialRegression(const std::vector<Point>& points, int degree) {
        int n = points.size();
        std::vector<std::vector<double>> X(n, std::vector<double>(degree + 1));
        std::vector<double> Y(n);

        for (int i = 0; i < n; ++i) {
            double xi = points[i].X;
            double yi = points[i].Y;
            Y[i] = yi;
            for (int j = 0; j <= degree; ++j) {
                X[i][j] = std::pow(xi, j);
            }
        }

        std::vector<std::vector<double>> XT = transposeMatrix(X);
        std::vector<std::vector<double>> XTX = multiplyMatrices(XT, X);

        // 手动构建XTY
        std::vector<double> XTY(degree + 1, 0.0);
        for (int i = 0; i <= degree; ++i) {
            for (int j = 0; j < n; ++j) {
                XTY[i] += XT[i][j] * Y[j];
            }
        }

        std::vector<double> coefficients = gaussianElimination(XTX, XTY);
        return coefficients;
    }

    // 计算给定x值的预测y值
    double predict(double x, const std::vector<double>& coefficients) {
        double y = 0.0;
        for (int i = 0; i < coefficients.size(); ++i) {
            y += coefficients[i] * std::pow(x, i);
        }
        return y;
    }


    std::vector<double> CharacteristicsValuationFunction(const std::vector<Point>& points)
    {
        // 输入散点数据
        double dta = 100;
        double param = -1;
       
        std::vector<double> coe;
        for (int j = 1; ++j < 64;) {
            int degree = j; // 多项式的最高次数

            // 计算多项式回归系数
            std::vector<double> coefficients = polynomialRegression(points, degree);

            // 预测给定x值对应的y值
            double x = points[points.size() / 2].X;
            double y = points[points.size() / 2].Y;
            double y_pred = predict(x, coefficients);
            if (std::isnan(y_pred))break;
            if (dta > std::abs(y - y_pred)) {
                dta = std::abs(y - y_pred);
                param = degree;
                coe = coefficients;

            }
  
        }
        printf("param = %f , dta= %f\n", param, dta);
        return coe;
    
    }


    // 线性插值函数
    double LinearInterpolation(double x1, double y1, double x2, double y2, double x) {
        return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
    }

    // 根据给定的x坐标找到对应的y值
    double GetCurveYValue(const std::vector<GPointF>& points, double x) {
        // 在points中找到最接近给定x坐标的两个点的索引
    // 二分查找
        int left = 0;
        int right = points.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (points[mid].X == x) {
                return points[mid].Y; // 找到精确匹配的点
            }
            else if (points[mid].X < x) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }
        // 二分查找结束后，left指向右侧最近的点，right指向左侧最近的点
        int idx1 = max(0, right);
        int idx2 = min((int)points.size() - 1, left);
        if (idx1 == idx2) {
            return points[idx1].Y;
        }
        // 使用线性插值计算给定x值对应的y值
        return LinearInterpolation(points[idx1].X, points[idx1].Y, points[idx2].X, points[idx2].Y, x);
    }

}
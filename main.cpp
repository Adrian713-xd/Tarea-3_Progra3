#include <iostream>
#include <vector>
#include <string>
#include <concepts>
#include <iterator>


using namespace std;

namespace core_numeric {
    template <typename C>
    concept Iterable = requires(C c) {
        begin(c);
        end(c);
    };

    template <typename T>
    concept Addable = requires(T a, T b) {
        { a + b } -> same_as<T>;
    };

    template <typename T>
    concept Divisible = requires(T a, std::size_t n) {
        { a / n } -> same_as<T>;
    };

    template <typename T>
    concept Comparable = requires(T a, T b) {
        { a < b } -> convertible_to<bool>;
    };


    template <Iterable C>
  requires Addable<typename C::value_type>
  auto sum(const C& container) {
        using T = typename C::value_type;
        T result{};
        for (const auto& value : container) {
            result = result + value;
        }
        return result;
    }


    template <Iterable C>
    requires Addable<typename C::value_type> && Divisible<typename C::value_type>
    auto mean(const C& container) {
        using T = typename C::value_type;
        T total = sum(container);
        return total / container.size();
    }

    template <Iterable C>
    requires Addable<typename C::value_type> && Divisible<typename C::value_type>
    auto variance(const C& container) {
        using T = typename C::value_type;
        auto avg = mean(container);

        double accum = 0.0;
        for (const auto& value : container) {
            auto diff = value - avg;
            accum += diff * diff;
        }
        return accum / container.size();
    }


    template <Iterable C>
    requires Comparable<typename C::value_type>
    auto max(const C& container) {
        auto it = begin(container);
        auto max_val = *it;
        for (++it; it != end(container); ++it) {
            if (max_val < *it) {
                max_val = *it;
            }
        }
        return max_val;
    }


    template <Iterable C, typename Func>
    auto transform_reduce(const C& container, Func func) {
        auto it = begin(container);
        auto result = func(*it);
        for (++it; it != end(container); ++it) {
            result = result + func(*it);
        }
        return result;
    }

    template <typename... Args>
requires (Addable<Args> && ...)
auto sum_variadic(Args... args) {
        return (args + ...);
    }

    template <typename... Args>
requires (Addable<Args> && ...)
auto mean_variadic(Args... args) {
        auto total = sum_variadic(args...);
        using CommonType = common_type_t<Args...>;


        if constexpr (is_integral_v<CommonType>) {

            return static_cast<double>(total) / sizeof...(args);
        } else {

            return total / static_cast<double>(sizeof...(args));
        }
    }

    template <typename... Args>
auto variance_variadic(Args... args) {
        double avg = mean_variadic(args...);

        double accum = (((args - avg) * (args - avg)) + ...);
        return accum / sizeof...(args);
    }

    template <typename First, typename... Args>
requires Comparable<First> && (Comparable<Args> && ...)
auto max_variadic(First first, Args... args) {
        auto max_val = first;

        ((max_val = (max_val < args) ? args : max_val), ...);
        return max_val;
    }

}

struct Vector3D {
    double x = 0, y = 0, z = 0;

    Vector3D operator+(const Vector3D& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3D operator/(std::size_t n) const { return {x / n, y / n, z / n}; }
    bool operator<(const Vector3D& o) const { return (x*x + y*y + z*z) < (o.x*o.x + o.y*o.y + o.z*o.z); }
};

struct Esfera {
    double radio;
    Esfera operator+(const Esfera& o) const { return {radio + o.radio}; }
};


int main() {
    cout << "--- CASOS QUE COMPILAN CORRECTAMENTE ---\n";


   vector<double> v1{1.0, 2.0, 3.0, 4.0};
   cout << "Mean (double): " << core_numeric::mean(v1) << "\n";
   cout << "Max (double): " << core_numeric::max(v1) << "\n";
   cout << "Sum Variadic: " << core_numeric::sum_variadic(1, 2, 33, 4) << "\n";

   vector<Vector3D> vectores{{1, 0, 0}, {3, 4, 0}, {0, 1, 0}};
    Vector3D m_vec = core_numeric::mean(vectores);
    Vector3D max_vec = core_numeric::max(vectores);
   cout << "Mean Vector3D: (" << m_vec.x << ", " << m_vec.y << ", " << m_vec.z << ")\n";


    auto r = core_numeric::transform_reduce(v1, [](double x) { return x * x; });
   cout << "Transform Reduce: " << r << "\n";

    cout << "\n--- CASOS QUE NO COMPILAN (VER COMENTARIOS EN CÓDIGO) ---\n";

    /*
    // CASO FALLIDO 1: std::string con algoritmo mean
   vector<std::string> palabras{"Hola", "Mundo"};
    // auto m_str = core_numeric::mean(palabras);


    // CASO FALLIDO 2: Esfera con algoritmo max
    std::vector<Esfera> esferas{{1.5}, {3.0}, {0.5}};
    // auto max_esf = core_numeric::max(esferas);


    // CASO FALLIDO 3: max_variadic con tipos que no se pueden comparar
    struct SinComparar {};
    // core_numeric::max_variadic(SinComparar{}, SinComparar{});
*/

    return 0;
}

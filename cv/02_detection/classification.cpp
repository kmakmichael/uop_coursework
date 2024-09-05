#include "classification.hpp"


Fruit classify(const region &r) {
    if (r.ecc > 0.8) {
        return banana;
    }
    if (r.eigen.first > 300 && r.eigen.second > 300) {
        return orange;
    }
    if (r.eigen.first < 300 && r.eigen.second < 300) {
        return apple;
    }
    return mystery_fruit;
}
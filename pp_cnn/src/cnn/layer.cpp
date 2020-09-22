#include "layer.hpp"

Layer::Layer(const string& name, const ELayerClass& layer_class) : name_(name), layer_class_(layer_class) {}
Layer::~Layer() {}

void Layer::forward(Ciphertext3D& input) const {
}

void Layer::forward(vector<Ciphertext>& input) const {
}

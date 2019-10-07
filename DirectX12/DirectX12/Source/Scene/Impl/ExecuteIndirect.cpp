#include "ExecuteIndirect.h"

ExecuteIndirect::ExecuteIndirect() { }

ExecuteIndirect::~ExecuteIndirect() { }

void ExecuteIndirect::load(Framework::Scene::Collecter& collecter) { }

void ExecuteIndirect::update() { }

bool ExecuteIndirect::isEndScene() const {
    return false;
}

void ExecuteIndirect::draw() { }

void ExecuteIndirect::end() { }

Framework::Define::SceneType ExecuteIndirect::next() {
    return Framework::Define::SceneType();
}

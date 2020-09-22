#pragma once

class OptOption {
public:
  OptOption() : enable_fuse_layers_(false), enable_optimize_activation_(false) {}
  ~OptOption() {}

  const bool& enable_fuse_layers() const { return enable_fuse_layers_; };
  const bool& enable_optimize_activation() const { return enable_optimize_activation_; };
  const bool& enable_optimize_pooling() const { return enable_optimize_pooling_; };

  void setEnableFuseLayers(const bool& enable_fuse_layers) {
    enable_fuse_layers_ = enable_fuse_layers;
  };
  void setEnableOptimizeActivation(const bool& enable_optimize_activation) {
    enable_optimize_activation_ = enable_optimize_activation;
  };
  void setEnableOptimizePooling(const bool& enable_optimize_pooling) {
    enable_optimize_pooling_ = enable_optimize_pooling;
  }

private:
  bool enable_fuse_layers_;
  bool enable_optimize_activation_;
  bool enable_optimize_pooling_;
};

// SPDX-License-Identifier:  MIT
// Copyright (C) 2021-2022 - Cycles - https://github.com/igormcoelho/cycles

#ifndef CYCLES_DETAIL_TARROWV1_HPP_
#define CYCLES_DETAIL_TARROWV1_HPP_

// C++
#include <iostream>
#include <utility>
#include <vector>
// C++ HELPER ONLY
#include <memory>   // JUST FOR HELPER is_shared_ptr??
#include <sstream>  // just for value_to_string ??
#include <string>
//
#include <cycles/detail/IDynowForest.hpp>
#include <cycles/detail/utils.hpp>
#include <cycles/detail/v1/TNodeV1.hpp>

using std::ostream, std::vector;  // NOLINT

// ============================
//   memory-managed Tree Node
// ============================
// Tree Node
// all memory is self-managed

namespace cycles {

namespace detail {

// default is now type-erased T
template <typename T = TNodeData>
class TArrowV1 : public IArrow {
  using X = T;

  bool debug_flag_arrow{false};

 public:
  bool is_owned_by_node{false};

 public:
  // TArrowV1() {}
  wptr<TNode<X>> remote_node;
  //
  // NOTE THAT is_owned_by_node MAY BE TRUE, WHILE owned_by_node
  // BECOMES UNREACHABLE... THIS HAPPENS IF OWNER DIES BEFORE THIS POINTER.
  // THE RELATION SHOULD BE IMMUTABLE, IT MEANS THAT ONCE "OWNED", ALWAYS
  // "OWNED".
  wptr<TNode<X>> owned_by_node;

 public:
  void setIsOwnedByNode(bool b) { this->is_owned_by_node = b; }

  void setDebug(bool b) {
    debug_flag_arrow = b;
    auto node = remote_node.lock();
    if (node) node->debug_flag = b;
  }

  bool debug() const { return debug_flag_arrow; }

  // ========== TWO FUNDAMENTAL PROPERTIES ===========
  // A) is_nullptr
  // B) is_root
  // C) is_owned
  // Node should respect: is_nullptr() || is_root() || is_owned()
  //

  // check if this pointer is nullptr
  bool is_nullptr() const override { return (!this->remote_node.lock()); }

  // check if this pointer is root (in tree/forest universe)
  bool is_root() const override {
    // IMPORTANT! DO NOT REMOVE is_owned() check from here!!
    if (is_nullptr() || is_owned()) {
      return false;
    } else {
      return !this->remote_node.lock()->has_parent();
      // ctx not avaliable here! use information from NodeLocator instead!
      // return
      // !(this->get_ctx()->opx_hasParent(this->arrow.remote_node.lock()));
    }
  }

  // check if this pointer is already owned by someone
  bool is_owned() const override {
    bool b1 = is_owned_by_node;
    // NOLINTNEXTLINE
    bool b2 = (bool)(this->owned_by_node.lock());
    if (b1 && !b2) {
      if (debug())
        std::cout
            << "relation_ptr is_owned() WARNING: is_owned_by_node but owner "
               "does not exist!"
            << std::endl;
    }
    return b1;
  }

  // HELPER: is_nullptr, is_root, is_owned
  std::string getType() {
    if (is_nullptr())
      return "is_nullptr";
    else if (is_root())
      return "is_root";
    else
      return "is_owned";
  }
};

}  // namespace detail

}  // namespace cycles

#endif  // CYCLES_DETAIL_TARROWV1_HPP_

// SPDX-License-Identifier:  MIT
// Copyright (C) 2021-2022 - Cycles - https://github.com/igormcoelho/cycles

#ifndef CYCLES_RELATION_PTR_HPP_  // NOLINT
#define CYCLES_RELATION_PTR_HPP_  // NOLINT

// C++
#include <iostream>
#include <map>
#include <string>  // ONLY FOR getType HELPER??
#include <utility>
#include <vector>

//
#include <cycles/detail/utils.hpp>
#include <cycles/detail/v1/DynowForestV1.hpp>
#include <cycles/detail/v1/TArrowV1.hpp>
#include <cycles/detail/v1/TreeV1.hpp>
#include <cycles/relation_pool.hpp>

using std::vector, std::ostream, std::map;  // NOLINT

// ========================
// relation_ptr using DynowForestV1
// ========================
// smart pointer suitable for cycles
// memory is self-managed
//-------------------------

namespace cycles {

// NOLINTNEXTLINE
using namespace detail;

template <typename T, class DOF = DynowForestV1>
// NOLINTNEXTLINE
class relation_ptr {
  using X = TNodeData;
  // TODO(igormcoelho): make private!
 public:
  using pool_type = DOF;
  // TODO(igormcoelho): is this weak or strong?
#ifdef WEAK_POOL_PTR
  wptr<DOF> ctx;
#else
  sptr<DOF> ctx;
#endif
  //
  TArrowV1<X> arrow;
  //
  bool debug_flag_ptr{false};

 public:
  void setDebug(bool b) {
    debug_flag_ptr = b;
    // auto node = remote_node.lock();
    // if (node) node->debug_flag = b;
    arrow.setDebug(b);
  }

 public:
#ifdef WEAK_POOL_PTR
  sptr<DOF> get_ctx() const { return ctx.lock(); }
#else
  sptr<DOF> get_ctx() const { return ctx; }
#endif

  bool debug() const { return debug_flag_ptr; }

  // C[-1] constructor for Strong Self links is a terrible idea...
  // 1) because it breaks many things! (with no much promising gains)
  // 2) it makes many things unsecure and changes basic assumptions (like no
  // strong loops!)
  // 3) it is interesting! but, maybe needs to improve current basic things
  // first
  /*
  // flag for C[-1] constructor...
  struct weak_self {};

  // ======= C[-1] constructor (weak self) ======
  // Maybe this is a bad idea, but seems interesting to try...
  // 1. will store T* t owned by ITSELF
  // 2. will create a new TNode
  // 3. will NOT create a Tree... it's all alone!
  relation_ptr(wptr<DOF> _ctx, T* t, weak_self flag) : ctx{_ctx} {
    ...
  }
    */

  // ======= C0 nullptr ===============
  relation_ptr() {
    // this->is_owned_by_node = false;
    this->arrow = TArrowV1<X>{};
    assert(this->arrow.is_null());
  }

  // ======= C0' nullptr (allow nullptr implicit conversion) ===============
  // NOLINTNEXTLINE
  relation_ptr(std::nullptr_t t) {
    // this->arrow.is_owned_by_node = false;
    this->arrow = TArrowV1<X>{};
    assert(arrow.is_null());
  }

  // ======= C1' - spointer constructor =======
  // 1. will store T* t owned by new local shared_ptr 'ref'
  // 2. will create a new TNode , also carrying shared_ptr 'ref'
  // 3. will create a new Tree and point
  relation_ptr(T* t, const relation_pool<DOF>& _pool)
      : ctx{_pool.getContext()} {
    assert(get_ctx());  // REMOVE! (allow with better logic than assert)
    //
    bool is_owned_by_node{false};
    this->debug_flag_ptr = get_ctx()->debug();
    // KEEP LOCAL!
    sptr<TNodeData> ref;
    if (t) ref = TNodeData::make_sptr<T>(t);
    //
    if (!ref) return;  // SHOULD NOT CREATE A NEW TREE!

    //
    if (debug()) {
      std::cout
          << "C1' pointer constructor: creating NEW relation_ptr (this_new="
          << this << " to t*=" << t << ") ";
      if (ref)
        std::cout << "with ref -> " << *ref << std::endl;
      else
        std::cout << "with ref -> nullptr" << std::endl;
    }
    //
    // using op1: we only store weak reference here
    this->arrow = get_ctx()->op1_addNodeToNewTree(ref);
    assert(this->arrow.is_owned_by_node == is_owned_by_node);
    // this->remote_node = target;
    //
    if (debug()) {
      get_ctx()->print();
      std::cout << " -> finished C1' pointer constructor" << std::endl;
    }
  }

  // ======= C1 - spointer constructor =======
  // 1. will store T* t owned by new local shared_ptr 'ref'
  // 2. will create a new TNode , also carrying shared_ptr 'ref'
  // 3. will create a new Tree and point
  relation_ptr(T* t, wptr<DOF> _ctx) : ctx{_ctx} {
    assert(get_ctx());  // REMOVE! (allow with better logic than assert)
    bool is_owned_by_node{false};
    //
    this->debug_flag_ptr = get_ctx()->debug();
    // KEEP LOCAL!
    sptr<TNodeData> ref;
    if (t) ref = TNodeData::make_sptr<T>(t);
    //
    if (!ref) return;  // SHOULD NOT CREATE A NEW TREE!

    //
    if (debug()) {
      std::cout
          << "C1 pointer constructor: creating NEW relation_ptr (this_new="
          << this << " to t*=" << t << ") ";
      if (ref)
        std::cout << "with ref -> " << *ref << std::endl;
      else
        std::cout << "with ref -> nullptr" << std::endl;
    }
    //
    // using op1: we only store weak reference here
    this->arrow = get_ctx()->op1_addNodeToNewTree(ref);
    assert(this->arrow.is_owned_by_node == is_owned_by_node);
    // this->remote_node = target;
    //
    if (debug()) {
      get_ctx()->print();
      std::cout << " -> finished C1 pointer constructor" << std::endl;
    }
  }

  // C2 CONSTRUCTOR - EQUIVALENT TO C1+C4
  relation_ptr(T* t, const relation_ptr<T>& owner) : ctx{owner.ctx} {
    // context must exist
    assert(get_ctx());
    //
    this->debug_flag_ptr = get_ctx()->debug();
    //
    // KEEP LOCAL!
    sptr<TNodeData> ref;
    if (t) ref = TNodeData::make_sptr<T>(t);
    //

    //
    if (debug()) {
      std::cout
          << "C2 pointer constructor: creating NEW relation_ptr (this_new="
          << this << " to t*=" << t << ") ";
      if (ref)
        std::cout << "with ref -> " << *ref << std::endl;
      else
        std::cout << "with ref -> nullptr" << std::endl;
    }
    //
    if (!ref) return;  // SHOULD NOT CREATE A NEW TREE!

    // WE NEED TO HOLD SPTR locally, UNTIL we store it in definitive sptr...
    // this 'remote_node' is weak!
    // auto sptr_remote_node = !t ? nullptr : sptr<TNode<X>>(new TNode<X>{ref});
    // we only hold weak reference here
    // this->remote_node = sptr_remote_node;
    // this->is_owned_by_node = false;
    //

    //
    // END C1 PART
    //
    // BEGIN C4 PART
    //
    assert(!owner.arrow.is_null());
    // remember ownership (for future deletion?)
    // this->owned_by_node = owner.remote_node;
    //     this->is_owned_by_node = true;
    // OWNER MUST EXIST... AT LEAST NOW!
    // assert(this->owned_by_node.lock());
    // auto myNewParent = owned_by_node.lock();
    //
    // register STRONG ownership in tree
    //
    // auto sptr_mynode = this->remote_node.lock();
    // auto [orig, target] =
    this->arrow =
        get_ctx()->op2_addChildStrong(owner.arrow.remote_node.lock(), ref);
    // TODO(igormcoelho): no errors possible in op2?
    bool is_owned_by_node = true;
    assert(this->arrow.is_owned_by_node == is_owned_by_node);
    // this->owned_by_node = orig;
    // this->remote_node = target;
    //
    if (debug())
      std::cout << "finish c2: stored owner in owned_by_node" << std::endl;
  }

 private:
  //
  // NO COPY CONSTRUCTOR
  //
  // ======= C3 copy constructor =======
  relation_ptr(const relation_ptr<T>& copy) = delete;

 public:
  // ======= M1 move constructor =======
  // simply move smart pointer to all elements: ctx, ref and remote_node
  //
  // IMPORTANT: allow conversion from any type U, such that U* converts to T*
  //
  template <class U, class = typename std::enable_if<
                         std::is_convertible<U*, T*>::value, void>::type>
  relation_ptr(relation_ptr<U>&& corpse) noexcept
      : ctx{corpse.ctx},
        // remote_node{std::move(corpse.remote_node)},
        // owned_by_node{std::move(corpse.owned_by_node)},
        // is_owned_by_node{corpse.is_owned_by_node},
        arrow{std::move(corpse.arrow)} {
    // corpse.remote_node.reset();
    // corpse.owned_by_node.reset();
    // corpse.is_owned_by_node = false;
    if (get_ctx()) this->debug_flag_ptr = get_ctx()->debug();
  }

 public:
  // ======= C4 copy constructor WITH owner =======
  // copy constructor (still good for vector... must be the meaning of a
  // "copy") proposed operation is: cptr1 = relation_ptr<T>(cptr0, cptr2); //
  // (cptr0 and cptr2 exists already) it could also follow this logic:
  // 1. cptr1 = relation_ptr<T>{cptr0}; // copy cptr0 into cptr1
  // 2. cptr1.set_owned_by(cptr2);   // makes cptr1 (and also cptr0) owned by
  // cptr2
  //
  relation_ptr(const relation_ptr<T>& copy, const relation_ptr<T>& owner)
      : ctx{copy.ctx}  //, remote_node{copy.remote_node} {
  {
    // context must exist
    assert(get_ctx());  // TODO: avoid assert here
    // same pointers on ctx
    // TODO: this assert could be necessary indeed...
    assert(get_ctx().get() == owner.get_ctx().get());
    //
    this->debug_flag_ptr = get_ctx()->debug();
    if (debug()) std::cout << "c4 constructor for get_owned" << std::endl;
    assert(this != &copy);   // IMPOSSIBLE
    assert(this != &owner);  // IMPOSSIBLE
    //
    assert(!owner.arrow.is_null());  // TODO: avoid assert here
    //
    // both nodes exist already (copy node and owner node)
    // register WEAK ownership in tree
    // this->set_owned_by(owner);  // invokes 'op3_weakSetOwnedBy'

    // it seems that best logic is:
    // - each weak owned_by link corresponds to weak owns link
    // - each strong child link corresponds to a weak parent link
    //
    // auto [orig, target]
    // this->arrow = get_ctx()->op3_weakSetOwnedBy(copy.remote_node.lock(),
    //                                             owner.remote_node.lock());
    this->arrow = get_ctx()->op3_weakSetOwnedBy(copy.arrow.remote_node.lock(),
                                                owner.arrow.remote_node.lock());

    // remember ownership (for future deletion?)
    // this->remote_node = target;
    // this->owned_by_node = orig;
    //
    // this->owned_by_node = owner.remote_node;
    bool is_owned_by_node = true;
    assert(this->arrow.is_owned_by_node == is_owned_by_node);
    // OWNER MUST EXIST... AT LEAST NOW!
    assert(this->arrow.owned_by_node.lock());  // TODO: crazy test.. why?
    if (debug())
      std::cout << "finish c4: stored owner in owned_by_node" << std::endl;
  }

 private:
  int get_ref_use_count() const { return this->get_shared().use_count(); }

 public:
  void destroy() {
    if (debug()) std::cout << "destroy: BEGIN" << std::endl;
    // TODO(igormcoelho): how to manage "delegated sptr" pointers here?
    // Maybe just consider some "unique_ptr forest" for now?
    //
    if (debug()) {
      std::cout << "destroy: ref_use_count=" << this->get_ref_use_count();
      if (!has_get())
        std::cout << " get(): NULL";
      else
        std::cout << " get(): (" << this->get() << ")";
      std::cout << std::endl;
    }
    // BEGIN complex logic
    // where is nullptr here? TODO: fix
    assert(this->arrow.is_null() || this->arrow.is_root() ||
           this->arrow.is_owned());
    //
    if (this->arrow.is_null()) {
      if (debug()) std::cout << "destroy: is_null()" << std::endl;
      return;  // nothing to destroy
               // (MUST KEEP else below, otherwise it may break(?))
    } else {
      //
      bool isRoot = this->arrow.is_root();
      bool isOwned = this->arrow.is_owned();
      //
      assert(isRoot || isOwned);
      //
      if (debug()) std::cout << "destroy: is_root() || is_owned()" << std::endl;

      this->get_ctx()->op4_remove(this->arrow, isRoot, isOwned);

      //
      // end-if is_root || is_owned
    }

    if (debug()) std::cout << "destroy: last cleanups" << std::endl;
    //
    this->arrow = TArrowV1<X>{};  // CLEAR
    bool is_owned_by_node = false;
    assert(this->arrow.is_owned_by_node == is_owned_by_node);
//
#ifdef WEAK_POOL_PTR
    // nothing to do
#else
    // reset shared structure for context
    this->ctx = nullptr;
#endif
    //
    if (debug()) std::cout << "destroy: END" << std::endl;
  }

  void reset() {
    if (debug()) std::cout << "relation_ptr::reset() BEGIN" << std::endl;
    destroy();
    if (debug()) std::cout << "relation_ptr::reset() END" << std::endl;
  }

  ~relation_ptr() {
    if (debug())
      std::cout << "begin ~relation_ptr() getType=" << this->arrow.getType()
                << std::endl;
    destroy();
    if (debug()) std::cout << "end ~relation_ptr()" << std::endl;
  }

  // TODO: remove this method!
  int count_owned_by() const {
    auto node_ptr = this->arrow.remote_node.lock();
    assert(node_ptr);
    // AVOID direct usage of TNode here...
    return node_ptr->owned_by.size();
    // return this->get_ctx()->opx_countOwnedBy(node_ptr);
  }

  // TODO: remove this method!
  auto getOwnedBy(int idx) const {
    auto node_ptr = this->arrow.remote_node.lock();
    assert(node_ptr);
    // AVOID direct usage of TNode here...
    // return node_ptr->owned_by[idx].lock();
    return node_ptr->owned_by.at(idx).lock();
  }

 private:
  // no copy assignment
  relation_ptr& operator=(const relation_ptr& other) = delete;

 public:
  relation_ptr& operator=(relation_ptr&& corpse) noexcept {
    if (debug()) std::cout << "begin operator==(&&)" << std::endl;
    destroy();
    if (debug()) std::cout << "will move assign" << std::endl;
    this->ctx = std::move(corpse.ctx);
    this->debug_flag_ptr = corpse.debug_flag_ptr;
    // this->remote_node = std::move(corpse.remote_node);
    // this->owned_by_node = std::move(corpse.owned_by_node);
    this->arrow = std::move(corpse.arrow);
    // this->is_owned_by_node = std::move(corpse.is_owned_by_node);
    if (debug()) std::cout << "end operator==(&&)" << std::endl;

    return *this;
  }

  // =============== BASE OPERATIONS ===============

  // get_owned: returns a relation_ptr pointing to owner's data and setup
  // ownership relationship to 'owner'
  //
  // example: b = a.get_owned(c);
  // b is a copy of a's pointer, and relationship c->b is created
  // (so relation "c owns a", c->a, is kept on object b)
  //
  auto get_owned(const relation_ptr<T>& owner) const {
    // C4 constructor
    // NOTE: this cannot be nullptr
    assert(!this->arrow.is_null());
    // NOTE: owner cannot be nullptr
    assert(!owner.arrow.is_null());
    // int sz_owns = owner.arrow.remote_node.lock()->owns.size();
    auto r = relation_ptr<T>(*this, owner);
    // assert(sz_owns + 1 == owner.arrow.remote_node.lock()->owns.size());
    //
    return r;
  }

  // create self-owned reference (similar to "weak reference")
  auto get_self_owned() const {
    auto self_ptr = this->get_owned(*this);
    return self_ptr;
  }

  // NOT implemented yet
  auto get_unowned() {
    // cannot get pointer from null
    if (this->arrow.is_null()) {
      // return null
      return relation_ptr<T>{};
    }
    // cannot currently make copies of unowned
    if (this->arrow.is_root()) {
      // return null
      return relation_ptr<T>{};
    }
    // this must be owned
    assert(this->arrow.is_owned());
    std::cout << "ERROR: must implement unowned from owned logic!" << std::endl;
    assert(false);
    return relation_ptr<T>{};
  }

  bool operator==(const relation_ptr<T>& other) const {
    // do not comparing null pointers as 'true' (why?)... just feels like
    // right now. (thinking more of refs than pointers) (this->has_get() &&
    // other.has_get()) &&
    // TODO: think more.
    return (this->has_get() == other.has_get()) &&
           (get_ctx() == other.get_ctx()) &&
           (get() == other.get());  //&& (ref == other.ref);
  }

 private:
  // same as operator bool()
  bool has_get() const noexcept {
    // NOLINTNEXTLINE
    return (bool)get();
  }

 public:
  // this typically replaces 'has_get'
  explicit operator bool() const noexcept { return has_get(); }

  sptr<T> get_shared() const {
    // assert(!is_null());
    auto sremote_node = this->arrow.remote_node.lock();
    if (!sremote_node) {
      return nullptr;
    } else {
      // TODO: how to make this simpler?
      //
      // return sremote_node->value;
      //
      /*
      sptr<TNodeData> tdata = sremote_node->value;
      T* ptr_t = (T*)tdata->p;
      */
      //
      // delegate liveness of sptr<TNodeData> to sptr<T>
      //
      // NOLINTNEXTLINE
      return sptr<T>{sremote_node->value, (T*)(sremote_node->value->p)};
    }
  }

  T* get() const {
    auto mysptr = get_shared();
    if (!mysptr)
      return nullptr;
    else
      return mysptr.get();
  }

  // typical navigation operator
  T* operator->() const { return get(); }

 public:
  template <class... Args>
  static relation_ptr<T> make(sptr<DynowForestV1> ctx, Args&&... args) {
    // NOLINTNEXTLINE
    auto* t = new T(std::forward<Args>(args)...);
    return relation_ptr<T>{t, ctx};
  }
};

}  // namespace cycles

#endif  // CYCLES_RELATION_PTR_HPP_ // NOLINT

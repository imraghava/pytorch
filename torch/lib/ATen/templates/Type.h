#pragma once

#include <memory>

#include "ATen/ArrayRef.h"
#include "ATen/Half.h"

namespace at {

class Context;
struct Storage;
struct Tensor;
class Scalar;
struct Generator;

#define AT_FORALL_SCALAR_TYPES(_) \
_(uint8_t,Byte,i) \
_(int8_t,Char,i) \
_(double,Double,d) \
_(float,Float,d) \
_(int,Int,i) \
_(int64_t,Long,i) \
_(int16_t,Short,i) \
_(Half,Half,d)

enum class ScalarType {
#define DEFINE_ENUM(_1,n,_2) \
  n,
  AT_FORALL_SCALAR_TYPES(DEFINE_ENUM)
#undef DEFINE_ENUM
  NumOptions
};

enum class Backend {
  CPU,
  CUDA,
  SparseCPU,
  SparseCUDA,
  NumOptions
};


constexpr Backend kCPU = Backend::CPU;
constexpr Backend kCUDA = Backend::CUDA;
constexpr Backend kSparseCPU = Backend::SparseCPU;
constexpr Backend kSparseCUDA = Backend::SparseCUDA;

static inline const char * toString(Backend b) {
  switch(b) {
    case Backend::CPU: return "CPU";
    case Backend::CUDA: return "CUDA";
    case Backend::SparseCPU: return "SparseCPU";
    case Backend::SparseCUDA: return "SparseCUDA";
    default: return "UNKNOWN_BACKEND";
  }
}

#define DEFINE_CONSTANT(_,name,_2) \
constexpr ScalarType k##name = ScalarType::name;

AT_FORALL_SCALAR_TYPES(DEFINE_CONSTANT)
#undef DEFINE_CONSTANT

static inline const char * toString(ScalarType t) {
#define DEFINE_CASE(_,name,_2) \
  case ScalarType:: name : return #name;

  switch(t) {
    AT_FORALL_SCALAR_TYPES(DEFINE_CASE)
    default:
      return "UNKNOWN_SCALAR_TYPE";
  }
#undef DEFINE_CASE
}

enum class TypeID {
  ${type_ids}
  NumOptions
};


typedef ArrayRef<int64_t> IntList;

struct Type {
  Type(Context * context)
  : context(context) {}
  virtual ScalarType scalarType() = 0;
  virtual Backend backend() = 0;
  virtual bool isSparse() = 0;
  virtual bool isDistributed() = 0;
  static void registerAll(Context * context);
  virtual std::unique_ptr<Storage> storage() = 0;
  virtual std::unique_ptr<Storage> storage(size_t size) = 0;
  virtual std::unique_ptr<Storage> storageFromBlob(void * data, int64_t size) = 0;
  virtual std::unique_ptr<Generator> generator() = 0;
  virtual Tensor unsafeTensorFromTH(void * th_pointer) = 0;
  virtual const char * toString() const = 0;
  Type & toBackend(Backend b);
  Type & toScalarType(ScalarType s);

  // contingious IDs for all types in the system
  // for external dispatch
  virtual TypeID ID() const = 0;

  virtual void copy(const Tensor & src, Tensor & dst) = 0;
  Tensor copy(const Tensor & src);

  Tensor tensorFromBlob(void * data, IntList sizes);
  Tensor tensorFromBlob(void * data, IntList sizes, IntList strides);
  Tensor scalarTensor(Scalar s);
  // example
  // virtual Tensor * add(Tensor & a, Tensor & b) = 0;
  ${type_method_declarations}
protected:
  Context* context;
};


}

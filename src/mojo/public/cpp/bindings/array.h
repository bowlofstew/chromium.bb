// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_PUBLIC_CPP_BINDINGS_ARRAY_H_
#define MOJO_PUBLIC_CPP_BINDINGS_ARRAY_H_

#include <string.h>

#include <algorithm>
#include <string>
#include <vector>

#include "mojo/public/cpp/bindings/lib/array_internal.h"
#include "mojo/public/cpp/bindings/type_converter.h"

namespace mojo {

// Provides read-only access to array data.
template <typename T>
class Array {
 public:
  typedef internal::ArrayTraits<T,
                                internal::TypeTraits<T>::kIsObject,
                                internal::TypeTraits<T>::kIsHandle> Traits_;
  typedef typename Traits_::DataType Data;
  typedef typename Traits_::ConstRef ConstRef;

  Array() : data_(NULL) {
  }

  template <typename U>
  Array(const U& u, Buffer* buf = Buffer::current()) {
    TypeConverter<Array<T>,U>::AssertAllowImplicitTypeConversion();
    *this = TypeConverter<Array<T>,U>::ConvertFrom(u, buf);
  }

  template <typename U>
  Array& operator=(const U& u) {
    TypeConverter<Array<T>,U>::AssertAllowImplicitTypeConversion();
    *this = TypeConverter<Array<T>,U>::ConvertFrom(u, Buffer::current());
    return *this;
  }

  template <typename U>
  operator U() const {
    TypeConverter<Array<T>,U>::AssertAllowImplicitTypeConversion();
    return To<U>();
  }

  template <typename U>
  U To() const {
    return TypeConverter<Array<T>,U>::ConvertTo(*this);
  }

  template <typename U>
  static Array From(const U& u, Buffer* buf = Buffer::current()) {
    return TypeConverter<Array<T>,U>::ConvertFrom(u, buf);
  }

  bool is_null() const { return !data_; }

  size_t size() const { return data_->size(); }

  ConstRef at(size_t offset) const {
    return Traits_::ToConstRef(data_->at(offset));
  }
  ConstRef operator[](size_t offset) const { return at(offset); }

  // Provides a way to initialize an array element-by-element.
  class Builder {
   public:
    typedef typename Array<T>::Data Data;
    typedef typename Array<T>::Traits_ Traits_;
    typedef typename Traits_::Ref Ref;

    explicit Builder(size_t num_elements, Buffer* buf = mojo::Buffer::current())
        : data_(Data::New(num_elements, buf, Traits_::GetDestructor())) {
    }

    size_t size() const { return data_->size(); }

    Ref at(size_t offset) {
      return Traits_::ToRef(data_->at(offset));
    }
    Ref operator[](size_t offset) { return at(offset); }

    Array<T> Finish() {
      Data* data = NULL;
      std::swap(data, data_);
      return internal::Wrap(data);
    }

   private:
    Data* data_;
    MOJO_DISALLOW_COPY_AND_ASSIGN(Builder);
  };

 protected:
  friend class internal::WrapperHelper<Array<T> >;

  struct Wrap {};
  Array(Wrap, const Data* data) : data_(data) {}

  const Data* data_;
};

// UTF-8 encoded
typedef Array<char> String;

template <>
class TypeConverter<String, std::string> {
 public:
  static String ConvertFrom(const std::string& input, Buffer* buf);
  static std::string ConvertTo(const String& input);

  MOJO_ALLOW_IMPLICIT_TYPE_CONVERSION();
};

template <size_t N>
class TypeConverter<String, char[N]> {
 public:
  static String ConvertFrom(const char input[N], Buffer* buf) {
    String::Builder result(N - 1, buf);
    memcpy(&result[0], input, N - 1);
    return result.Finish();
  }

  MOJO_ALLOW_IMPLICIT_TYPE_CONVERSION();
};

// Appease MSVC.
template <size_t N>
class TypeConverter<String, const char[N]> {
 public:
  static String ConvertFrom(const char input[N], Buffer* buf) {
    return TypeConverter<String, char[N]>::ConvertFrom(input, buf);
  }

  MOJO_ALLOW_IMPLICIT_TYPE_CONVERSION();
};

template <>
class TypeConverter<String, const char*> {
 public:
  static String ConvertFrom(const char* input, Buffer* buf);
  // NOTE: |ConvertTo| explicitly not implemented since String is not null
  // terminated (and may have embedded null bytes).

  MOJO_ALLOW_IMPLICIT_TYPE_CONVERSION();
};

template <typename T, typename E>
class TypeConverter<Array<T>, std::vector<E> > {
 public:
  static Array<T> ConvertFrom(const std::vector<E>& input, Buffer* buf) {
    typename Array<T>::Builder result(input.size(), buf);
    for (size_t i = 0; i < input.size(); ++i)
      result[i] = TypeConverter<T, E>::ConvertFrom(input[i], buf);
    return result.Finish();
  }
  static std::vector<E> ConvertTo(const Array<T>& input) {
    std::vector<E> result;
    if (!input.is_null()) {
      result.resize(input.size());
      for (size_t i = 0; i < input.size(); ++i)
        result[i] = TypeConverter<T, E>::ConvertTo(input[i]);
    }
    return result;
  }

  MOJO_INHERIT_IMPLICIT_TYPE_CONVERSION(T, E);
};

}  // namespace mojo

#endif  // MOJO_PUBLIC_CPP_BINDINGS_ARRAY_H_

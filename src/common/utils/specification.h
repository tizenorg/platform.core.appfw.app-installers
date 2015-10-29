// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_SPECIFICATION_H_
#define COMMON_UTILS_SPECIFICATION_H_

#include <memory>

namespace common_installer {

template<typename T>
class ISpecification {
 public:
  virtual ~ISpecification() {}

  virtual bool IsSatisfiedBy(const T &candidate) = 0;
  virtual std::shared_ptr<ISpecification<T> > And(
      std::shared_ptr<ISpecification<T> > other) = 0;
  virtual std::shared_ptr<ISpecification<T> > Or(
      std::shared_ptr<ISpecification<T> > other) = 0;
  virtual std::shared_ptr<ISpecification<T> > Not() = 0;
};

template<typename T>
class CompositeSpecification : public ISpecification<T>,
  public std::enable_shared_from_this<CompositeSpecification<T> > {
 public:
  virtual ~CompositeSpecification() {}

  virtual bool IsSatisfiedBy(const T &candidate) = 0;
  std::shared_ptr<ISpecification<T> > And(
      std::shared_ptr<ISpecification<T> > other) override;
  std::shared_ptr<ISpecification<T> > Or(
      std::shared_ptr<ISpecification<T> > other) override;
  std::shared_ptr<ISpecification<T> > Not() override;
};

template<typename T>
class AndSpecification : public CompositeSpecification<T> {
 public:
  AndSpecification(std::shared_ptr<ISpecification<T> > one,
      std::shared_ptr<ISpecification<T> > other);
  virtual ~AndSpecification() {}

  bool IsSatisfiedBy(const T &candidate) override;

 private:
  std::shared_ptr<ISpecification<T> > _one;
  std::shared_ptr<ISpecification<T> > _other;
};

template<typename T>
class OrSpecification : public CompositeSpecification<T> {
 public:
  OrSpecification(std::shared_ptr<ISpecification<T> > one,
      std::shared_ptr<ISpecification<T> > other);
  virtual ~OrSpecification() {}

  bool IsSatisfiedBy(const T &candidate) override;

 private:
  std::shared_ptr<ISpecification<T> > _one;
  std::shared_ptr<ISpecification<T> > _other;
};

template<typename T>
class NotSpecification : public CompositeSpecification<T> {
 public:
  explicit NotSpecification(std::shared_ptr<ISpecification<T> > one);
  virtual ~NotSpecification() {}

  bool IsSatisfiedBy(const T &candidate) override;

 private:
  std::shared_ptr<ISpecification<T> > _wrap;
};

template<typename T> std::shared_ptr<ISpecification<T> >
CompositeSpecification<T>::And(std::shared_ptr<ISpecification<T> > other) {
  return std::shared_ptr<AndSpecification<T> >(
      new AndSpecification<T>(this->shared_from_this(), other));
}

template<typename T> std::shared_ptr<ISpecification<T> >
CompositeSpecification<T>::Or(std::shared_ptr<ISpecification<T> > other) {
  return std::shared_ptr<ISpecification<T> >(
      new OrSpecification<T>(this->shared_from_this(), other));
}

template<typename T> std::shared_ptr<ISpecification<T> >
CompositeSpecification<T>::Not() {
  return std::shared_ptr<ISpecification<T> >(
      new NotSpecification<T>(this->shared_from_this()));
}

template<typename T> AndSpecification<T>::AndSpecification(
    std::shared_ptr<ISpecification<T> > one,
    std::shared_ptr<ISpecification<T> > other) :
        _one(one), _other(other) { }

template<typename T> bool AndSpecification<T>::IsSatisfiedBy(
    const T &candidate) {
  return _one->IsSatisfiedBy(candidate) && _other->IsSatisfiedBy(candidate);
}

template<typename T> OrSpecification<T>::OrSpecification(
    std::shared_ptr<ISpecification<T> > one,
    std::shared_ptr<ISpecification<T> > other) :
            _one(one), _other(other) { }

template<typename T> bool OrSpecification<T>::IsSatisfiedBy(
    const T &candidate) {
  return _one->IsSatisfiedBy(candidate) || _other->IsSatisfiedBy(candidate);
}

template<typename T> NotSpecification<T>::NotSpecification(
    std::shared_ptr<ISpecification<T> > one) : _wrap(one) { }

template<typename T> bool NotSpecification<T>::IsSatisfiedBy(
    const T &candidate) {
  return !_wrap->IsSatisfiedBy(candidate);
}

}  // namespace common_installer

#endif  // COMMON_UTILS_SPECIFICATION_H_

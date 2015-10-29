// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_SPECIFICATION_H_
#define COMMON_UTILS_SPECIFICATION_H_

#include <memory>

namespace common_installer {

template<typename T>
class ISpecification;

template<typename T>
using ISpecPtr = std::shared_ptr<ISpecification<T>>;

/**
 * \brief Interface that support chain and nested logical operations
 */
template<typename T>
class ISpecification {
 public:
  virtual ~ISpecification() {}

  /**
   * \brief Declaration of method that check if condition is meet
   *        for candidate in order to utilize it's result in
   *        logical operations AND, OR, and NOT
   *
   * \param candidate against with test will be performed
   *
   * \return true if condition is satisfied by candidate, false
   *        otherwise
   */
  virtual bool IsSatisfiedBy(const T &candidate) = 0;

  /**
   * \brief Declaration of AND operation, condition has following form
   *        this AND other
   *
   * \param other Specification object to allow nested conditions
   *
   * \return Specification object to allow method chaining. Is expected to
   *         return AndSpecification implementation.
   */
  virtual ISpecPtr<T> And(ISpecPtr<T> other) = 0;

  /**
   * \brief Declaration of OR operation, condition has following form
   *        this OR other
   *
   * \param other Specification object to allow nested conditions
   *
   * \return Specification object to allow method chaining. Is expected
   *         to return OrSpecification implementation.
   */
  virtual ISpecPtr<T> Or(ISpecPtr<T> other) = 0;

  /**
   * \brief Declaration of Not operation, condition has following form
   *        !this
   *
   * \return Specification object to allow method chaining. Is expected
   *         to return NotSpecification implementation.
   */
  virtual ISpecPtr<T> Not() = 0;
};

/**
 * \brief Abstract class that implement basic logical operations
 *        AND, OR, and NOT of ISpecification interface. It is expected to
 *        derive from this class when new specification is needed. Deriving
 *        class have to implement IsSatifiedBy method.
 */
template<typename T>
class CompositeSpecification : public ISpecification<T>,
  public std::enable_shared_from_this<CompositeSpecification<T> > {
 public:
  virtual ~CompositeSpecification() {}

  virtual bool IsSatisfiedBy(const T &candidate) = 0;

  /**
   * \brief Implementation of AND operation
   */
  ISpecPtr<T> And(ISpecPtr<T> other) override;

  /**
   * \brief Implementation of OR operation
   */
  ISpecPtr<T> Or(ISpecPtr<T> other) override;

  /**
   * \brief Implementation of NOT operation
   */
  ISpecPtr<T> Not() override;
};

/**
 * \brief Implementation of AND operation
 */
template<typename T>
class AndSpecification : public CompositeSpecification<T> {
 public:
  explicit AndSpecification(ISpecPtr<T> one, ISpecPtr<T> other);
  virtual ~AndSpecification() {}

  bool IsSatisfiedBy(const T &candidate) override;

 private:
  ISpecPtr<T> one_;
  ISpecPtr<T> other_;
};

/**
 * \brief Implementation of OR operation
 */
template<typename T>
class OrSpecification : public CompositeSpecification<T> {
 public:
  explicit OrSpecification(ISpecPtr<T> one, ISpecPtr<T> other);
  virtual ~OrSpecification() {}

  bool IsSatisfiedBy(const T &candidate) override;

 private:
  ISpecPtr<T> one_;
  ISpecPtr<T> other_;
};

/**
 * \brief Implementation of NOT operation
 */
template<typename T>
class NotSpecification : public CompositeSpecification<T> {
 public:
  explicit NotSpecification(ISpecPtr<T> one);
  virtual ~NotSpecification() {}

  bool IsSatisfiedBy(const T &candidate) override;

 private:
  ISpecPtr<T> wrap_;
};

template<typename T>
ISpecPtr<T> CompositeSpecification<T>::And(ISpecPtr<T> other) {
  return std::make_shared<AndSpecification<T>>(this->shared_from_this(), other);
}

template<typename T>
ISpecPtr<T> CompositeSpecification<T>::Or(ISpecPtr<T> other) {
  return std::make_shared<OrSpecification<T>>(this->shared_from_this(), other);
}

template<typename T>
ISpecPtr<T> CompositeSpecification<T>::Not() {
  return std::make_shared<NotSpecification<T>>(this->shared_from_this());
}

template<typename T> AndSpecification<T>::AndSpecification(
    ISpecPtr<T> one, ISpecPtr<T> other) : one_(one), other_(other) { }

template<typename T> bool AndSpecification<T>::IsSatisfiedBy(
    const T &candidate) {
  return one_->IsSatisfiedBy(candidate) && other_->IsSatisfiedBy(candidate);
}

template<typename T> OrSpecification<T>::OrSpecification(
    ISpecPtr<T> one, ISpecPtr<T> other) : one_(one), other_(other) { }

template<typename T> bool OrSpecification<T>::IsSatisfiedBy(
    const T &candidate) {
  return one_->IsSatisfiedBy(candidate) || other_->IsSatisfiedBy(candidate);
}

template<typename T> NotSpecification<T>::NotSpecification(
    ISpecPtr<T> one) : wrap_(one) { }

template<typename T> bool NotSpecification<T>::IsSatisfiedBy(
    const T &candidate) {
  return !wrap_->IsSatisfiedBy(candidate);
}

}  // namespace common_installer

#endif  // COMMON_UTILS_SPECIFICATION_H_

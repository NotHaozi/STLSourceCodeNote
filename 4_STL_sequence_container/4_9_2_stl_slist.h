/*
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_SLIST_H
#define __SGI_STL_INTERNAL_SLIST_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

// 单向串行的节点基本结构
struct _Slist_node_base
{
  _Slist_node_base* _M_next;
};

// 插入新节点于已知节点后面
inline _Slist_node_base*
__slist_make_link(_Slist_node_base* __prev_node,
                  _Slist_node_base* __new_node)
{
  __new_node->_M_next = __prev_node->_M_next;
  __prev_node->_M_next = __new_node;
  return __new_node;
}

inline _Slist_node_base* 
__slist_previous(_Slist_node_base* __head,
                 const _Slist_node_base* __node)
{
  while (__head && __head->_M_next != __node)
    __head = __head->_M_next;
  return __head;
}

inline const _Slist_node_base* 
__slist_previous(const _Slist_node_base* __head,
                 const _Slist_node_base* __node)
{
  while (__head && __head->_M_next != __node)
    __head = __head->_M_next;
  return __head;
}

inline void __slist_splice_after(_Slist_node_base* __pos,
                                 _Slist_node_base* __before_first,
                                 _Slist_node_base* __before_last)
{
  if (__pos != __before_first && __pos != __before_last) {
    _Slist_node_base* __first = __before_first->_M_next;
    _Slist_node_base* __after = __pos->_M_next;
    __before_first->_M_next = __before_last->_M_next;
    __pos->_M_next = __first;
    __before_last->_M_next = __after;
  }
}

inline void
__slist_splice_after(_Slist_node_base* __pos, _Slist_node_base* __head)
{
  _Slist_node_base* __before_last = __slist_previous(__head, 0);
  if (__before_last != __head) {
    _Slist_node_base* __after = __pos->_M_next;
    __pos->_M_next = __head->_M_next;
    __head->_M_next = 0;
    __before_last->_M_next = __after;
  }
}

inline _Slist_node_base* __slist_reverse(_Slist_node_base* __node)
{
  _Slist_node_base* __result = __node;
  __node = __node->_M_next;
  __result->_M_next = 0;
  while(__node) {
    _Slist_node_base* __next = __node->_M_next;
    __node->_M_next = __result;
    __result = __node;
    __node = __next;
  }
  return __result;
}

// 元素个数
inline size_t __slist_size(_Slist_node_base* __node)
{
  size_t __result = 0;
  for ( ; __node != 0; __node = __node->_M_next)
    ++__result;
  return __result;
}

// 单向串行的节点结构
template <class _Tp>
struct _Slist_node : public _Slist_node_base
{
  _Tp _M_data;
};

// 单向串行的迭代器基本结构
struct _Slist_iterator_base
{
  typedef size_t               size_type;
  typedef ptrdiff_t            difference_type;
  typedef forward_iterator_tag iterator_category; // 单向

  _Slist_node_base* _M_node; // 指向节点基本结构

  _Slist_iterator_base(_Slist_node_base* __x) : _M_node(__x) {}
  void _M_incr() { _M_node = _M_node->_M_next; }

  bool operator==(const _Slist_iterator_base& __x) const {
    return _M_node == __x._M_node;
  }
  bool operator!=(const _Slist_iterator_base& __x) const {
    return _M_node != __x._M_node;
  }
};

// 单向串行的迭代器结构
template <class _Tp, class _Ref, class _Ptr>
struct _Slist_iterator : public _Slist_iterator_base
{
  typedef _Slist_iterator<_Tp, _Tp&, _Tp*>             iterator;
  typedef _Slist_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
  typedef _Slist_iterator<_Tp, _Ref, _Ptr>             _Self;

  typedef _Tp              value_type;
  typedef _Ptr             pointer;
  typedef _Ref             reference;
  typedef _Slist_node<_Tp> _Node;

  _Slist_iterator(_Node* __x) : _Slist_iterator_base(__x) {}
  //呼叫 slist<T>::end()时会造成 __slist_iterator(0)，于是唤起上述函式。
  // 看不懂上面的注解
  _Slist_iterator() : _Slist_iterator_base(0) {}
  _Slist_iterator(const iterator& __x) : _Slist_iterator_base(__x._M_node) {}

  reference operator*() const { return ((_Node*) _M_node)->_M_data; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  _Self& operator++()
  {
    _M_incr(); // 前进一个节点
    return *this;
  }
  _Self operator++(int)
  {
    _Self __tmp = *this;
    _M_incr();
    return __tmp;
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

inline ptrdiff_t* distance_type(const _Slist_iterator_base&) {
  return 0;
}

inline forward_iterator_tag iterator_category(const _Slist_iterator_base&) {
  return forward_iterator_tag();
}

template <class _Tp, class _Ref, class _Ptr> 
inline _Tp* value_type(const _Slist_iterator<_Tp, _Ref, _Ptr>&) {
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Base class that encapsulates details of allocators.  Three cases:
// an ordinary standard-conforming allocator, a standard-conforming
// allocator with no non-static data, and an SGI-style allocator.
// This complexity is necessary only because we're worrying about backward
// compatibility and because we want to avoid wasting storage on an 
// allocator instance if it isn't necessary.

#ifdef __STL_USE_STD_ALLOCATORS

// Base for general standard-conforming allocators.
template <class _Tp, class _Allocator, bool _IsStatic>
class _Slist_alloc_base {
public:
  typedef typename _Alloc_traits<_Tp,_Allocator>::allocator_type
          allocator_type;
  allocator_type get_allocator() const { return _M_node_allocator; }

  _Slist_alloc_base(const allocator_type& __a) : _M_node_allocator(__a) {}

protected:
  _Slist_node<_Tp>* _M_get_node() 
    { return _M_node_allocator.allocate(1); }
  void _M_put_node(_Slist_node<_Tp>* __p) 
    { _M_node_allocator.deallocate(__p, 1); }

protected:
  typename _Alloc_traits<_Slist_node<_Tp>,_Allocator>::allocator_type
           _M_node_allocator;
  _Slist_node_base _M_head;
};

// Specialization for instanceless allocators.
template <class _Tp, class _Allocator>
class _Slist_alloc_base<_Tp,_Allocator, true> {
public:
  typedef typename _Alloc_traits<_Tp,_Allocator>::allocator_type
          allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _Slist_alloc_base(const allocator_type&) {}

protected:
  typedef typename _Alloc_traits<_Slist_node<_Tp>, _Allocator>::_Alloc_type
          _Alloc_type;
  _Slist_node<_Tp>* _M_get_node() { return _Alloc_type::allocate(1); }
  void _M_put_node(_Slist_node<_Tp>* __p) { _Alloc_type::deallocate(__p, 1); }

protected:
  _Slist_node_base _M_head;
};


template <class _Tp, class _Alloc>
struct _Slist_base
  : public _Slist_alloc_base<_Tp, _Alloc,
                             _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
  typedef _Slist_alloc_base<_Tp, _Alloc,
                            _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
          _Base;
  typedef typename _Base::allocator_type allocator_type;

  _Slist_base(const allocator_type& __a)
    : _Base(__a) { this->_M_head._M_next = 0; }
  ~_Slist_base() { _M_erase_after(&this->_M_head, 0); }

protected:

  _Slist_node_base* _M_erase_after(_Slist_node_base* __pos)
  {
    _Slist_node<_Tp>* __next = (_Slist_node<_Tp>*) (__pos->_M_next);
    _Slist_node_base* __next_next = __next->_M_next;
    __pos->_M_next = __next_next;
    destroy(&__next->_M_data);
    _M_put_node(__next);
    return __next_next;
  }
  _Slist_node_base* _M_erase_after(_Slist_node_base*, _Slist_node_base*);
};

#else /* __STL_USE_STD_ALLOCATORS */

template <class _Tp, class _Alloc> 
struct _Slist_base {
  typedef _Alloc allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _Slist_base(const allocator_type&) { _M_head._M_next = 0; }
  ~_Slist_base() { _M_erase_after(&_M_head, 0); }

protected:
  typedef simple_alloc<_Slist_node<_Tp>, _Alloc> _Alloc_type;
  _Slist_node<_Tp>* _M_get_node() { return _Alloc_type::allocate(1); }
  void _M_put_node(_Slist_node<_Tp>* __p) { _Alloc_type::deallocate(__p, 1); }

  _Slist_node_base* _M_erase_after(_Slist_node_base* __pos)
  {
    _Slist_node<_Tp>* __next = (_Slist_node<_Tp>*) (__pos->_M_next);
    _Slist_node_base* __next_next = __next->_M_next;
    __pos->_M_next = __next_next;
    destroy(&__next->_M_data);
    _M_put_node(__next);
    return __next_next;
  }
  _Slist_node_base* _M_erase_after(_Slist_node_base*, _Slist_node_base*);

protected:
  _Slist_node_base _M_head; // 头部，不是指针
};  

#endif /* __STL_USE_STD_ALLOCATORS */

template <class _Tp, class _Alloc> 
_Slist_node_base*
_Slist_base<_Tp,_Alloc>::_M_erase_after(_Slist_node_base* __before_first,
                                        _Slist_node_base* __last_node) {
  _Slist_node<_Tp>* __cur = (_Slist_node<_Tp>*) (__before_first->_M_next);
  while (__cur != __last_node) {
    _Slist_node<_Tp>* __tmp = __cur;
    __cur = (_Slist_node<_Tp>*) __cur->_M_next;
    destroy(&__tmp->_M_data);
    _M_put_node(__tmp);
  }
  __before_first->_M_next = __last_node;
  return __last_node;
}

template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class slist : private _Slist_base<_Tp,_Alloc>
{
  // requirements:

  __STL_CLASS_REQUIRES(_Tp, _Assignable);

private:
  typedef _Slist_base<_Tp,_Alloc> _Base;
public:
  typedef _Tp                value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;

  typedef _Slist_iterator<_Tp, _Tp&, _Tp*>             iterator;
  typedef _Slist_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;

  typedef typename _Base::allocator_type allocator_type;
  allocator_type get_allocator() const { return _Base::get_allocator(); }

private:
  typedef _Slist_node<_Tp>      _Node;
  typedef _Slist_node_base      _Node_base;
  typedef _Slist_iterator_base  _Iterator_base;

  _Node* _M_create_node(const value_type& __x) {
    _Node* __node = this->_M_get_node(); // 配置空间
    __STL_TRY {
      construct(&__node->_M_data, __x); // 构造元素
      __node->_M_next = 0;
    }
    __STL_UNWIND(this->_M_put_node(__node));
    return __node;
  }
  
  _Node* _M_create_node() {
    _Node* __node = this->_M_get_node();
    __STL_TRY {
      construct(&__node->_M_data);
      __node->_M_next = 0;
    }
    __STL_UNWIND(this->_M_put_node(__node));
    return __node;
  }

public:
  explicit slist(const allocator_type& __a = allocator_type()) : _Base(__a) {}

  slist(size_type __n, const value_type& __x,
        const allocator_type& __a =  allocator_type()) : _Base(__a)
    { _M_insert_after_fill(&this->_M_head, __n, __x); }

  explicit slist(size_type __n) : _Base(allocator_type())
    { _M_insert_after_fill(&this->_M_head, __n, value_type()); }

#ifdef __STL_MEMBER_TEMPLATES
  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InputIterator>
  slist(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a =  allocator_type()) : _Base(__a)
    { _M_insert_after_range(&this->_M_head, __first, __last); }

#else /* __STL_MEMBER_TEMPLATES */
  slist(const_iterator __first, const_iterator __last,
        const allocator_type& __a =  allocator_type()) : _Base(__a)
    { _M_insert_after_range(&this->_M_head, __first, __last); }
  slist(const value_type* __first, const value_type* __last,
        const allocator_type& __a =  allocator_type()) : _Base(__a)
    { _M_insert_after_range(&this->_M_head, __first, __last); }
#endif /* __STL_MEMBER_TEMPLATES */

  slist(const slist& __x) : _Base(__x.get_allocator())
    { _M_insert_after_range(&this->_M_head, __x.begin(), __x.end()); }

  slist& operator= (const slist& __x);

  ~slist() {}

public:
  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.

  void assign(size_type __n, const _Tp& __val)
    { _M_fill_assign(__n, __val); }

  void _M_fill_assign(size_type __n, const _Tp& __val);


#ifdef __STL_MEMBER_TEMPLATES

  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) __n, (_Tp) __val); }

  template <class _InputIterator>
  void _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
                          __false_type);

#endif /* __STL_MEMBER_TEMPLATES */

public:

  iterator begin() { return iterator((_Node*)this->_M_head._M_next); }
  const_iterator begin() const 
    { return const_iterator((_Node*)this->_M_head._M_next);}

  iterator end() { return iterator(0); }
  const_iterator end() const { return const_iterator(0); }

  // Experimental new feature: before_begin() returns a
  // non-dereferenceable iterator that, when incremented, yields
  // begin().  This iterator may be used as the argument to
  // insert_after, erase_after, etc.  Note that even for an empty 
  // slist, before_begin() is not the same iterator as end().  It 
  // is always necessary to increment before_begin() at least once to
  // obtain end().
  iterator before_begin() { return iterator((_Node*) &this->_M_head); }
  const_iterator before_begin() const
    { return const_iterator((_Node*) &this->_M_head); }

  size_type size() const { return __slist_size(this->_M_head._M_next); }
  
  size_type max_size() const { return size_type(-1); }

  bool empty() const { return this->_M_head._M_next == 0; }

  // 两个slist互换，直接将head交换互指即可
  void swap(slist& __x)
    { __STD::swap(this->_M_head._M_next, __x._M_head._M_next); }

public:

  // 取头部元素
  reference front() { return ((_Node*) this->_M_head._M_next)->_M_data; }
  const_reference front() const 
    { return ((_Node*) this->_M_head._M_next)->_M_data; }
  // 从头部插入元素
  void push_front(const value_type& __x)   {
    __slist_make_link(&this->_M_head, _M_create_node(__x));
  }
  // 插入空元素
  void push_front() { __slist_make_link(&this->_M_head, _M_create_node()); }
  // 从头部取走元素
  void pop_front() {
    _Node* __node = (_Node*) this->_M_head._M_next;
    this->_M_head._M_next = __node->_M_next;
    destroy(&__node->_M_data);
    this->_M_put_node(__node);
  }

  iterator previous(const_iterator __pos) {
    return iterator((_Node*) __slist_previous(&this->_M_head, __pos._M_node));
  }
  const_iterator previous(const_iterator __pos) const {
    return const_iterator((_Node*) __slist_previous(&this->_M_head,
                                                    __pos._M_node));
  }

private:
  _Node* _M_insert_after(_Node_base* __pos, const value_type& __x) {
    return (_Node*) (__slist_make_link(__pos, _M_create_node(__x)));
  }

  _Node* _M_insert_after(_Node_base* __pos) {
    return (_Node*) (__slist_make_link(__pos, _M_create_node()));
  }

  void _M_insert_after_fill(_Node_base* __pos,
                            size_type __n, const value_type& __x) {
    for (size_type __i = 0; __i < __n; ++__i)
      __pos = __slist_make_link(__pos, _M_create_node(__x));
  }

#ifdef __STL_MEMBER_TEMPLATES

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InIter>
  void _M_insert_after_range(_Node_base* __pos, 
                             _InIter __first, _InIter __last) {
    typedef typename _Is_integer<_InIter>::_Integral _Integral;
    _M_insert_after_range(__pos, __first, __last, _Integral());
  }

  template <class _Integer>
  void _M_insert_after_range(_Node_base* __pos, _Integer __n, _Integer __x,
                             __true_type) {
    _M_insert_after_fill(__pos, __n, __x);
  }

  template <class _InIter>
  void _M_insert_after_range(_Node_base* __pos,
                             _InIter __first, _InIter __last,
                             __false_type) {
    while (__first != __last) {
      __pos = __slist_make_link(__pos, _M_create_node(*__first));
      ++__first;
    }
  }

#else /* __STL_MEMBER_TEMPLATES */

  void _M_insert_after_range(_Node_base* __pos,
                             const_iterator __first, const_iterator __last) {
    while (__first != __last) {
      __pos = __slist_make_link(__pos, _M_create_node(*__first));
      ++__first;
    }
  }
  void _M_insert_after_range(_Node_base* __pos,
                             const value_type* __first,
                             const value_type* __last) {
    while (__first != __last) {
      __pos = __slist_make_link(__pos, _M_create_node(*__first));
      ++__first;
    }
  }

#endif /* __STL_MEMBER_TEMPLATES */

public:

  iterator insert_after(iterator __pos, const value_type& __x) {
    return iterator(_M_insert_after(__pos._M_node, __x));
  }

  iterator insert_after(iterator __pos) {
    return insert_after(__pos, value_type());
  }

  void insert_after(iterator __pos, size_type __n, const value_type& __x) {
    _M_insert_after_fill(__pos._M_node, __n, __x);
  }

#ifdef __STL_MEMBER_TEMPLATES

  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InIter>
  void insert_after(iterator __pos, _InIter __first, _InIter __last) {
    _M_insert_after_range(__pos._M_node, __first, __last);
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert_after(iterator __pos,
                    const_iterator __first, const_iterator __last) {
    _M_insert_after_range(__pos._M_node, __first, __last);
  }
  void insert_after(iterator __pos,
                    const value_type* __first, const value_type* __last) {
    _M_insert_after_range(__pos._M_node, __first, __last);
  }

#endif /* __STL_MEMBER_TEMPLATES */

  iterator insert(iterator __pos, const value_type& __x) {
    return iterator(_M_insert_after(__slist_previous(&this->_M_head,
                                                     __pos._M_node),
                    __x));
  }

  iterator insert(iterator __pos) {
    return iterator(_M_insert_after(__slist_previous(&this->_M_head,
                                                     __pos._M_node),
                                    value_type()));
  }

  void insert(iterator __pos, size_type __n, const value_type& __x) {
    _M_insert_after_fill(__slist_previous(&this->_M_head, __pos._M_node),
                         __n, __x);
  } 
    
#ifdef __STL_MEMBER_TEMPLATES

  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <class _InIter>
  void insert(iterator __pos, _InIter __first, _InIter __last) {
    _M_insert_after_range(__slist_previous(&this->_M_head, __pos._M_node), 
                          __first, __last);
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator __pos, const_iterator __first, const_iterator __last) {
    _M_insert_after_range(__slist_previous(&this->_M_head, __pos._M_node), 
                          __first, __last);
  }
  void insert(iterator __pos, const value_type* __first, 
                              const value_type* __last) {
    _M_insert_after_range(__slist_previous(&this->_M_head, __pos._M_node), 
                          __first, __last);
  }

#endif /* __STL_MEMBER_TEMPLATES */


public:
  iterator erase_after(iterator __pos) {
    return iterator((_Node*) this->_M_erase_after(__pos._M_node));
  }
  iterator erase_after(iterator __before_first, iterator __last) {
    return iterator((_Node*) this->_M_erase_after(__before_first._M_node, 
                                                  __last._M_node));
  } 

  iterator erase(iterator __pos) {
    return (_Node*) this->_M_erase_after(__slist_previous(&this->_M_head, 
                                                          __pos._M_node));
  }
  iterator erase(iterator __first, iterator __last) {
    return (_Node*) this->_M_erase_after(
      __slist_previous(&this->_M_head, __first._M_node), __last._M_node);
  }

  void resize(size_type new_size, const _Tp& __x);
  void resize(size_type new_size) { resize(new_size, _Tp()); }
  void clear() { this->_M_erase_after(&this->_M_head, 0); }

public:
  // Moves the range [__before_first + 1, __before_last + 1) to *this,
  //  inserting it immediately after __pos.  This is constant time.
  void splice_after(iterator __pos, 
                    iterator __before_first, iterator __before_last)
  {
    if (__before_first != __before_last) 
      __slist_splice_after(__pos._M_node, __before_first._M_node, 
                           __before_last._M_node);
  }

  // Moves the element that follows __prev to *this, inserting it immediately
  //  after __pos.  This is constant time.
  void splice_after(iterator __pos, iterator __prev)
  {
    __slist_splice_after(__pos._M_node,
                         __prev._M_node, __prev._M_node->_M_next);
  }


  // Removes all of the elements from the list __x to *this, inserting
  // them immediately after __pos.  __x must not be *this.  Complexity:
  // linear in __x.size().
  void splice_after(iterator __pos, slist& __x)
  {
    __slist_splice_after(__pos._M_node, &__x._M_head);
  }

  // Linear in distance(begin(), __pos), and linear in __x.size().
  void splice(iterator __pos, slist& __x) {
    if (__x._M_head._M_next)
      __slist_splice_after(__slist_previous(&this->_M_head, __pos._M_node),
                           &__x._M_head, __slist_previous(&__x._M_head, 0));
  }

  // Linear in distance(begin(), __pos), and in distance(__x.begin(), __i).
  void splice(iterator __pos, slist& __x, iterator __i) {
    __slist_splice_after(__slist_previous(&this->_M_head, __pos._M_node),
                         __slist_previous(&__x._M_head, __i._M_node),
                         __i._M_node);
  }

  // Linear in distance(begin(), __pos), in distance(__x.begin(), __first),
  // and in distance(__first, __last).
  void splice(iterator __pos, slist& __x, iterator __first, iterator __last)
  {
    if (__first != __last)
      __slist_splice_after(__slist_previous(&this->_M_head, __pos._M_node),
                           __slist_previous(&__x._M_head, __first._M_node),
                           __slist_previous(__first._M_node, __last._M_node));
  }

public:
  void reverse() { 
    if (this->_M_head._M_next)
      this->_M_head._M_next = __slist_reverse(this->_M_head._M_next);
  }

  void remove(const _Tp& __val); 
  void unique(); 
  void merge(slist& __x);
  void sort();     

#ifdef __STL_MEMBER_TEMPLATES
  template <class _Predicate> 
  void remove_if(_Predicate __pred);

  template <class _BinaryPredicate> 
  void unique(_BinaryPredicate __pred); 

  template <class _StrictWeakOrdering> 
  void merge(slist&, _StrictWeakOrdering);

  template <class _StrictWeakOrdering> 
  void sort(_StrictWeakOrdering __comp); 
#endif /* __STL_MEMBER_TEMPLATES */
};

template <class _Tp, class _Alloc>
slist<_Tp,_Alloc>& slist<_Tp,_Alloc>::operator=(const slist<_Tp,_Alloc>& __x)
{
  if (&__x != this) {
    _Node_base* __p1 = &this->_M_head;
    _Node* __n1 = (_Node*) this->_M_head._M_next;
    const _Node* __n2 = (const _Node*) __x._M_head._M_next;
    while (__n1 && __n2) {
      __n1->_M_data = __n2->_M_data;
      __p1 = __n1;
      __n1 = (_Node*) __n1->_M_next;
      __n2 = (const _Node*) __n2->_M_next;
    }
    if (__n2 == 0)
      this->_M_erase_after(__p1, 0);
    else
      _M_insert_after_range(__p1, const_iterator((_Node*)__n2), 
                                  const_iterator(0));
  }
  return *this;
}

template <class _Tp, class _Alloc>
void slist<_Tp, _Alloc>::_M_fill_assign(size_type __n, const _Tp& __val) {
  _Node_base* __prev = &this->_M_head;
  _Node* __node = (_Node*) this->_M_head._M_next;
  for ( ; __node != 0 && __n > 0 ; --__n) {
    __node->_M_data = __val;
    __prev = __node;
    __node = (_Node*) __node->_M_next;
  }
  if (__n > 0)
    _M_insert_after_fill(__prev, __n, __val);
  else
    this->_M_erase_after(__prev, 0);
}

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> template <class _InputIter>
void
slist<_Tp, _Alloc>::_M_assign_dispatch(_InputIter __first, _InputIter __last,
                                       __false_type)
{
  _Node_base* __prev = &this->_M_head;
  _Node* __node = (_Node*) this->_M_head._M_next;
  while (__node != 0 && __first != __last) {
    __node->_M_data = *__first;
    __prev = __node;
    __node = (_Node*) __node->_M_next;
    ++__first;
  }
  if (__first != __last)
    _M_insert_after_range(__prev, __first, __last);
  else
    this->_M_erase_after(__prev, 0);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
inline bool 
operator==(const slist<_Tp,_Alloc>& _SL1, const slist<_Tp,_Alloc>& _SL2)
{
  typedef typename slist<_Tp,_Alloc>::const_iterator const_iterator;
  const_iterator __end1 = _SL1.end();
  const_iterator __end2 = _SL2.end();

  const_iterator __i1 = _SL1.begin();
  const_iterator __i2 = _SL2.begin();
  while (__i1 != __end1 && __i2 != __end2 && *__i1 == *__i2) {
    ++__i1;
    ++__i2;
  }
  return __i1 == __end1 && __i2 == __end2;
}


template <class _Tp, class _Alloc>
inline bool
operator<(const slist<_Tp,_Alloc>& _SL1, const slist<_Tp,_Alloc>& _SL2)
{
  return lexicographical_compare(_SL1.begin(), _SL1.end(), 
                                 _SL2.begin(), _SL2.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Alloc>
inline bool 
operator!=(const slist<_Tp,_Alloc>& _SL1, const slist<_Tp,_Alloc>& _SL2) {
  return !(_SL1 == _SL2);
}

template <class _Tp, class _Alloc>
inline bool 
operator>(const slist<_Tp,_Alloc>& _SL1, const slist<_Tp,_Alloc>& _SL2) {
  return _SL2 < _SL1;
}

template <class _Tp, class _Alloc>
inline bool 
operator<=(const slist<_Tp,_Alloc>& _SL1, const slist<_Tp,_Alloc>& _SL2) {
  return !(_SL2 < _SL1);
}

template <class _Tp, class _Alloc>
inline bool 
operator>=(const slist<_Tp,_Alloc>& _SL1, const slist<_Tp,_Alloc>& _SL2) {
  return !(_SL1 < _SL2);
}

template <class _Tp, class _Alloc>
inline void swap(slist<_Tp,_Alloc>& __x, slist<_Tp,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


template <class _Tp, class _Alloc>
void slist<_Tp,_Alloc>::resize(size_type __len, const _Tp& __x)
{
  _Node_base* __cur = &this->_M_head;
  while (__cur->_M_next != 0 && __len > 0) {
    --__len;
    __cur = __cur->_M_next;
  }
  if (__cur->_M_next) 
    this->_M_erase_after(__cur, 0);
  else
    _M_insert_after_fill(__cur, __len, __x);
}

template <class _Tp, class _Alloc>
void slist<_Tp,_Alloc>::remove(const _Tp& __val)
{
  _Node_base* __cur = &this->_M_head;
  while (__cur && __cur->_M_next) {
    if (((_Node*) __cur->_M_next)->_M_data == __val)
      this->_M_erase_after(__cur);
    else
      __cur = __cur->_M_next;
  }
}

template <class _Tp, class _Alloc> 
void slist<_Tp,_Alloc>::unique()
{
  _Node_base* __cur = this->_M_head._M_next;
  if (__cur) {
    while (__cur->_M_next) {
      if (((_Node*)__cur)->_M_data == 
          ((_Node*)(__cur->_M_next))->_M_data)
        this->_M_erase_after(__cur);
      else
        __cur = __cur->_M_next;
    }
  }
}

template <class _Tp, class _Alloc>
void slist<_Tp,_Alloc>::merge(slist<_Tp,_Alloc>& __x)
{
  _Node_base* __n1 = &this->_M_head;
  while (__n1->_M_next && __x._M_head._M_next) {
    if (((_Node*) __x._M_head._M_next)->_M_data < 
        ((_Node*)       __n1->_M_next)->_M_data) 
      __slist_splice_after(__n1, &__x._M_head, __x._M_head._M_next);
    __n1 = __n1->_M_next;
  }
  if (__x._M_head._M_next) {
    __n1->_M_next = __x._M_head._M_next;
    __x._M_head._M_next = 0;
  }
}

template <class _Tp, class _Alloc>
void slist<_Tp,_Alloc>::sort()
{
  if (this->_M_head._M_next && this->_M_head._M_next->_M_next) {
    slist __carry;
    slist __counter[64];
    int __fill = 0;
    while (!empty()) {
      __slist_splice_after(&__carry._M_head,
                           &this->_M_head, this->_M_head._M_next);
      int __i = 0;
      while (__i < __fill && !__counter[__i].empty()) {
        __counter[__i].merge(__carry);
        __carry.swap(__counter[__i]);
        ++__i;
      }
      __carry.swap(__counter[__i]);
      if (__i == __fill)
        ++__fill;
    }

    for (int __i = 1; __i < __fill; ++__i)
      __counter[__i].merge(__counter[__i-1]);
    this->swap(__counter[__fill-1]);
  }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> 
template <class _Predicate>
void slist<_Tp,_Alloc>::remove_if(_Predicate __pred)
{
  _Node_base* __cur = &this->_M_head;
  while (__cur->_M_next) {
    if (__pred(((_Node*) __cur->_M_next)->_M_data))
      this->_M_erase_after(__cur);
    else
      __cur = __cur->_M_next;
  }
}

template <class _Tp, class _Alloc> template <class _BinaryPredicate> 
void slist<_Tp,_Alloc>::unique(_BinaryPredicate __pred)
{
  _Node* __cur = (_Node*) this->_M_head._M_next;
  if (__cur) {
    while (__cur->_M_next) {
      if (__pred(((_Node*)__cur)->_M_data, 
                 ((_Node*)(__cur->_M_next))->_M_data))
        this->_M_erase_after(__cur);
      else
        __cur = (_Node*) __cur->_M_next;
    }
  }
}

template <class _Tp, class _Alloc> template <class _StrictWeakOrdering>
void slist<_Tp,_Alloc>::merge(slist<_Tp,_Alloc>& __x,
                              _StrictWeakOrdering __comp)
{
  _Node_base* __n1 = &this->_M_head;
  while (__n1->_M_next && __x._M_head._M_next) {
    if (__comp(((_Node*) __x._M_head._M_next)->_M_data,
               ((_Node*)       __n1->_M_next)->_M_data))
      __slist_splice_after(__n1, &__x._M_head, __x._M_head._M_next);
    __n1 = __n1->_M_next;
  }
  if (__x._M_head._M_next) {
    __n1->_M_next = __x._M_head._M_next;
    __x._M_head._M_next = 0;
  }
}

template <class _Tp, class _Alloc> template <class _StrictWeakOrdering> 
void slist<_Tp,_Alloc>::sort(_StrictWeakOrdering __comp)
{
  if (this->_M_head._M_next && this->_M_head._M_next->_M_next) {
    slist __carry;
    slist __counter[64];
    int __fill = 0;
    while (!empty()) {
      __slist_splice_after(&__carry._M_head,
                           &this->_M_head, this->_M_head._M_next);
      int __i = 0;
      while (__i < __fill && !__counter[__i].empty()) {
        __counter[__i].merge(__carry, __comp);
        __carry.swap(__counter[__i]);
        ++__i;
      }
      __carry.swap(__counter[__i]);
      if (__i == __fill)
        ++__fill;
    }

    for (int __i = 1; __i < __fill; ++__i)
      __counter[__i].merge(__counter[__i-1], __comp);
    this->swap(__counter[__fill-1]);
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

// Specialization of insert_iterator so that insertions will be constant
// time rather than linear time.

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class _Tp, class _Alloc>
class insert_iterator<slist<_Tp, _Alloc> > {
protected:
  typedef slist<_Tp, _Alloc> _Container;
  _Container* container;
  typename _Container::iterator iter;
public:
  typedef _Container          container_type;
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;

  insert_iterator(_Container& __x, typename _Container::iterator __i) 
    : container(&__x) {
    if (__i == __x.begin())
      iter = __x.before_begin();
    else
      iter = __x.previous(__i);
  }

  insert_iterator<_Container>&
  operator=(const typename _Container::value_type& __value) { 
    iter = container->insert_after(iter, __value);
    return *this;
  }
  insert_iterator<_Container>& operator*() { return *this; }
  insert_iterator<_Container>& operator++() { return *this; }
  insert_iterator<_Container>& operator++(int) { return *this; }
};

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE 

#endif /* __SGI_STL_INTERNAL_SLIST_H */

// Local Variables:
// mode:C++
// End:

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_SET_H
#define __SGI_STL_INTERNAL_SET_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

// Forward declarations of operators < and ==, needed for friend declaration.

template <class _Key, class _Compare __STL_DEPENDENT_DEFAULT_TMPL(less<_Key>),
          class _Alloc = __STL_DEFAULT_ALLOCATOR(_Key) >
class set;

template <class _Key, class _Compare, class _Alloc>
inline bool operator==(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y);

template <class _Key, class _Compare, class _Alloc>
inline bool operator<(const set<_Key,_Compare,_Alloc>& __x, 
                      const set<_Key,_Compare,_Alloc>& __y);


// Set默认递增排序
template <class _Key, class _Compare, class _Alloc>
class set {
  // requirements:

  __STL_CLASS_REQUIRES(_Key, _Assignable);
  __STL_CLASS_BINARY_FUNCTION_CHECK(_Compare, bool, _Key, _Key);

public:
  // typedefs:

  typedef _Key     key_type;
  typedef _Key     value_type;
  // key_compare与value_compare使用同一个函数
  typedef _Compare key_compare;
  typedef _Compare value_compare;
private:
  typedef _Rb_tree<key_type, value_type, 
                  _Identity<value_type>, key_compare, _Alloc> _Rep_type;
  // 使用红黑树表现set
  _Rep_type _M_t;  // red-black tree representing set
public:
  typedef typename _Rep_type::const_pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::const_reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  // iterator定义为const，不允许写入操作
  typedef typename _Rep_type::const_iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  // allocation/deallocation
  // set使用红黑树的insert_unique，不允许重复键值
  set() : _M_t(_Compare(), allocator_type()) {}
  explicit set(const _Compare& __comp,
               const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}

#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  set(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_unique(__first, __last); }

  template <class _InputIterator>
  set(_InputIterator __first, _InputIterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }
#else
  set(const value_type* __first, const value_type* __last) 
    : _M_t(_Compare(), allocator_type()) 
    { _M_t.insert_unique(__first, __last); }

  set(const value_type* __first, 
      const value_type* __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }

  set(const_iterator __first, const_iterator __last)
    : _M_t(_Compare(), allocator_type()) 
    { _M_t.insert_unique(__first, __last); }

  set(const_iterator __first, const_iterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }
#endif /* __STL_MEMBER_TEMPLATES */

  set(const set<_Key,_Compare,_Alloc>& __x) : _M_t(__x._M_t) {}
  set<_Key,_Compare,_Alloc>& operator=(const set<_Key, _Compare, _Alloc>& __x)
  { 
    _M_t = __x._M_t; 
    return *this;
  }

  // accessors:
  // 调用红黑树实现
  key_compare key_comp() const { return _M_t.key_comp(); }
  // value_compare一样调用key_compare
  value_compare value_comp() const { return _M_t.key_comp(); }
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  iterator begin() const { return _M_t.begin(); }
  iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() const { return _M_t.rbegin(); } 
  reverse_iterator rend() const { return _M_t.rend(); }
  bool empty() const { return _M_t.empty(); }
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  void swap(set<_Key,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  pair<iterator,bool> insert(const value_type& __x) { 
    pair<typename _Rep_type::iterator, bool> __p = _M_t.insert_unique(__x); 
    return pair<iterator, bool>(__p.first, __p.second);
  }
  iterator insert(iterator __position, const value_type& __x) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    return _M_t.insert_unique((_Rep_iterator&)__position, __x);
  }
#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_unique(__first, __last);
  }
#else
  void insert(const_iterator __first, const_iterator __last) {
    _M_t.insert_unique(__first, __last);
  }
  void insert(const value_type* __first, const value_type* __last) {
    _M_t.insert_unique(__first, __last);
  }
#endif /* __STL_MEMBER_TEMPLATES */
  void erase(iterator __position) { 
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__position); 
  }
  size_type erase(const key_type& __x) { 
    return _M_t.erase(__x); 
  }
  void erase(iterator __first, iterator __last) { 
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__first, (_Rep_iterator&)__last); 
  }
  void clear() { _M_t.clear(); }

  // set operations:

  iterator find(const key_type& __x) const { return _M_t.find(__x); }
  size_type count(const key_type& __x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1;
  }
  iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x);
  }
  iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x); 
  }
  pair<iterator,iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }

#ifdef __STL_TEMPLATE_FRIENDS
  template <class _K1, class _C1, class _A1>
  friend bool operator== (const set<_K1,_C1,_A1>&, const set<_K1,_C1,_A1>&);
  template <class _K1, class _C1, class _A1>
  friend bool operator< (const set<_K1,_C1,_A1>&, const set<_K1,_C1,_A1>&);
#else /* __STL_TEMPLATE_FRIENDS */
  friend bool __STD_QUALIFIER
  // __STL_NULL_TMPL_ARGS定义为<>
  operator== __STL_NULL_TMPL_ARGS (const set&, const set&);
  friend bool __STD_QUALIFIER
  operator<  __STL_NULL_TMPL_ARGS (const set&, const set&);
#endif /* __STL_TEMPLATE_FRIENDS */
};

template <class _Key, class _Compare, class _Alloc>
inline bool operator==(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return __x._M_t == __y._M_t;
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator<(const set<_Key,_Compare,_Alloc>& __x, 
                      const set<_Key,_Compare,_Alloc>& __y) {
  return __x._M_t < __y._M_t;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Key, class _Compare, class _Alloc>
inline bool operator!=(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator>(const set<_Key,_Compare,_Alloc>& __x, 
                      const set<_Key,_Compare,_Alloc>& __y) {
  return __y < __x;
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator<=(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator>=(const set<_Key,_Compare,_Alloc>& __x, 
                       const set<_Key,_Compare,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Key, class _Compare, class _Alloc>
inline void swap(set<_Key,_Compare,_Alloc>& __x, 
                 set<_Key,_Compare,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_SET_H */

// Local Variables:
// mode:C++
// End:

// Copyright (c) 2013, Tim Kaler - MIT License

#ifndef REDUCER_VECTOR_H
#define REDUCER_VECTOR_H

#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <list>
#include <cilk/cilk.h>
#include <cilk/reducer.h>
#include <cilk/reducer_list.h>
#include <cilk/cilk_api.h>

template <typename T>
class Vector_reducer
{
private:
  cilk::reducer_list_append<std::vector<T>* > list_reducer;
public:
  Vector_reducer();
  void insert(T);
  std::vector<T> &get_reference();
  std::vector<T> &get_reference_serial();
  inline uint numElements() const;
  inline bool isEmpty() const;
  inline void clear();
  std::vector<T>** vectors;
};

template <typename T>
Vector_reducer<T>::Vector_reducer(){}

template <typename T>
void
Vector_reducer<T>::insert(T el)
{
  if (list_reducer.get_reference().size() == 0) {
    list_reducer.get_reference().push_back(new std::vector<T>());
  }
  list_reducer.get_reference().back()->push_back(el);
}

template <typename T>
std::vector<T>&
Vector_reducer<T>::get_reference()
{
  std::list<std::vector<T>* >& list = list_reducer.get_reference();
  if (list.size() == 1) {
    return *list.back();
  }
  int total_size = 0;
  int* starts = (int*) malloc(sizeof(int) * list.size());
  std::vector<T>** vector_list = (std::vector<T>**) malloc(sizeof(std::vector<T>*) * list.size());
  int index = 0;
  for (typename std::list<std::vector<T>* >::iterator it=list.begin(); it != list.end(); ++it) {
    starts[index] = total_size;
    total_size += (*it)->size();
    vector_list[index] = *it;
    index++;
  }
  std::vector<T>* combined = new std::vector<T>();
  combined->resize(total_size);

  cilk_for (int i = 0; i < list.size(); i++) {
    std::vector<T>* vector = vector_list[i];
    cilk_for (int j = starts[i]; j < vector->size() + starts[i]; j++) {
      (*combined)[j] = (*vector)[j - starts[i]];
    }
    delete vector;
  }

  list.clear();
  list.push_back(combined);

  return *combined;
}

// Unoptimized
template <typename T>
inline uint
Vector_reducer<T>::numElements() const
{
  return get_reference().size();
}

// Unoptimized
template <typename T>
inline bool
Vector_reducer<T>::isEmpty() const
{
  return get_reference().size() == 0;
}

// Unoptimized
template <typename T>
inline void
Vector_reducer<T>::clear()
{
  get_reference().clear();
}

#endif

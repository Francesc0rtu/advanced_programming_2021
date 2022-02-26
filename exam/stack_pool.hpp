#include<vector>
#include<iostream>
#include<utility>
#include<iterator>

 template <typename P, typename S, typename T>
 class _iterator{
   P* ptr_stack_pool;
   S stack;
  public:
   using value_type = T;
   using reference = value_type&;
   using pointer = value_type*;
   using difference_type = std::ptrdiff_t;
   using iterator_category = std::forward_iterator_tag;

  _iterator(P* ptr, S stack): ptr_stack_pool{ptr}, stack{stack} {}

  reference operator*() const {return ptr_stack_pool -> value(stack);}

   _iterator& operator++() {  // pre-increment
    stack = ptr_stack_pool -> next(stack);
    return *this;
  }
  _iterator operator++(int) {  // post-increment
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

   friend bool operator==(const _iterator& x, const _iterator& y) {
    return (x.ptr_stack_pool  == y.ptr_stack_pool && x.stack == y.stack);
  }

  friend bool operator!=(const _iterator& x, const _iterator& y) {
    return !(x == y);
  }
 };


template <typename T, typename N = std::size_t>
class stack_pool{
  struct node_t{
    T value;
    N next;
  };
  std::vector<node_t> pool;
  using stack_type = N;
  using value_type = T;
  using size_type = typename std::vector<node_t>::size_type;
  stack_type free_nodes; // at the beginning, it is empty

  node_t& node(stack_type x) noexcept { return pool[x-1]; }
  const node_t& node(stack_type x) const noexcept { return pool[x-1]; }

  template <typename S>
  stack_type _push(S&& val, stack_type head){
    if(empty(free_nodes)){
      pool.push_back({std::forward<S>(val),head});
      return pool.size(); 
    }else{
      stack_type new_head{free_nodes};
      free_nodes = next(free_nodes);
      next(new_head) = head;
      value(new_head) = std::forward<S>(val);
      return new_head;
    }

  }
 public:

  stack_pool() : free_nodes{end()} {};
  explicit stack_pool(size_type n): free_nodes{end()} { pool.reserve(n);  } // reserve n nodes in the pool
  ~stack_pool() = default;



  using iterator = _iterator< stack_pool, stack_type, value_type>;
  using const_iterator = _iterator< const stack_pool, stack_type, const value_type>;


  iterator begin(stack_type x)  { return iterator{this, x}; }
  iterator end(stack_type)  { return iterator{this,end()}; } 
  

  const_iterator begin(stack_type x) const { return const_iterator{this, x}; }
  const_iterator end(stack_type) const { return const_iterator{this,end()}; }
  

  const_iterator cbegin(stack_type x) const { return const_iterator{this, x}; }
  const_iterator cend(stack_type) const { return const_iterator{this,end()}; }


  stack_type new_stack() noexcept { return end(); } 

  void reserve(size_type n) { pool.reserve(n); }  // reserve n nodes in the pool
  size_type capacity() const noexcept { return pool.capacity(); } // the capacity of the pool

  bool empty(stack_type x) const noexcept { return x == end(); }

  stack_type end() const noexcept { return stack_type(0); } //0 is default to end


  T& value(stack_type x) noexcept { return node(x).value; }
  const T& value(stack_type x) const noexcept { return node(x).value;  }


  stack_type& next(stack_type x) noexcept { return node(x).next;  }
  const stack_type& next(stack_type x) const noexcept { return node(x).next;  }
  

  stack_type push(const T& val, stack_type head) { return _push(val,head);  }

  stack_type push(T&& val, stack_type head) { return _push(std::move(val),head); }


  stack_type pop(stack_type x) {    
    // remove first node of a stack 
    if( empty(x) ) { //if empty nothing to do
      return x;
    }
    stack_type tmp = x;
    x = next(x);      //new head
    next(tmp) = free_nodes;   // old head become new head of free_nodes
    free_nodes = std::move(tmp);
    return x;
  } // remove first node


  /*
    This function push the entire stack x to free_nodes, simply changed the next
    element of the last one element of x.
  */

  stack_type free_stack(stack_type x){
    if(empty(x) == true) {    // if y is empty nothing to do
      return x;
    }else{
      stack_type tmp = x;        
      for (auto ptr = cbegin(next(tmp)); ptr != cend(0); ++ptr){    // find last element of x
        tmp = next(tmp);            
      } 
      next(tmp) = free_nodes;           // last element of x point to old free_nodes
      free_nodes = std::move(x);         // free_nodes point to x
      return end();

    }
  }

  void print_stack(stack_type x) const {
    for (auto ptr = cbegin(x); ptr != cend(0); ++ptr){
      std::cout << *ptr  << " " ;
    } 
    std::cout << std::endl;
  }

};
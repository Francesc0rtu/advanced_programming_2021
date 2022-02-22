#include<vector>
#include<iostream>

 template <typename P, typename S, typename T>
 class _iterator{
   P *ptr_stack_pool;
   S stack;
  public:
   using value_type = T;
   using reference = value_type&;
   using pointer = value_type*;

  _iterator(P* ptr, S stack): ptr_stack_pool{ptr}, stack{stack} {
      // add some exception
   }
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
  stack_type _push(const S&& val, stack_type head){
    if(empty(free_nodes) == true){
      node_t _new = node_t();
      _new.value = std::move(val); //why not std::forwaRD()?
      _new.next = head;
      pool.push_back(std::move(_new));
      return pool.size(); 
    }else{
      stack_type new_head{free_nodes};
      free_nodes = next(free_nodes);
      next(new_head) = std::move(head);
      value(new_head) = std::move(val);
      return new_head;
    }

  }
 public:
  stack_pool(): free_nodes{end()} {};
  explicit stack_pool(size_type n): free_nodes{end()} {
    reserve(n); //funziona anche senza il this, perchè?
  } // reserve n nodes in the pool

  using iterator = _iterator< stack_pool, stack_type, value_type>;
  using const_iterator = _iterator< stack_pool, stack_type, const value_type>;

  iterator begin(stack_type x){
    return iterator{this, x};
  }
  iterator end_(){
    return iterator{this,end()};
  } // this is not a typo
  //
  const_iterator begin(stack_type x) const {
    return const_iterator{this, x};
  }
  const_iterator end_() const{
    return const_iterator{this,end()};
  }
  //
  // const_iterator cbegin(stack_type x) const;
  // const_iterator cend(stack_type ) const;

  ~stack_pool() {};

  stack_type new_stack() { 
    return end();} // return an empty stack---what value shuld return? 1-first free place?

  void reserve(size_type n){ //do not work
   pool.reserve(n);
  } // reserve n nodes in the pool

  size_type capacity() const {
    return pool.capacity();
  } // the capacity of the pool

  bool empty(stack_type x) const{
    return x == end();
  }

  stack_type end() const noexcept { 
    return stack_type(0); } //0 is default to end

  T& value(stack_type x) {
    // return reference to top value of stack x
    return node(x).value;
  }
  const T& value(stack_type x) const {
    // return reference to top value of stack x
    return node(x).value;
  }
  //
  stack_type& next(stack_type x){
    return node(x).next;
  }
  const stack_type& next(stack_type x) const{
    return node(x).next;
  }
  
  stack_type push(const T& val, stack_type head){
    return _push(std::move(val),head);
  }
  stack_type push(T&& val, stack_type head){
     return _push(std::move(val),head);
  }
  //
  stack_type pop(stack_type x){     // what if x is 0 ??
    stack_type tmp = x;
    x = next(x);
    next(tmp) = free_nodes;
    free_nodes = std::move(tmp);
    return x;
  } // delete first node
  //
  stack_type free_stack(stack_type x){
    for (auto ptr = begin(x); ptr != end_(); ++ptr){
      pop(ptr);
    } 
    return x;
  } // free entire stack

  void print_stack(stack_type x){
    for (auto ptr = begin(x); ptr != end_(); ++ptr){
      std::cout << *ptr  << "\n" ;
    } 
  }
};


int main(int argc, char const *argv[]) {
  stack_pool<int, std::size_t> pool{};
  
  auto l = pool.new_stack(), f = pool.new_stack();
  pool.reserve(10);

  for(int i=0; i<10; i++){
  l = pool.push(i,l);
  f = pool.push(10*i,f);
  }
  l = pool.push(10,l);
  pool.print_stack(l);
  pool.print_stack(f);
  // std::cout << pool.value(l) << "\n";
  // l = pool.pop(l);
  // // l = pool.pop(l);
  // std::cout << pool.value(l) << "\n";
  // // l == std::size_t(1)
  // pool.value(l) = 77;
  // return 0;
}

#ifndef SAE_GRAPG_STORAGE
#define SAE_GRAPG_STORAGE

#include <vector>

namespace saedb {

      template<typename VertexData, typename EdgeData>
      class graph_storage {
      public:
	    typedef saedb::lvid_type lvid_type;
	    typedef saedb::edge_id_type edge_id_type;

	    typedef EdgeData edge_data_type;

	    /** The type of the vertex data stored in the graph. */
	    typedef VertexData vertex_data_type;

	    
	    typedef std::pair<size_t, size_t>  edge_range_type;
	    
      public:
	    class edge_info {
	    public:
		  std::vector<EdgeData> data;
		  std::vector<lvid_type> source_arr;
		  std::vector<lvid_type> target_arr;
	    public:
		  edge_info () {}
		  void reserve_edge_space(size_t n) {
			data.reserve(n);
			source_arr.reserve(n);
			target_arr.reserve(n);
		  }
		  // \brief Add an edge to the temporary storage.
		  void add_edge(lvid_type source, lvid_type target, EdgeData _data) {
			data.push_back(_data);
			source_arr.push_back(source);
			target_arr.push_back(target);
		  }
		  // \brief Add edges in block to the temporary storage.
		  void add_block_edges(const std::vector<lvid_type>& src_arr, 
				       const std::vector<lvid_type>& dst_arr, 
				       const std::vector<EdgeData>& edata_arr) {
			data.insert(data.end(), edata_arr.begin(), edata_arr.end());
			source_arr.insert(source_arr.end(), src_arr.begin(), src_arr.end());
			target_arr.insert(target_arr.end(), dst_arr.begin(), dst_arr.end());
		  }
		  // \brief Remove all contents in the storage. 
		  void clear() {
			std::vector<EdgeData>().swap(data);
			std::vector<lvid_type>().swap(source_arr);
			std::vector<lvid_type>().swap(target_arr);
		  }
		  // \brief Return the size of the storage.
		  size_t size() const {
			return source_arr.size();
		  }
		  // \brief Return the estimated memory footprint used.
		  size_t estimate_sizeof() const {
			return data.capacity()*sizeof(EdgeData) + 
			      source_arr.capacity()*sizeof(lvid_type)*2 + 
			      sizeof(data) + sizeof(source_arr)*2 + sizeof(edge_info);
		  }
	    }; // end of class edge_info.

	    // A class of edge information. Used as value type of the edge_list.
	    class edge_type {
	    public:
		  /** \brief Creates an empty edge type. */
		  edge_type () : _source(-1), _target(-1), _edge_id(-1), 
				 _dir(NO_EDGES), _empty(true) { }
		  /** \brief Creates an edge of given source id, target id, edge
		   * id and direction enum.  \internal edge id is used to locate
		   * the edge data in the edge_data array.  edge_dir type is
		   * defined in graph_basic.hpp. **/
		  edge_type (const lvid_type _source, 
			     const lvid_type _target, 
			     const edge_id_type _eid, edge_dir_type _dir) :
			_source(_source), _target(_target), _edge_id(_eid), 
			_dir(_dir), _empty(false) {
			if (_dir != OUT_EDGES) std::swap(this->_source, this->_target);
		  }
	    public:
		  /** \brief Returns the source vertex id of the edge. */
		  inline lvid_type source() const {
			return _source;
		  }
		  /** \brief Returns the target vertex id of the edge. */
		  inline lvid_type target() const { 
			return _target;
		  }
		  /** \brief Returns the direction of the edge. */
		  inline edge_dir_type get_dir() const {
			return _dir;
		  }
		  /** \brief Returns whether this is an empty edge. */
		  inline bool empty() const { return _empty; }
		  // Data fields. 
	    private:
		  lvid_type _source;
		  lvid_type _target;
		  edge_id_type _edge_id;
		  edge_dir_type _dir;
		  bool _empty;

		  friend class graph_storage;
	    }; // end of class edge_type.

	    // Internal iterator on edge_types.
	    class edge_iterator  {
	    public:
		  typedef std::random_access_iterator_tag iterator_category;
		  typedef edge_type    value_type;
		  typedef ssize_t      difference_type;
		  typedef edge_type*   pointer;
		  typedef edge_type   reference;

	    public:
		  // Cosntructors
		  /** \brief Creates an empty iterator. */
		  edge_iterator () : offset(-1), empty(true) { }
		  /** \brief Creates an iterator at a specific edge.
		   * The edge location is defined by the follows: 
		   * A center vertex id,  an offset to the center, the direction and the
		   * pointer to the start of edge id array. */
		  edge_iterator (lvid_type _center, size_t _offset, 
				 edge_dir_type _itype, const lvid_type* _vid_arr) :
			center(_center), offset(_offset), itype(_itype), vid_arr(_vid_arr), 
			empty(false) { }
		  /** \brief Returns the value of the iterator. An empty iterator always returns empty edge type*/ 
		  inline edge_type operator*() const  {
			//  ASSERT_TRUE(!empty);
			return make_value();
		  }

		  /** \brief Returns if two iterators point to the same edge. */
		  inline bool operator==(const edge_iterator& it) const {
			return (empty && it.empty) || 
			      (empty == it.empty && itype == it.itype && center == it.center && 
			       offset == it.offset);
		  }

		  /** \brief Returns if two iterators don't point to the same edge. */
		  inline bool operator!=(const edge_iterator& it) const { 
			return !(*this == it);
		  }

		  /** \brief Increases the iterator. */
		  inline edge_iterator& operator++() {
			//ASSERT_TRUE(!empty);
			++offset;
			return *this;
		  }

		  /** \brief Increases the iterator. */
		  inline edge_iterator operator++(int) {
			//ASSERT_TRUE(!empty);
			const edge_iterator copy(*this);
			operator++();
			return copy;
		  }

		  /** \brief Computes the difference of two iterators. */
		  inline ssize_t operator-(const edge_iterator& it) const {
			return offset - it.offset;
		  }

		  /** \brief Returns a new iterator whose value is increased by i difference units. */
		  inline edge_iterator operator+(difference_type i) const {
			return edge_iterator(center, offset+i, itype, vid_arr);
		  }

		  /** \brief Increases the iterator by i difference units. */
		  inline edge_iterator& operator+=(difference_type i) {
			offset+=i;
			return *this;
		  }

		  /** \brief Generate the return value of the iterator. */
		  inline edge_type make_value() const {
			return empty ? edge_type() : edge_type(center, vid_arr[offset], offset, itype);
		  }

	    private:
		  lvid_type center;
		  size_t offset;
		  edge_dir_type itype;
		  const lvid_type* vid_arr;
		  bool empty;
	    }; // end of class edge_iterator.

	    /** Represents an iteratable list of edge_types. */
	    class edge_list {
	    public:
		  typedef edge_iterator iterator;
		  typedef edge_iterator const_iterator;
		  typedef edge_type value_type;
	    private:
		  edge_iterator begin_iter, end_iter;
	    public:
		  /** Cosntructs an edge_list with begin and end.  */
		  edge_list(const edge_iterator begin_iter = edge_iterator(), 
			    const edge_iterator end_iter = edge_iterator()) : 
			begin_iter(begin_iter), end_iter(end_iter) { }
		  inline size_t size() const { return end_iter - begin_iter;}            
		  inline edge_type operator[](size_t i) const {return *(begin_iter + i);}
		  iterator begin() const { return begin_iter; }
		  iterator end() const { return end_iter; }
		  bool empty() const { return size() == 0; }
	    }; // end of class edge_list.


      public:
	    // CONSTRUCTORS ============================================================>
	    graph_storage() : use_skip_list(false) {  }

	    // METHODS =================================================================>
   
	    /** \internal \brief  Set graph storage to use skip list.
	     * Skip list is used to jump between ...
	     * */ 
	    void set_use_skip_list (bool x) { use_skip_list = x;}

	    /** \brief Returns the number of edges in the graph. */
	    size_t edge_size() const { return num_edges; }

	    /** \brief Returns the number of vertices in the graph. */
	    size_t vertices_size() const { return num_vertices; }

	    /** \brief Returns the number of in edges of the vertex. */
	    size_t num_in_edges (const lvid_type v) const {

	    }

	    /** \brief Returns the number of out edges of the vertex. */
	    size_t num_out_edges (const lvid_type v) const {
	    }

	    /** \brief Returns the edge id of the edge. 
	     * Edges are assigned with consecutive int ids,
	     * ordered first by source and then by target.
	     * */
	    edge_id_type edge_id(const edge_type& edge) const {
	    }

	    /** \brief Returns the reference of edge data of an edge. */
	    edge_data_type& edge_data(lvid_type source, lvid_type target) {
	    }

	    /** \brief Returns the constant reference of edge data of an edge. */
	    const edge_data_type& edge_data(lvid_type source, 
					    lvid_type target) const {
	    }

	    /** \brief Returns the reference of edge data of an edge. */
	    edge_data_type& edge_data(edge_type edge) {
	    }

	    /** \brief Returns the constant reference of edge data of an edge. */
	    const edge_data_type& edge_data(edge_type edge) const {
	    }

	    /** \brief Returns a list of in edges of a vertex. */
	    edge_list in_edges(const lvid_type v) const {
	    }

	    /** \brief Returns a list of out edges of a vertex. */
	    edge_list out_edges(const lvid_type v) const {
	    }

	    void clear() {
	    }

	    void clear_reserve() {
	    }

      private:
	    /** Number of vertices in the storage (not counting singletons)*/
	    size_t num_vertices;
	    /** Number of edges in the storage. */
	    size_t num_edges;

	    /** Array of edge data sorted by source vid. */
	    std::vector<EdgeData> edge_data_list;

	    /** \internal 
	     * Row index of CSR, corresponding to the source vertices. */
	    std::vector<edge_id_type> CSR_src;

	    /** 
	     * \internal 
	     * Suppose CSR_src is: 1 x x 3 x x x x 5
	     * where x means no out edges.
	     *     CSR_src_skip =  0 2 2 0 4 0 0 4 0
	     * is used to jump to the prev/next valid vertex in CSR_src.  
	     * Optional.
	     */
	    std::vector<edge_id_type> CSR_src_skip;

	    /** \internal 
	     * Col index of CSR, corresponding to the target vertices. */
	    std::vector<lvid_type> CSR_dst;

	    /** \internal 
	     * Map the sort-by-col edge id to sort-by-row edge id */
	    std::vector<edge_id_type> c2r_map;

	    /** \internal
	     * Row index of CSC, corresponding to the target vertices. */
	    std::vector<edge_id_type> CSC_dst;

	    std::vector<edge_id_type> CSC_dst_skip;

	    std::vector<lvid_type> CSC_src;

	    /** Graph storage traits. */
	    bool use_skip_list;


	    /****************************************************************************
	     *                       Internal Functions                                 *
	     *                     ----------------------                               *
	     * These functions functions and types provide internal access to the       *
	     * underlying graph representation. They should not be used unless you      *
	     * *really* know what you are doing.                                        *
	     ****************************************************************************/
      private:
	    /** \internal
	     *  Returns the begin and end index of the in edge of vertex v. */
	    inline std::pair<bool, edge_range_type> inEdgeRange(lvid_type v) const {
	    }

	    /** \internal
	     *  Returns the begin and end index of the out edge of vertex v. */
	    inline std::pair<bool, edge_range_type> outEdgeRange(lvid_type v) const {
	    }


	    //-------------Private Helper functions------------
	    /** \internal
	     *  Compare functor of any type*/
	    template <typename anyvalue>
	    struct cmp_by_any_functor {
		  const std::vector<anyvalue>& vec;
		  cmp_by_any_functor(const std::vector<anyvalue>& _vec) : vec(_vec) { }
		  bool operator()(size_t me, size_t other) const {
			return (vec[me] < vec[other]);
		  }
	    };

	    /** \internal
	     *  Binary search vfind in a vector of lvid_type 
	     *  within range [start, end]. Returns (size_t)(-1) if not found. */
	    size_t binary_search(const std::vector<lvid_type>& vec, 
				 size_t start, size_t end, 
				 lvid_type vfind) const {
	    }// End of binary_search

      public:

	    /** \internal
	     * Returns a reference of CSR_src.*/
	    const std::vector<lvid_type>& get_csr_src() const {
		  return CSR_src;
	    }
	    /** \internal
	     * Returns a reference of CSR_dst.*/
	    const std::vector<edge_id_type>& get_csr_dst() const {
		  return CSR_dst;
	    }
	    /** \internal
	     * Returns a reference of CSC_src.*/
	    const std::vector<edge_id_type>& get_csc_src() const {
		  return CSC_src;
	    }
	    /** \internal
	     * Returns a reference of CSC_dst.*/
	    const std::vector<lvid_type>& get_csc_dst() const {
		  return CSC_dst;
	    }
	    /** \internal
	     * Returns a reference of edge_data_list.*/
	    const std::vector<EdgeData>& get_edge_data() const {
		  return edge_data_list;
	    }

	    /** \brief Load the graph from an archive */
	    void load() {
	    }

	    /** \brief Save the graph to an archive */
	    void save() const {
	    }

	    /** swap two graph storage*/
	    void swap(graph_storage& other) {
	    }

      };
}

#endif

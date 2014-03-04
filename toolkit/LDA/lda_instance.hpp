#include "malloc.hpp"
#include <iostream>
using namespace std;
#define NULL_TOPIC -1


class vertex_data
{
public:
	int id;//save key of SAE graph
	int topic_num;
	int* n;//topic distribution for document or word
	vertex_data()
	{
	}
	vertex_data(int idd) : id(idd){
	}
	void AllocHeap()
	{
		n=NewVector<int>(topic_num);
	}
	friend std::ostream & operator<< (std::ostream & output, const vertex_data &v) 
	{
		output<<"id: "<<v.id<<std::endl;
		output<<"topic num: "<<v.topic_num<<std::endl;
		output<<"n: "<<std::endl;
		OutputVector<int>(v.n,v.topic_num);
		return output;  
	}
	void ReleaseHeap()
	{
		DeleteVector<int>(n);
	}

}; //end of vertex_data

class edge_data
{
public:
	int id;
	int assignment; // this edge assign to which topic
	edge_data(){assignment=NULL_TOPIC;}
	edge_data(int idd) :id(idd)
	{
			assignment=NULL_TOPIC;
	};
}; // end of edge_data;

namespace sae{
namespace serialization{

namespace custom_serialization_impl {

     /*
     * VertexInstance
     */
    template <>
    struct deserialize_impl<ISerializeStream, vertex_data> {
        static void run(ISerializeStream& istr, vertex_data& vdata) {
	 
            istr>>vdata.id>>vdata.topic_num>>vdata.n;
            
        }
    };

    template <>
    struct serialize_impl<OSerializeStream, vertex_data> {
        static void run(OSerializeStream& ostr, const vertex_data& vdata) {
	 
	   ostr<<vdata.id<<vdata.topic_num<<vdata.n;
        }
    };
     /*
     * EdgeInstance
     */
    template <>
    struct deserialize_impl<ISerializeStream, edge_data> {
        static void run(ISerializeStream& istr, edge_data& edata) {
	    
            istr>>edata.id>>edata.assignment;
        }
    };

    template <>
    struct serialize_impl<OSerializeStream, edge_data> {
        static void run(OSerializeStream& ostr, const edge_data& edata) {
	    
            ostr<<edata.id<<edata.assignment;
        }
    };




}
}
}



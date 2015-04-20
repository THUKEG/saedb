#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <functional>
#include <cassert>

#include <graphlab.hpp>

// defination of HIS
size_t top_k = 0;
size_t c_num = 0;
std::vector<int> target_communities;
const double SH_TOLERANCE = 1.0E-5;
double* alpha;
double* beta;

// defination of PageRank
const double PR_RESET_PROB = 0.15;
const double PR_TOLERANCE = 1.0E-5;
int cnt;
double max_pr = 0.0;

// defination of vertex_data, which is a part of vertex_type
struct vertex_data{
    std::vector<double> inf_value;
    std::vector<double> sh_value;

    int area;
    double pr_value;

    vertex_data() : 
        inf_value(), sh_value()
        {   area = 0;   
            pr_value = 0.0; }

    vertex_data(int a) :
        inf_value(), sh_value()
        {   area = a;   
            pr_value = 0.0; }

    void save(graphlab::oarchive& oarc) const{
        oarc << area << pr_value
                << inf_value << sh_value;
    }

    void load(graphlab::iarchive& iarc) {
        iarc >> area >> pr_value
                >> inf_value >> sh_value;
    }
};

struct neighbor_info{
    std::vector<double> neigh_max;

    neighbor_info() : neigh_max()  {}

    neighbor_info(double n) : neigh_max() {
        neigh_max.push_back(n);
    }

    neighbor_info(const std::vector<double>& other) : neigh_max() {
        std::copy(other.begin(), other.end(), std::back_inserter(neigh_max));
    }

    neighbor_info& operator+=(const neighbor_info& other){
        for (std::vector<double>::const_iterator iter = other.neigh_max.begin(); 
            iter != other.neigh_max.end(); iter++){
            neigh_max.push_back(*iter);
        }
        return *this;
    }

    void push(double n){
        neigh_max.push_back(n);
    }

    void save(graphlab::oarchive& oarc) const {
        oarc << neigh_max;
    }

    void load(graphlab::iarchive& iarc) {
        iarc >> neigh_max;
    }
};


// defination of graphlab
size_t NUM_CORE = 0;
typedef graphlab::empty edge_data_type;
typedef graphlab::distributed_graph<vertex_data, edge_data_type> graph_type;

// load data from file
bool vertex_loader(graph_type& graph, const std::string fname, 
                    const std::string& line) {
    if(line.empty()) return true;
    std::stringstream strm(line);
    graphlab::vertex_id_type vid;
    int a;
    strm >> vid;
    if(strm.fail()) return false;
    strm >> a;
    if(strm.fail()) return false;

    graph.add_vertex(vid, vertex_data(a));
    return true;
}

bool edge_loader(graph_type& graph, const std::string& filename,
                  const std::string& textline) {
    if(textline.empty()) return true;
    std::stringstream strm(textline);
    size_t source_vid = 0;
    size_t target_vid = 0;
    strm >> source_vid;
    if(strm.fail()) return false;
    strm >> target_vid;
    if(strm.fail()) return false;
	
	// load both direction of the edge
    graph.add_edge(source_vid, target_vid);
    graph.add_edge(target_vid, source_vid);
    return true;
}

// pagerank
void init_pagerank(graph_type::vertex_type& vertex){
    vertex.data().pr_value = 1.0;
}

struct pagerank_writer {
    std::string save_vertex(graph_type::vertex_type v) {
        std::stringstream strm;
        strm << v.id() << "\t" << v.data().pr_value << std::endl;
        strm.flush();
        return strm.str();
    }

    std::string save_edge(graph_type::edge_type e) {
        return "";
    }
};

void getmax_pr(graph_type::vertex_type& vertex) {
    if(vertex.data().pr_value > max_pr)
        max_pr = vertex.data().pr_value;
}

void scale(graph_type::vertex_type& vertex) {
    vertex.data().pr_value /= max_pr;
}

class pagerank : 
    public graphlab::ivertex_program<graph_type, double>,
    public graphlab::IS_POD_TYPE {
    double last_change;
public:

    /*  gather information from what kind of vertex
        just the in edges or all the edges?
    edge_dir_type gather_edges(icontext_type& context,
        const vertex_type& vertex) const {
            // return graphlag::IN_EDGES as a default value
            // change if you want to return graphlab::ALL_EDGES
        }
    */

    // gather information, 
    // return what should be gathered of this single vertex
    double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
        return ((1.0 - PR_RESET_PROB) / edge.source().num_out_edges()) * 
            edge.source().data().pr_value;
    }

    // analyse the data collected from adjucent vertices
    // gather_type is the second template parameter of ivertex_program
    void apply(icontext_type& context, vertex_type& vertex,
                const gather_type& total) {
        const double newval = total + PR_RESET_PROB;
        last_change = std::fabs(newval - vertex.data().pr_value);
        vertex.data().pr_value = newval;
    }

    // scatter information to which kind of edges?
    // return graphlab::OUT_EDGES as default
    edge_dir_type scatter_edges(icontext_type& context, 
                                const vertex_type& vertex) const {
        if(last_change > PR_TOLERANCE) return graphlab::OUT_EDGES;
        else return graphlab::NO_EDGES;
    }

    // what should do during the scatter phase?
    // signal means to activate adjucent vertex
    void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
        context.signal(edge.target());
    }
};

// his
int countbit(int n) { return (n==0)?0:(1+countbit(n&(n-1)));}
bool inline contain(int s, int c){
    int power = 1 << c;
    return ((s & power) != 0);
}

void init_alpha_beta(){
    alpha = new double[c_num];
    int beta_num = 1 << c_num;
    beta = new double[beta_num];
	// How to calculate these parameters?
    for(int i=0; i<c_num; i++)
        alpha[i] = 0.30;
    for(int set=0; set<beta_num; set++){
        if (countbit(set)==0) beta[set]=0;
        else if (countbit(set)==1) beta[set]=0;
        else if (countbit(set)==2) beta[set]=0.17;
        else if (countbit(set)==3) beta[set]=0.25;
        else if (countbit(set)==4) beta[set]=0.29;
        else if (countbit(set)==5) beta[set]=0.30;
        else if (countbit(set)>=6) beta[set]=0.35;
    }
}

void init_his(graph_type::vertex_type& vertex){
    int area = vertex.data().area;
    double pr = vertex.data().pr_value;
    for(int k=0; k<c_num; k++){
		// TODO: why this initialization doesn't use contain(i,j)?
        if((area & target_communities[k]) == target_communities[k])
            vertex.data().inf_value.push_back(pr);
        else
            vertex.data().inf_value.push_back(0.0);
    }
    int beta_num = 1 << c_num;
    for(int i=0; i<beta_num; i++)
        vertex.data().sh_value.push_back(0.0);
	for(int i=1; i<beta_num; i++){
		double min = std::numeric_limits<double>::infinity();
		for(int j=0; j<c_num; j++)
			if(vertex.data().inf_value[j] < min && contain(i,j))
				min = vertex.data().inf_value[j];

		assert(min != std::numeric_limits<double>::infinity());
		vertex.data().sh_value[i] = min;
	}
}

struct his_writer{
    std::string save_vertex(graph_type::vertex_type v) {
        double max = 0;
        std::stringstream strm;
        for(int i=1; i<(1<<c_num); i++)
            if(v.data().sh_value[i] > max)
                max = v.data().sh_value[i];
        strm << v.id() << "\t" << max << std::endl;
        strm.flush();
        return strm.str();
    }

    std::string save_edge(graph_type::edge_type e) { return ""; }
};

class his : public graphlab::ivertex_program<graph_type,
    neighbor_info>, public graphlab::IS_POD_TYPE {
    std::vector<double> last_change;
public:
    // use the default gather_edges
	// return IN_EDGES
	
    // scatter to all the edges
    edge_dir_type scatter_edges(icontext_type& context,
        const vertex_type& vertex) const {
        double max_last_change = 0.0;
        for(int i=0; i<c_num; i++)
            if(last_change[i] > max_last_change)
                max_last_change = last_change[i];

        if(max_last_change > SH_TOLERANCE) return graphlab::OUT_EDGES;
        else return graphlab::NO_EDGES;
    }

    neighbor_info gather(icontext_type& context, const vertex_type& vertex, 
        edge_type& edge) const{
        int beta_num = 1 << c_num;
        std::vector<double> temp;
		const vertex_type& v = edge.source();
        for(int i=0; i<c_num; i++){
            double max = 0;
            for(int j=1; j<beta_num; j++){
                if(contain(j,i)){
                    double t = alpha[i] * v.data().inf_value[i]
                                + beta[j] * v.data().sh_value[j];
					max = t > max ? t : max;
                }
            }
            temp.push_back(max);
        }
        return neighbor_info(temp);
    }

    // gather_type is the neighbor_info
    // you must rewrite the operator += in order to gather all the information of the surrounding vertices
    void apply(icontext_type& context, vertex_type& vertex, 
        const gather_type& total) {
        if(last_change.size() == 0)
            for(int i=0; i<c_num; i++)
                last_change.push_back(0.0);
            
        for(int i=0; i<c_num; i++){
            double max = vertex.data().inf_value[i];
            for(int iter = i; iter < total.neigh_max.size(); iter = iter + c_num){
                if(total.neigh_max[iter] > max)
                    max = total.neigh_max[iter];
            }
			
            last_change[i] = std::fabs(max - vertex.data().inf_value[i]);
            vertex.data().inf_value[i] = max;
        }
        int beta_num = 1 << c_num;
        for(int i=1; i<beta_num; i++){
            double min = std::numeric_limits<double>::infinity();
            for(int j=0; j<c_num; j++){
                if(contain(i,j))
                    if(vertex.data().inf_value[j] < min)
                        min = vertex.data().inf_value[j];
            }
            vertex.data().sh_value[i] = min;
        }
    }

    void scatter(icontext_type& context, const vertex_type& vertex,
        edge_type& edge) const {
        context.signal(edge.target());
    }
};

void get_partial_answer(std::vector<std::pair<double, int> >& sum, int part){
	const std::string begin = "sh_value.out_";
	const std::string end = "_of_";
	std::stringstream strm;
	strm << begin << part << end << NUM_CORE;
	strm.flush();
	std::string filename = strm.str();

	std::string content;
	double sh;
	int which;
	std::ifstream fin(filename.data());
	while(getline(fin, content)){
		std::stringstream strm;
		strm << content;
		strm >> which;
		strm >> sh;
		sum.push_back(std::make_pair(sh, which));
	}
	fin.close();
}

void get_all_answer(){
	std::vector<std::pair<double, int> > sum;
	for(int i=1; i<=NUM_CORE; i++)
		get_partial_answer(sum, i);
	std::sort(sum.begin(), sum.end(), std::greater<std::pair<double, int> >());
	for(int i=0; i<top_k; i++)
		std::cout<<sum[i].second << "\t" << sum[i].first << std::endl;
}

// main program
int main(int argc, char **argv)
{
    std::cout<<"Computes top k structure hole of input graph.\n";

    // deal with args
    graphlab::command_line_options clopts
        ("This algorithm using HIS algorithm,"
         "Paralleled by graphlab");

    // default parameters
    std::string graphfile = "dblp_graph.txt";
    std::string communityfile = "community.input";
    std::string pr_sv_prefix = "pr_value.out";
    std::string sh_sv_prefix = "sh_value.out";
	NUM_CORE = 4;
    top_k = 100;
    std::string exec_type = "synchronous";

    // parse arguments and handle exceptions
    clopts.attach_option("graph", graphfile,
                         "Input file, a graph using adjacent representation");
    clopts.attach_option("community", communityfile,
                         "Input file, the communities of a certain vertex");
    clopts.attach_option("clusters", NUM_CORE,
                         "The number of clusters to create.");
    clopts.attach_option("k", top_k,
                         "Calculate the top k structure hole");
    clopts.attach_option("engine", exec_type,
                         "The engine type is synchronous.");
    clopts.attach_option("prvalue", pr_sv_prefix,
                         "saving prefix of pagerank value.");
    clopts.attach_option("shvalue", sh_sv_prefix,
                         "saving prefix of structure hole value.");
    if(!clopts.parse(argc, argv)) return EXIT_FAILURE;
    if(graphfile == "") {
        std::cout<<"--graph is not optional\n";
        return EXIT_FAILURE;
    }
    if(communityfile == "") {
        std::cout<<"--community is not optional\n";
        return EXIT_FAILURE;
	}
    if(NUM_CORE == 0) {
        std::cout<<"--clusters is not optional\n";
        return EXIT_FAILURE;
    }
    if(top_k == 0) {
        std::cout<<"--k is not optional\n";
        return EXIT_FAILURE;
    }

    std::cout<<"Parse arguments success"<<std::endl;
    std::cout<<"Please input the number of communiites"<<std::endl;
    std::cin>>c_num;
    if(c_num < 2){
        std::cout<<"The number of communities must be larger than 2"<<std::endl;
        return EXIT_FAILURE;
    }
    std::cout<<"Please input "<<c_num<<" community id"<<std::endl;
    for(size_t i=0; i<c_num; i++){
        int temp;
        std::cin>>temp;
		// TODO: push_back(temp) or push_back(1 << temp)?
		// what is the format of input?
        target_communities.push_back(temp);
    }
    init_alpha_beta();

    // init graph and distributed tools
    graphlab::mpi_tools::init(argc, argv);
    graphlab::distributed_control dc;

    //load graph
    graph_type graph(dc, clopts);
    graph.load(communityfile, vertex_loader);
    graph.load(graphfile, edge_loader);
    graph.finalize();
    dc.cout()<<"#vertices: "<<graph.num_vertices()<<" "
             <<"#edges: "<<graph.num_edges() << std::endl;
    cnt = graph.num_vertices();

    // page rank
    dc.cout()<<"calculating pagerank value of vertices"<<std::endl;
    graph.transform_vertices(init_pagerank);

    graphlab::omni_engine<pagerank> engine(dc, graph, exec_type, clopts);
    engine.signal_all();
    engine.start();
    
    graph.transform_vertices(getmax_pr);
    graph.transform_vertices(scale);

    const float runtime = engine.elapsed_seconds();
    dc.cout() <<"finished pagerank in "<<runtime
                <<" seconds."<<std::endl;
    
    if(pr_sv_prefix != ""){
        graph.save(pr_sv_prefix, pagerank_writer(),
                    false,      // do not gzip
                    true,       // save vertices
                    false);     // do not save edges
    }

    // his	
    dc.cout()<<"calculating structure holes using HIS algorithm"<<std::endl;
    graph.transform_vertices(init_his);
    graphlab::omni_engine<his> sh_engine(dc, graph, exec_type, clopts);
    sh_engine.signal_all();
    sh_engine.start();

    const float sh_runtime = sh_engine.elapsed_seconds();
    dc.cout() << "finished his algorithm in "<<sh_runtime
	            << " seconds."<<std::endl;

    if(sh_sv_prefix != ""){
        graph.save(sh_sv_prefix, his_writer(),
                    false,
                    true,
                    false);
    }
    

    graphlab::mpi_tools::finalize();

	get_all_answer();

    return EXIT_SUCCESS;
}


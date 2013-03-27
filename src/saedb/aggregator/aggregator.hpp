#ifndef xcode_sae_iaggregator_h
#define xcode_sae_iaggregator_h
namespace saedb {
    class aggregator {
    public:
        // initial accumulate value
        virtual void init(void*) {};
        
        // reduce with another data into a sigle value
        virtual void reduce(void*) {};
        
        // return the final aggregated data. read only.
        virtual void* data() const { return nullptr;};
        
        virtual ~aggregator() {};
    };
}
#endif
#ifndef CAFFE_NET_HPP_
#define CAFFE_NET_HPP_

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/layer.hpp"


// add
//#include "caffe/util/io.hpp"
//#include "caffe/util/upgrade_proto.hpp"
//#include "caffe/proto/caffe.pb.h"

namespace caffe {
    
    /**
     * @brief Connects Layer%s together into a directed acyclic graph (DAG)
     *        specified by a NetParameter.
     *
     * TODO(dox): more thorough description.
     */
    template <typename Dtype>
    class Net {
    public:
        /*
        explicit Net(const NetParameter& param, const Net* root_net = NULL);
        explicit Net(const string& param_file, Phase phase,
                     const Net* root_net = NULL);
        
        /// @brief Initialize a network with a NetParameter.
        void Init(const NetParameter& param);
        
        // ------ add by tangyuan, 2016-11-26 ------
        //explicit Net(const string& param_string, const Net* root_net = NULL);
        explicit Net(const string& model_file, const Net* root_net = NULL);
        void CopyFrom(const shared_ptr<Net<Dtype> >& other);
        int LoadTrainedLayers(const std::string& save_file);
        int SaveTrainedLayers(const std::string& deploy_file, const std::string& save_file);
        
        */
        // ------ add by tangyuan, 2016-12-10 ------
        void CopyFrom(const std::string& m_file);
		void CopyFrom_int8(const std::string& m_file);

		//add by chigo for detect
        void CopyFrom_detect(const std::string& m_file);
		void CopyFrom_detect_int8(const std::string& m_file);

		//change data
		void blob_float2uchar(std::ostream &os, vector<shared_ptr<Blob<Dtype>>> &inBlobs);
		void blob_uchar2float(std::istream &is, vector<shared_ptr<Blob<Dtype>>> &outBlobs);
        
        
        //
        explicit Net( const Net* root_net = NULL): root_net_(root_net) {}
        virtual ~Net() {}
        
        /**
         * @brief Run Forward with the input Blob%s already fed separately.
         *
         * You can get the input blobs using input_blobs().
         */
        const vector<Blob<Dtype>*>& ForwardPrefilled(Dtype* loss = NULL);
        
        /**
         * The From and To variants of Forward and Backward operate on the
         * (topological) ordering by which the net is specified. For general DAG
         * networks, note that (1) computing from one layer to another might entail
         * extra computation on unrelated branches, and (2) computation starting in
         * the middle may be incorrect if all of the layers of a fan-in are not
         * included.
         */
        Dtype ForwardFromTo(int start, int end);
        Dtype ForwardFrom(int start);
        Dtype ForwardTo(int end);
        /// @brief Run forward using a set of bottom blobs, and return the result.
        const vector<Blob<Dtype>*>& Forward(const vector<Blob<Dtype>* > & bottom,
                                            Dtype* loss = NULL);
        
        
        /**
         * @brief Zeroes out the diffs of all net parameters.
         *        Should be run before Backward.
         */
        void ClearParamDiffs();
        

        
        /**
         * @brief Reshape all layers from bottom to top.
         *
         * This is useful to propagate changes to layer sizes without running
         * a forward pass, e.g. to compute output feature size.
         */
        void Reshape();
        

        
        /// @brief Updates the network weights based on the diff values computed.
        void Update();
        /**
         * @brief Shares weight data of owner blobs with shared blobs.
         *
         * Note: this is called by Net::Init, and thus should normally not be
         * called manually.
         */
        //void ShareWeights();
        
        /**
         * @brief For an already initialized net, implicitly copies (i.e., using no
         *        additional memory) the pre-trained layers from another Net.
         */
        //void ShareTrainedLayersWith(const Net* other);
        // For an already initialized net, CopyTrainedLayersFrom() copies the already
        // trained layers from another net parameter instance.
        /**
         * @brief For an already initialized net, copies the pre-trained layers from
         *        another Net.
         */
        
        // read real number from stream, such as int float double //
        template <typename T>
        static inline T read_real(std::istream &is)
        {
            T var;
            is.read( reinterpret_cast<char *>(&var), sizeof(T));
            return var;
        }
        
        // write real number to stream, such as int float double //
        template <typename T>
        static inline void write_real(std::ostream &os, T var)
        {
            os.write( reinterpret_cast<char *>(&var), sizeof(T));
        }
        
        // read real number array from stream, such as int* float* double* //
        template <typename T>
        static inline void read_real(std::istream &is, T *var, uint64_t len)
        {
            uint64_t sz = sizeof(T) * len;
            is.read( reinterpret_cast<char *>(var), sz);
        }
        
        // write real number array to stream, such as int* float* double* //
        template <typename T>
        static inline void write_real(std::ostream &os, T *var, uint64_t len)
        {
            uint64_t sz = sizeof(T) * len;
            os.write( reinterpret_cast<char *>(var), sz);
        }
        
        // read string //
        static inline void read_string(std::istream &is, std::string &str)
        {
            uint64_t sz = read_real<uint32_t>( is );
            str.resize(sz);
            if(sz)
            {
                is.read(&str.at(0), sz);
            }
        }
        
        // write string //
        static inline void write_string(std::ostream &os, const std::string &str)
        {
            write_real<uint32_t>(os, str.size());
            if(str.size())
            {
                os.write(&str[0], str.size());
            }
        }
        
        
        //
        //void CopyTrainedLayersFrom(const NetParameter& param);
        //void CopyTrainedLayersFrom(const string trained_filename);
        //void CopyTrainedLayersFromBinaryProto(const string trained_filename);
        //void CopyTrainedLayersFromHDF5(const string trained_filename);
        
        /// @brief Writes the net to a proto.
        //void ToProto(NetParameter* param, bool write_diff = false) const;
        /// @brief Writes the net to an HDF5 file.
        //void ToHDF5(const string& filename, bool write_diff = false) const;
        
        /// @brief returns the network name.
        inline const string& name() const { return name_; }
        /// @brief returns the layer names
        inline const vector<string>& layer_names() const { return layer_names_; }
        /// @brief returns the blob names
        inline const vector<string>& blob_names() const { return blob_names_; }
        /// @brief returns the blobs
        inline const vector<shared_ptr<Blob<Dtype> > >& blobs() const {
            return blobs_;
        }
        /// @brief returns the layers
        inline const vector<shared_ptr<Layer<Dtype> > >& layers() const {
            return layers_;
        }
        /// @brief returns the phase: TRAIN or TEST
        inline Phase_ phase() const { return phase_; }
        /**
         * @brief returns the bottom vecs for each layer -- usually you won't
         *        need this unless you do per-layer checks such as gradients.
         */
        inline const vector<vector<Blob<Dtype>*> >& bottom_vecs() const {
            return bottom_vecs_;
        }
        /**
         * @brief returns the top vecs for each layer -- usually you won't
         *        need this unless you do per-layer checks such as gradients.
         */
        inline const vector<vector<Blob<Dtype>*> >& top_vecs() const {
            return top_vecs_;
        }
        /// @brief returns the ids of the top blobs of layer i
        inline const vector<int> & top_ids(int i) const {
            //CHECK_GE(i, 0) << "Invalid layer id";
            //CHECK_LT(i, top_id_vecs_.size()) << "Invalid layer id";
            return top_id_vecs_[i];
        }
        /// @brief returns the ids of the bottom blobs of layer i
        inline const vector<int> & bottom_ids(int i) const {
            //CHECK_GE(i, 0) << "Invalid layer id";
            //CHECK_LT(i, bottom_id_vecs_.size()) << "Invalid layer id";
            return bottom_id_vecs_[i];
        }
        inline const vector<vector<bool> >& bottom_need_backward() const {
            return bottom_need_backward_;
        }
        inline const vector<Dtype>& blob_loss_weights() const {
            return blob_loss_weights_;
        }
        inline const vector<bool>& layer_need_backward() const {
            return layer_need_backward_;
        }
		
		inline const vector<bool>& blob_need_backward() const {
            return blob_need_backward_;
        }
        /*
        /// @brief returns the parameters
        inline const vector<shared_ptr<Blob<Dtype> > >& params() const {
            return params_;
        }
        inline const vector<Blob<Dtype>*>& learnable_params() const {
            return learnable_params_;
        }
        /// @brief returns the learnable parameter learning rate multipliers
        inline const vector<float>& params_lr() const { return params_lr_; }
        inline const vector<bool>& has_params_lr() const { return has_params_lr_; }
        /// @brief returns the learnable parameter decay multipliers
        inline const vector<float>& params_weight_decay() const {
            return params_weight_decay_;
        }
        inline const vector<bool>& has_params_decay() const {
            return has_params_decay_;
        }
        const map<string, int>& param_names_index() const {
            return param_names_index_;
        }
        inline const vector<int>& param_owners() const { return param_owners_; }
        inline const vector<string>& param_display_names() const {
            return param_display_names_;
        }
         */
        /// @brief Input and output blob numbers
        inline int num_inputs() const { return net_input_blobs_.size(); }
        inline int num_outputs() const { return net_output_blobs_.size(); }
        inline const vector<Blob<Dtype>*>& input_blobs() const {
            return net_input_blobs_;
        }
        inline const vector<Blob<Dtype>*>& output_blobs() const {
            return net_output_blobs_;
        }
        inline const vector<int>& input_blob_indices() const {
            return net_input_blob_indices_;
        }
        inline const vector<int>& output_blob_indices() const {
            return net_output_blob_indices_;
        }
        bool has_blob(const string& blob_name) const;
        const shared_ptr<Blob<Dtype> > blob_by_name(const string& blob_name) const;
        bool has_layer(const string& layer_name) const;
        const shared_ptr<Layer<Dtype> > layer_by_name(const string& layer_name) const;
        
        void set_debug_info(const bool value) { debug_info_ = value; }
        
        //
        inline const vector<int>& learnable_param_ids() const {
            return learnable_param_ids_;
        }
        inline const vector<pair<int, int> >& param_layer_indices() const {
            return param_layer_indices_;
        }
        inline const vector<vector<int> >& param_id_vecs() const {
            return param_id_vecs_;
        }
        inline const vector<vector<int> >& top_id_vecs() const {
            return top_id_vecs_;
        }
        inline const vector<vector<int> >& bottom_id_vecs() const {
            return bottom_id_vecs_;
        }
        const map<string, int>& layer_names_index() const {
            return layer_names_index_;
        }
        const map<string, int>& blob_names_index() const {
            return layer_names_index_;
        }
        // Helpers for Init.
        /*
        static void FilterNet(const NetParameter& param,
                              NetParameter* param_filtered);
        /// @brief return whether NetState state meets NetStateRule rule
        static bool StateMeetsRule(const NetState& state, const NetStateRule& rule,
                                   const string& layer_name);
        */
    protected:
        // Helpers for Init.
        /// @brief Append a new input or top blob to the net.
        //void AppendTop(const NetParameter& param, const int layer_id,
        //               const int top_id, set<string>* available_blobs,
        //               map<string, int>* blob_name_to_idx);
        /// @brief Append a new bottom blob to the net.
        //int AppendBottom(const NetParameter& param, const int layer_id,
        //                 const int bottom_id, set<string>* available_blobs,
        //                 map<string, int>* blob_name_to_idx);
        /// @brief Append a new parameter blob to the net.
        //void AppendParam(const NetParameter& param, const int layer_id,
        //                 const int param_id);
        
        /// @brief Helper for displaying debug info in Forward about input Blobs.
        void InputDebugInfo(const int layer_id);
        /// @brief Helper for displaying debug info in Forward.
        void ForwardDebugInfo(const int layer_id);
        /// @brief Helper for displaying debug info in Backward.
        //void BackwardDebugInfo(const int layer_id);
        /// @brief Helper for displaying debug info in Update.
        void UpdateDebugInfo(const int param_id);
        
        /// @brief The network name
        string name_;
        /// @brief The phase: TRAIN or TEST
        Phase_ phase_;
        /// @brief Individual layers in the net
        vector<shared_ptr<Layer<Dtype> > > layers_;
        vector<string> layer_names_;
        map<string, int> layer_names_index_;
        vector<bool> layer_need_backward_;
        /// @brief the blobs storing intermediate results between the layer.
        vector<shared_ptr<Blob<Dtype> > > blobs_;
        vector<string> blob_names_;
        map<string, int> blob_names_index_;
        vector<bool> blob_need_backward_;
        /// bottom_vecs stores the vectors containing the input for each layer.
        /// They don't actually host the blobs (blobs_ does), so we simply store
        /// pointers.
        vector<vector<Blob<Dtype>*> > bottom_vecs_;
        vector<vector<int> > bottom_id_vecs_;
        vector<vector<bool> > bottom_need_backward_;
        /// top_vecs stores the vectors containing the output for each layer
        vector<vector<Blob<Dtype>*> > top_vecs_;
        vector<vector<int> > top_id_vecs_;
        /// Vector of weight in the loss (or objective) function of each net blob,
        /// indexed by blob_id.
        vector<Dtype> blob_loss_weights_;
        vector<vector<int> > param_id_vecs_;
        vector<int> param_owners_;
        vector<string> param_display_names_;
        vector<pair<int, int> > param_layer_indices_;
        map<string, int> param_names_index_;
        /// blob indices for the input and the output of the net
        vector<int> net_input_blob_indices_;
        vector<int> net_output_blob_indices_;
        vector<Blob<Dtype>*> net_input_blobs_;
        vector<Blob<Dtype>*> net_output_blobs_;
        /// The parameters in the network.
        vector<shared_ptr<Blob<Dtype> > > params_;
        vector<Blob<Dtype>*> learnable_params_;
        /**
         * The mapping from params_ -> learnable_params_: we have
         * learnable_param_ids_.size() == params_.size(),
         * and learnable_params_[learnable_param_ids_[i]] == params_[i].get()
         * if and only if params_[i] is an "owner"; otherwise, params_[i] is a sharer
         * and learnable_params_[learnable_param_ids_[i]] gives its owner.
         */
        vector<int> learnable_param_ids_;
        /// the learning rate multipliers for learnable_params_
        vector<float> params_lr_;
        vector<bool> has_params_lr_;
        /// the weight decay multipliers for learnable_params_
        vector<float> params_weight_decay_;
        vector<bool> has_params_decay_;
        /// The bytes of memory used by this net
        uint64_t memory_used_;
        /// Whether to compute and display debug info for the net.
        bool debug_info_;
        /// The root net that actually holds the shared layers in data parallelism
        const Net* const root_net_;
        DISABLE_COPY_AND_ASSIGN(Net);
    };
    
    
}  // namespace caffe

#endif  // CAFFE_NET_HPP_

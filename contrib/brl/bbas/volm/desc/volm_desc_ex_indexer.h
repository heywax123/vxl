// This is brl/bbas/volm/desc/volm_desc_ex_indexer.h
#ifndef volm_desc_ex_indexer_h_
#define volm_desc_ex_indexer_h_
// \file
// \brief  A class to contain the observable scene from each hypothesized location, each location has a volm_ex_descriptor
//
// \author Yi Dong
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include "volm_desc_indexer.h"
#include "volm_desc_ex.h"
#include <volm/volm_io_tools.h>
#include <volm/volm_buffered_index.h>


class volm_desc_ex_indexer : public volm_desc_indexer
{
public:

  //: constructor 
  volm_desc_ex_indexer(vcl_string const& index_folder,
                       vcl_string const& out_index_folder,
                       vcl_vector<double> const& radius,
                       unsigned const& norients = 3,
                       unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                       unsigned char const& initial_mag = 0);

  //: Destructor
  ~volm_desc_ex_indexer();

  //: load the volm_geo_index and initialize associated indices
  virtual bool load_tile_hypos(vcl_string const& geo_hypo_folder, int tile_id);

  //: extract histogram for a location
  virtual bool extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values);

  //: get the name of the indexer
  virtual vcl_string get_index_type_str() { return "existance"; }

  //: return the size of the histogram
  virtual unsigned layer_size() { return layer_size_; }


private:
  //: histogram parameters
  vcl_vector<double> radius_;
  unsigned         norients_;
  unsigned           nlands_;
  unsigned       layer_size_;
  unsigned char initial_mag_;
  

  //: directory where the previous created indices is stored
  vcl_string index_folder_;

  //: index


};

#endif  // volm_desc_ex_indexer_h_
